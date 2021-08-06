import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { IonicModule } from '@ionic/angular';

import { PairDevicePageRoutingModule } from './pair-device-routing.module';

import { PairDevicePage } from './pair-device.page';

@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    IonicModule,
    PairDevicePageRoutingModule
  ],
  declarations: [PairDevicePage]
})
export class PairDevicePageModule {}
