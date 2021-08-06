import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { BleDevicesComponent } from './ble-devices/ble-devices.component';
import { IonicModule } from '@ionic/angular';

@NgModule({
  declarations: [BleDevicesComponent],
  imports: [CommonModule, IonicModule],
})
export class NativeModule {}
