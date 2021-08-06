import { Component, OnInit } from '@angular/core';
import { BluetoothService } from '../services/bluetooth.service';

@Component({
  selector: 'app-pair-device',
  templateUrl: './pair-device.page.html',
  styleUrls: ['./pair-device.page.scss'],
})
export class PairDevicePage implements OnInit {
  constructor(readonly bluetoothService: BluetoothService) {}

  ngOnInit() {}
}
