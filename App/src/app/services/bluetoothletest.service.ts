import { Injectable } from '@angular/core';
import { BluetoothLE } from '@ionic-native/bluetooth-le/ngx';
import { Platform } from '@ionic/angular';
import { encode } from '../util/ble-encode-decode';
import { BluetoothService } from './bluetooth.service';

@Injectable({
  providedIn: 'root',
})
export class BluetoothletestService {
  constructor(public bluetoothle: BluetoothLE, public plt: Platform) {
    this.plt.ready().then((readySource) => {
      console.log('Platform ready from', readySource);

      const inited$ = this.bluetoothle.initialize();

      const devices = {};

      inited$.subscribe(async (ble) => {
        console.log({ ble }); // logs 'enabled'

        if (ble.status !== 'enabled') {
          throw new Error(ble.status);
        }

        try {
          await this.bluetoothle.stopScan();
        } catch (e) {}

        const scan$ = this.bluetoothle.startScan({});

        scan$.subscribe(async (got) => {
          devices[got.address] = got;
          if (String(got.name).includes('E46')) {
            try {
              await this.bluetoothle.stopScan();
              await this.bluetoothle.close({
                address: got.address,
              });
            } catch (e) {
              console.warn(e);
            }
            console.log('Connecting...');
            const connection$ = this.bluetoothle.connect({
              address: got.address,
            });

            connection$.subscribe(async (v) => {
              const dev = await this.bluetoothle.discover({
                address: got.address,
              });

              console.log({ dev, v });

              const vaaa = encode([]);

              const wrote = await this.bluetoothle.write({
                address: v.address,
                characteristic: BluetoothService.targetCharacteristic,
                service: BluetoothService.targetService,
                value: this.bluetoothle.bytesToEncodedString(vaaa),
              });

              console.log({ wrote });
            });
          }
        });
      });
    });
  }
}
