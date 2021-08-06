import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { PairDevicePage } from './pair-device.page';

const routes: Routes = [
  {
    path: '',
    component: PairDevicePage,
  },
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule],
})
export class PairDevicePageRoutingModule {}
