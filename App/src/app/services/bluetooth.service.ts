import { Injectable } from '@angular/core';
import { BLE } from '@ionic-native/ble/ngx';
import { BluetoothLE } from '@ionic-native/bluetooth-le/ngx';
import { ModalController, Platform } from '@ionic/angular';
import { EMPTY, Observable, of, Subject, throwError } from 'rxjs';
import { filter, map, take, timeoutWith, timeout } from 'rxjs/operators';
import Comm from 'src/app/interfaces/comm';
import { ConnectionError } from '../exceptions/connection-error';
import { DeviceChooserAborted } from '../exceptions/device-chooser-abort';
import { ResponseTimeoutError } from '../exceptions/response-timeout';
import { UnexpectedState } from '../exceptions/unexpected-state';
import { BLEAdapter } from '../interfaces/ble-adapter';
import { displayAlert } from '../util/alert';
import { decode } from '../util/ble-encode-decode';
import { displayError } from '../util/error';
import { BLENativeAdapter } from './bluetooth-adapters/ble-native';
import { BLEWebAdapter } from './bluetooth-adapters/ble-web';
import { BluetoothletestService } from './bluetoothletest.service';
import { LoggerService } from './logger.service';

export interface SendOptions<P, M extends Comm.Meta> {
  spec: Comm.Operation<P, M>;
  expectResponse: boolean;
}

export interface HistoryEntry<P, M> {
  value: Comm.Envelope<P, M>;
  at: Date;
  valid?: boolean;

  /**
   * if this is true, the client send the package, not the controller.
   */
  sent?: true;
}

@Injectable({
  providedIn: 'root',
})
export class BluetoothService {
  static targetService = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
  static targetCharacteristic = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';

  private readonly historySubject$ = new Subject<
    HistoryEntry<unknown, unknown>
  >();

  // eslint-disable-next-line @typescript-eslint/member-ordering
  readonly history$ = this.historySubject$.asObservable();

  private impl: BLEAdapter;
  private doesNotify: boolean;

  private _connected: boolean;

  get connected() {
    // eslint-disable-next-line no-underscore-dangle
    return this._connected || false;
  }

  get connecting() {
    return this.impl?.connecting || false;
  }

  constructor(
    private readonly logger: LoggerService,
    private readonly modalController: ModalController,
    public readonly ble: BluetoothLE,
    public readonly platform: Platform
  ) {}

  get isReady() {
    return !!this.impl;
  }

  get receivesNotifications(): boolean {
    return this.doesNotify;
  }

  async getTargetCharcteristic() {
    await this.checkState();

    const service = await this.impl.getService(BluetoothService.targetService);

    return service.getCharacteristic(BluetoothService.targetCharacteristic);
  }

  async send<R, P = unknown, M extends Comm.Meta = any>(
    options: SendOptions<P, M>
  ): Promise<Comm.EnvelopeResponse<R, M>> {
    await this.checkState();

    const to = await this.getTargetCharcteristic();

    const start = Date.now();

    const sentPayload = await this.impl.send({
      ...options,
      to,
    });

    this.pushHistory(sentPayload, true);

    if (options.expectResponse) {
      const response = await this.readNextValue(options.spec.operation);
      return new Comm.EnvelopeResponse(response, start);
    }

    return new Comm.EnvelopeResponse([], start);
  }

  async pairNew() {
    await this.checkState();

    try {
      return await this.impl.pairNew(BluetoothService.targetService);
    } catch (e) {
      if (e instanceof DeviceChooserAborted) {
        this.logger.log('User did not choose a new device, ignoring.');
        return;
      }
      throw e;
    }
  }

  async tryReconnect() {
    await this.checkState();

    this.logger.log('Reconnecting...');
    await this.impl.connect(BluetoothService.targetService);
  }

  protected async handleNotify(data: ArrayBuffer) {
    const decodedValue = decode(data);
    this.pushHistory(decodedValue, false);
  }

  protected async setupBLE() {
    const readySource = await this.platform.ready();
    this.logger.log('Platform ready:', readySource);

    try {
      const isCap = this.platform.is('capacitor');

      if (isCap) {
        const [{ isEnabled }, { isInitialized }] = await Promise.all([
          this.ble.isEnabled(),
          this.ble.isInitialized(),
        ]);

        if (!isEnabled) {
          try {
            const gotPermission = await this.ble.requestPermission();

            if (!gotPermission) {
              displayAlert(
                'Permissions required',
                [
                  'The app requires Bluetooth and Location Permissions.',
                  'The Location is required for close-by bluetooth scans.',
                  'The app will not work properly until you allow these permissions in your settings.',
                ].join(' ')
              );
            }
          } catch (e) {}

          const { status } = await this.ble
            .initialize()
            .pipe(take(1))
            .toPromise();

          debugger;
        }
      }
    } catch (errorMessage) {
      throw new ConnectionError(errorMessage);
    }

    switch (readySource) {
      case 'dom': {
        return this.setupWebBLE();
      }
      default:
        return this.setupNativeBLE();
    }
  }

  protected async setupWebBLE() {
    this.impl = new BLEWebAdapter(this.logger);

    this.impl.onConnected(this.onConnect.bind(this));
    this.impl.onDisconnected(this.onDisconnected.bind(this));
  }

  protected async setupNativeBLE() {
    try {
      await this.ble.isEnabled();

      this.impl = new BLENativeAdapter(
        this.logger,
        this.ble,
        this.platform,
        this.modalController
      );

      this.impl.onConnected(this.onConnect.bind(this));
      this.impl.onDisconnected(this.onDisconnected.bind(this));
    } catch (e) {
      displayError(e);
    }
  }

  protected async checkState() {
    if (!this.isReady) {
      await this.setupBLE();
    }
  }

  protected async onConnect() {
    // eslint-disable-next-line no-underscore-dangle
    this._connected = true;

    this.logger.log('Bluetooth Device connected.');

    this.listenForNotifications();
  }

  protected async listenForNotifications() {
    const char = await this.getTargetCharcteristic();

    if (!char) {
      throw new UnexpectedState();
    }

    await char.startNotifications();
    await char.onNotify(this.handleNotify.bind(this));

    this.doesNotify = true;
    this.logger.log('Listening on notifications...');
  }

  protected async onDisconnected() {
    // eslint-disable-next-line no-underscore-dangle
    this._connected = false;
    this.doesNotify = false;

    this.logger.log('Bluetooth Device disconnected');

    this.tryReconnect();
  }

  protected pushHistory<P, M, V extends Comm.Envelope<P, M>>(
    value: V,
    sentFromMe: boolean
  ) {
    this.logger.debug(`${sentFromMe ? '>' : '<'} ${JSON.stringify(value)}`);

    this.historySubject$.next({
      valid: typeof value === 'object',
      at: new Date(),
      sent: sentFromMe ? true : undefined,
      value,
    });
  }

  protected async readNextValue(targetOperationId: number, due = 500) {
    return this.history$
      .pipe(
        filter((n) => !n.sent),
        filter((n) => (n.value as Array<Comm.Any>)[0] === targetOperationId),
        take(1),
        map((n) => n.value),
        timeoutWith(due, throwError(new ResponseTimeoutError()))
      )
      .toPromise();
  }
}
