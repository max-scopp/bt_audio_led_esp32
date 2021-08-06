import { Component, Input, NgZone, OnInit } from '@angular/core';
import { BLE } from '@ionic-native/ble/ngx';
import {
  BluetoothLE,
  DeviceInfo,
  RSSI,
  ScanStatus,
} from '@ionic-native/bluetooth-le/ngx';
import { Platform } from '@ionic/angular';
import { Observable, partition, Subscriber } from 'rxjs';
import {
  buffer,
  bufferCount,
  bufferTime,
  filter,
  share,
  take,
} from 'rxjs/operators';
import { BluetoothService } from 'src/app/services/bluetooth.service';
import { LoggerService } from 'src/app/services/logger.service';
import { displayError } from 'src/app/util/error';

interface ScannedDevice extends RSSI {
  invalidated: boolean;
  isBonded: boolean | null;
  isConnected: boolean;
}

@Component({
  selector: 'app-ble-devices',
  templateUrl: './ble-devices.component.html',
  styleUrls: ['./ble-devices.component.scss'],
})
export class BleDevicesComponent implements OnInit {
  @Input()
  targetService: string;

  @Input()
  modal: HTMLIonModalElement;

  devices: { [key: string]: ScannedDevice } = {};

  isInitializing = false;
  isScanning;
  toastCtrl: any;
  statusMessage: any;

  turnOffScanTimeoutId: any;

  constructor(
    private readonly logger: LoggerService,
    private readonly ble: BluetoothLE,
    private readonly platform: Platform,
    private ngZone: NgZone
  ) {}

  async ngOnInit() {
    this.scan();
  }

  dismissModal(device?: DeviceInfo) {
    this.stopScan();
    this.modal.dismiss(device || null);
  }

  async stopScan() {
    const { isScanning } = await this.ble.isScanning();

    if (isScanning) {
      await this.ble.stopScan();
    }

    if (this.isScanning) {
      this.isScanning = false;

      this.logger.log('Stopped scanning for Bluetooth LE Devices');
    }

    if (this.turnOffScanTimeoutId) {
      clearTimeout(this.turnOffScanTimeoutId);
    }
  }

  async toggleScan() {
    const willStartScan = !this.isScanning;

    await this.stopScan();

    if (willStartScan) {
      await this.scan();
    }
  }

  get deviceList() {
    return Object.values(this.devices);
  }

  getCssVarForRssi(device: ScannedDevice) {
    if (device.isBonded) {
      return 'var(--device-bonded)';
    }

    if (device.rssi > -30) {
      return 'var(--rssi-perfect)';
    }
    if (device.rssi > -67) {
      return 'var(--rssi-good)';
    }
    if (device.rssi > -70) {
      return 'var(--rssi-okay)';
    }
    if (device.rssi > -80) {
      return 'var(--rssi-bad)';
    }

    return 'var(--rssi-horrible)';
  }

  async autoStopScan() {
    this.turnOffScanTimeoutId = setTimeout(() => {
      this.ngZone.run(() => {
        this.turnOffScanTimeoutId = clearTimeout(this.turnOffScanTimeoutId);
        this.stopScan();
      });
    }, 5e3);
  }

  async scan() {
    this.ngZone.run(() => {
      Object.keys(this.devices).forEach(
        (key) => (this.devices[key].invalidated = true)
      );
    });

    await this.stopScan();

    const sharedScan$ = this.ble.startScan({}).pipe(share());

    const [deviceScan$, status$] = partition(
      sharedScan$,
      (scan) => scan.status === 'scanResult'
    );

    status$.subscribe((scan: ScanStatus) => {
      switch (scan.status) {
        case 'scanStarted':
          this.logger.log('Scanning for Bluetooth LE Devices');
          this.isScanning = true;
          this.autoStopScan();
          break;

        case 'scanStopped':
          this.stopScan();
          break;
      }
    });

    deviceScan$
      .pipe(
        filter((scan) => {
          const knownDevice = this.devices[scan.address];

          if (knownDevice?.invalidated) {
            return true;
          }

          return knownDevice?.rssi !== scan.rssi;
        })
        //bufferTime(1e5)
      )
      .subscribe(async (scan: ScanStatus) => {
        this.logger.log(`Found Bluetooth LE Device(s)`);

        let isBonded: boolean | null = null;

        if (this.platform.is('android')) {
          const scannedBond = await this.ble.isBonded({
            address: scan.address,
          });

          isBonded = scannedBond.isBonded;
        }

        const { isConnected = false } = isBonded
          ? await this.ble.isConnected({
              address: scan.address,
            })
          : {};

        this.ngZone.run(() => {
          this.devices[scan.address] = {
            ...scan,
            isBonded,
            isConnected,
            invalidated: false,
            rssi: scan.rssi,
          };
        });

        // this.ngZone.run(() => {
        //   scans.forEach(async (scan) => {
        //   });
        // });
      });
  }

  trackBy(_key, value) {
    return value.key;
  }

  onSelectDevice(device: DeviceInfo) {
    this.dismissModal(device);
  }
}
