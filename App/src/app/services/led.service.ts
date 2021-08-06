import { Injectable } from '@angular/core';
import { AlertController, ToastController } from '@ionic/angular';
import { filter } from 'rxjs/operators';
import Comm from '../interfaces/comm';
import { BluetoothService, HistoryEntry } from './bluetooth.service';

@Injectable({
  providedIn: 'root',
})
export class LedService {
  constructor(
    private readonly bluetooth: BluetoothService,
    private readonly alertController: AlertController,
    private readonly toastController: ToastController
  ) {
    this.setupMessageListener();
  }

  setupMessageListener() {
    this.bluetooth.history$
      .pipe(
        filter((n) => Boolean(!n.sent && n.valid)),
        filter((n) => Comm.Meta.of(n.value[2]).isMessage())
      )
      .subscribe((envelope: HistoryEntry<string, Comm.Meta>) => {
        // eslint-disable-next-line @typescript-eslint/dot-notation
        const message = envelope.value.length ? [1]['o'] : 'No message';
        this.handleControllerUserMessage(
          message,
          Comm.Meta.of(envelope.value[2])
        );
      });
  }

  async handleControllerUserMessage(message: string, meta: Comm.Meta) {
    const toast = await (meta.isException()
      ? this.toastController.create({
          header: 'Controller exception',
          color: 'danger',
          message,
          keyboardClose: true,
          duration: 2e3,
          buttons: ['OK'],
          position: 'top',
        })
      : this.toastController.create({
          header: message,
        }));

    toast.present();
  }

  async ping() {
    let success = false;
    const prompt = await this.alertController.create({
      header: 'Pinging...',
      subHeader: 'This may take a minute or two.',
      backdropDismiss: false,
    });

    prompt.present();

    try {
      const validateValue = Math.trunc(Math.random() * 1e6);

      const response = await this.bluetooth.send<Comm.PingResponse>({
        expectResponse: true,
        spec: class Ping extends Number {
          static operation = Comm.KnownOperation.ping;

          valueOf() {
            return validateValue;
          }
        },
      });

      success = response.data?.got === validateValue;

      prompt.header = 'Success';
      prompt.subHeader = 'Device pinged successfully';
      prompt.message =
        '<p>' +
        [
          'The BimmerLED controller was able to successfully respond with an unique response.',
          `The procedure took ${response.took}ms`,
        ].join('</p><p>') +
        '</p>';
      prompt.buttons = ['Nice!'];

      if (!success) {
        prompt.header = 'Unable to ping';
        prompt.subHeader = undefined;
        prompt.message =
          '<p>' +
          [
            'The target device was not found or could not establish a connection. ',
            'Ensure the BimmerLED controller is powered on.',
            'Then, try to pair the device again.',
          ].join('</p><p>') +
          '</p>';
        prompt.buttons = ['Got it'];
      }
    } catch (e) {
      prompt.dismiss();
      throw e;
    }
  }

  async getState() {
    return this.bluetooth.send<Comm.StateResponse>({
      expectResponse: true,
      spec: class GetState {
        static operation = Comm.KnownOperation.getState;
      },
    });
  }

  async getEffects() {
    return this.bluetooth.send<Comm.EffectsResponse>({
      expectResponse: true,
      spec: class GetEffects {
        static operation = Comm.KnownOperation.getEffects;
      },
    });
  }

  async setEffect(effectName: string) {
    return this.bluetooth.send({
      expectResponse: false,
      spec: class SetEffect extends String {
        static operation = Comm.KnownOperation.setEffect;
        toString() {
          return effectName;
        }
      },
    });
  }

  async configureEffect(changes: { [key: string]: any }) {
    return this.bluetooth.send({
      expectResponse: false,
      spec: class ConfigureEffect {
        static operation = Comm.KnownOperation.configureEffect;

        constructor() {
          Object.assign(this, changes);
        }
      },
    });
  }

  async brightness(newBrightness: number, smooth: boolean = false) {
    return this.bluetooth.send({
      expectResponse: false,
      spec: class SetBrightness extends Number {
        static operation = Comm.KnownOperation.setBrightness;
        static meta = Comm.Meta.of({
          smooth,
        });

        valueOf() {
          return newBrightness;
        }
      },
    });
  }
}
