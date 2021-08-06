/* eslint-disable prefer-arrow/prefer-arrow-functions */
import { ConnectionError } from 'src/app/exceptions/connection-error';
import { DeviceChooserAborted } from 'src/app/exceptions/device-chooser-abort';
import { NotConnected } from 'src/app/exceptions/not-connected';
import { UnexpectedState } from 'src/app/exceptions/unexpected-state';
import {
  BLEAdapter,
  BLECharacteristicsAdapter,
  BLEServiceAdapter,
  SendOptionsForAdapter,
} from 'src/app/interfaces/ble-adapter';
import Comm from 'src/app/interfaces/comm';
import { encode } from 'src/app/util/ble-encode-decode';
import { runListeners } from 'src/app/util/run-listeners';
import { exponentialBackoff } from 'src/app/util/time';
import { BluetoothService } from '../bluetooth.service';
import { LoggerService } from '../logger.service';

export class BLEWebAdapter implements BLEAdapter {
  connecting = false;

  // eslint-disable-next-line @typescript-eslint/ban-types
  listeners = {
    // eslint-disable-next-line @typescript-eslint/ban-types
    connect: [] as Function[],
    // eslint-disable-next-line @typescript-eslint/ban-types
    disconnect: [] as Function[],
  };

  private device: null | BluetoothDevice = null;

  constructor(private readonly logger: LoggerService) {}

  get connected() {
    return Promise.resolve(this.device?.gatt?.connected || false);
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
    const connect = async () => {
      const r = await this.device?.gatt?.connect();
      this.connecting = false;
      this.gattConnected();
      return r;
    };

    if (this.device) {
      return connect();
    }

    try {
      const reqOpts = {
        acceptAllDevices: true,
        optionalServices: [targetService],
        // filters: [
        //   {
        //     services: [],
        //   },
        // ],
      };

      this.connecting = true;
      this.logger.log('Requesting connection to BimmerLED Device...');
      this.device = await navigator.bluetooth.requestDevice(reqOpts);

      this.device.addEventListener(
        'gattserverdisconnected',
        this.gattDisconnected.bind(this)
      );

      return exponentialBackoff(
        3,
        300,
        async () => {
          this.logger.log('Connecting to Bluetooth Device... ');
          return connect();
        },
        () => {
          throw new ConnectionError('Failed to reconnect.');
        }
      );
    } catch (error) {
      this.connecting = false;

      // just web things..
      if (error instanceof DOMException) {
        if (error.message.match(/user\scancelled/i)) {
          throw new DeviceChooserAborted();
        }
      }

      throw error;
    }
  }

  async disconnect() {
    return this.device?.gatt?.disconnect();
  }

  async pairNew(targetService: string) {
    await this.disconnect();

    this.device = null;

    return this.connect(targetService);
  }

  async send<P = unknown, M extends Comm.Meta = any>(
    options: SendOptionsForAdapter<P, M>
  ): Promise<Comm.Envelope<P, M>> {
    const { spec, to: char, expectResponse } = options;

    if (!this.device?.gatt?.connected) {
      throw new NotConnected();
    }

    const payload: Comm.Envelope<P, M> = [
      spec.operation,
      new spec(),
      spec.meta,
    ];

    const encodedPayload = encode(payload);

    await char?.writeValue(encodedPayload, expectResponse);

    return payload;
  }

  async getService(serviceUUID: string): Promise<BLEServiceAdapter> {
    if (!this.device?.gatt?.connected) {
      throw new NotConnected();
    }

    const service = await this.device?.gatt?.getPrimaryService(serviceUUID);

    if (!service) {
      throw new UnexpectedState();
    }

    return {
      uuid: service.uuid,
      async getCharacteristic(
        characteristicsUUID: string
      ): Promise<BLECharacteristicsAdapter> {
        const char = await service.getCharacteristic(characteristicsUUID);

        return {
          uuid: char.uuid,
          lastValue: char.value?.buffer,

          async readValue() {
            const v = await char.readValue();
            return v.buffer;
          },

          async writeValue(v: Uint8Array, willRespond: boolean) {
            if (willRespond) {
              return char.writeValueWithResponse(v);
            }

            return char.writeValueWithoutResponse(v);
          },

          async startNotifications() {
            await char.startNotifications();
          },

          async stopNotifications() {
            await char.stopNotifications();
          },

          onNotify(notifyFn: (data: ArrayBuffer) => any) {
            const listener = (event: any) => {
              notifyFn(event.target?.value);
            };

            char.addEventListener('characteristicvaluechanged', listener);

            return () => {
              char.removeEventListener('characteristicvaluechanged', listener);
            };
          },
        };
      },
    };
  }

  private gattDisconnected() {
    runListeners(this.listeners.disconnect, Array.from(arguments));
  }

  private gattConnected() {
    runListeners(this.listeners.connect, Array.from(arguments));
  }
}
