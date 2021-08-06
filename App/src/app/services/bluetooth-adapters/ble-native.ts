/* eslint-disable prefer-arrow/prefer-arrow-functions */
import {
  BluetoothLE,
  Device,
  DeviceInfo,
  OperationResult,
} from '@ionic-native/bluetooth-le/ngx';
import { ModalController, Platform } from '@ionic/angular';
import { Observable, of, Subject } from 'rxjs';
import { take } from 'rxjs/operators';
import { ConnectionError } from 'src/app/exceptions/connection-error';
import { DeviceChooserAborted } from 'src/app/exceptions/device-chooser-abort';
import { NotConnected } from 'src/app/exceptions/not-connected';
import { UnexpectedState } from 'src/app/exceptions/unexpected-state';
import {
  BLEAdapter,
  BLEServiceAdapter,
  SendOptionsForAdapter,
} from 'src/app/interfaces/ble-adapter';
import Comm from 'src/app/interfaces/comm';
import { BleDevicesComponent } from 'src/app/native/ble-devices/ble-devices.component';
import { encode } from 'src/app/util/ble-encode-decode';
import { runListeners } from 'src/app/util/run-listeners';
import { exponentialBackoff } from 'src/app/util/time';
import { LoggerService } from '../logger.service';

export class BLENativeAdapter implements BLEAdapter {
  connecting = false;

  // eslint-disable-next-line @typescript-eslint/ban-types
  listeners = {
    // eslint-disable-next-line @typescript-eslint/ban-types
    connect: [] as Function[],
    // eslint-disable-next-line @typescript-eslint/ban-types
    disconnect: [] as Function[],
  };

  device: Device | null = null;

  constructor(
    private readonly logger: LoggerService,
    private readonly ble: BluetoothLE,
    private readonly platform: Platform,
    private readonly modalController: ModalController
  ) {}

  get connected() {
    return (async () => {
      if (!this.device?.address) {
        return false;
      }

      try {
        const status = await this.ble.isConnected({
          address: this.device.address,
        });

        return status.isConnected;
      } catch (e) {
        return false;
      }
    })();
  }

  onConnected(connectedFn: () => any) {
    this.listeners.connect.push(connectedFn);

    return () => {
      this.listeners.connect = this.listeners.connect.filter(
        (fn) => fn !== connectedFn
      );
    };
  }

  onDisconnected(disconnectFn: () => any) {
    this.listeners.disconnect.push(disconnectFn);

    return () => {
      this.listeners.disconnect = this.listeners.disconnect.filter(
        (fn) => fn !== disconnectFn
      );
    };
  }

  async connect(targetService: string) {
    const connect = async (device: DeviceInfo) => {
      const { address } = device;
      this.connecting = true;
      let connected$: Observable<DeviceInfo>;

      try {
        this.logger.log('Probing device...');

        const { wasConnected } = await this.ble.wasConnected({
          address,
        });

        const { isConnected } = wasConnected
          ? await this.ble.isConnected({ address })
          : { isConnected: false };

        if (!isConnected && wasConnected) {
          this.logger.log('Re-Connecting to device...');
          connected$ = this.ble.reconnect({ address });
        } else if (!isConnected) {
          this.logger.log('Connecting to device...');
          connected$ = this.ble.connect({
            address,
          });
        } else {
          this.logger.log('Device already connected!');
          connected$ = of(device);
        }

        await connected$.pipe(take(1)).toPromise();

        this.device = await this.ble.discover({
          address,
        });
      } finally {
        this.connecting = false;
      }

      this.gattConnected();
    };

    if (this.device) {
      return connect(this.device);
    }

    const modal = await this.modalController.create({
      swipeToClose: true,
      component: BleDevicesComponent,
      componentProps: {
        targetService,
      },
    });

    await modal.present();

    const { data: selectedDevice } =
      await modal.onWillDismiss<DeviceInfo | null>();

    if (!selectedDevice) {
      throw new DeviceChooserAborted();
    }

    return exponentialBackoff(
      3,
      300,
      async () => connect(selectedDevice),
      (err: Error) => {
        throw new ConnectionError('Failed to (re)connect: ' + err.message);
      }
    );
  }

