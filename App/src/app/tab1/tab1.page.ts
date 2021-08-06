import { Component } from '@angular/core';
import { BluetoothService } from '../services/bluetooth.service';
import { LedService } from '../services/led.service';

@Component({
  selector: 'app-tab1',
  templateUrl: 'tab1.page.html',
  styleUrls: ['tab1.page.scss'],
})
export class Tab1Page {
  constructor(
    readonly bluetoothService: BluetoothService,
    readonly ledService: LedService
  ) {}
}