  async disconnect() {
    if (!this.device?.address) {
      throw new UnexpectedState();
    }

    if (await this.connected) {
      const fromNative = await this.ble.disconnect({
        address: this.device.address,
      });

      if (fromNative.status !== 'disconnected') {
        throw new ConnectionError('Unable to disconnect.');
      }

      return;
    }
  }

  async pairNew(targetService: string) {
    if (this.device?.address) {
      await this.disconnect();

      const fromNative = await this.ble.close({ address: this.device.address });

      if (fromNative.status !== 'closed') {
        throw new ConnectionError(
          [
            'Unable to pair a new device, existing device could not be closed.',
            'Try removing the device from your device settings, then try again.',
          ].join(' ')
        );
      }
    }

    this.device = null;
    return this.connect(targetService);
  }

  async send<P = unknown, M extends Comm.Meta = any>(
    options: SendOptionsForAdapter<P, M>
  ): Promise<Comm.Envelope<P, M>> {
    const { spec, to: char, expectResponse } = options;

    if (!(await this.connected)) {
      throw new NotConnected();
    }

    const payload: Comm.Envelope<P, M> = [
      spec.operation,
      new spec(),
      spec.meta,
    ];

    const encodedPayload = encode(payload);

    char.writeValue(encodedPayload, expectResponse);

    return payload;
  }

  async getService(serviceUUID: string): Promise<BLEServiceAdapter> {
    if (!(await this.connected)) {
      throw new NotConnected();
    }

    if (!this.device?.address) {
      throw new UnexpectedState();
    }

    const subscriptionSubject$: Subject<OperationResult> = new Subject();

    return {
      uuid: this.device.address,
      getCharacteristic: async (characteristicsUUID: string) => ({
        uuid: characteristicsUUID,
        lastValue: undefined,
        readValue: async () => {
          if (!this.device?.address) {
            throw new UnexpectedState();
          }

          const result = await this.ble.read({
            address: this.device.address,
            characteristic: characteristicsUUID,
            service: serviceUUID,
          });

          return this.ble.stringToBytes(result.value);
        },
        writeValue: async (v: Uint8Array, willRespond: boolean) => {
          if (!this.device?.address) {
            throw new UnexpectedState();
          }

          const out = {
            address: this.device.address,
            characteristic: characteristicsUUID,
            service: serviceUUID,
            value: this.ble.bytesToEncodedString(v),
          };

          await this.ble.write(out);

          return; // writeQ is not supported, yet
          if (willRespond) {
          } else {
            // await this.ble.writeQ(out);
          }
        },
        startNotifications: async () => {
          if (!this.device?.address) {
            throw new UnexpectedState();
          }
          try {
            await this.ble.unsubscribe({
              address: this.device.address,
              service: serviceUUID,
              characteristic: characteristicsUUID,
            });
          } catch (e) {
            console.warn(e);
          }

          const subscription$ = this.ble.subscribe({
            address: this.device.address,
            characteristic: characteristicsUUID,
            service: serviceUUID,
          });

          return new Promise((resolve, reject) => {
            subscription$.subscribe((opResult) => {
              switch (opResult.status) {
                case 'subscribed':
                  resolve();
                  break;
                case 'subscribedResult':
                  subscriptionSubject$.next(opResult);
                  break;
              }
            }, reject);
          });
        },
        stopNotifications: async () => {
          if (!this.device?.address) {
            throw new UnexpectedState();
          }

          this.ble.unsubscribe({
            address: this.device.address,
            characteristic: characteristicsUUID,
            service: serviceUUID,
          });
        },

        onNotify: (notifyFn: (data: ArrayBuffer) => any) => {
          const observer$ = subscriptionSubject$.asObservable();

          observer$.subscribe((opResult) => {
            notifyFn(this.ble.encodedStringToBytes(opResult.value).buffer);
          });

          return () => {
            throw new Error('Method not implemented.');
          };
        },
      }),
    };
  }

  private async gattDisconnected() {
    runListeners(this.listeners.disconnect, Array.from(arguments));
  }

  private async gattConnected() {
    if (!this.device?.address) {
      throw new UnexpectedState();
    }

    if (this.platform.is('android')) {
      const { mtu } = await this.ble.mtu({
        address: this.device.address,
        mtu: 100, // this is the default for iPhone 6s and above.
      });

      this.logger.log(
        `Fixed MTU size for android specifically, it's now ${mtu}.`
      );
    }

    runListeners(this.listeners.connect, Array.from(arguments));
  }
}
