import { BLE } from '@ionic-native/ble/ngx';
import { ErrorHandler, NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { RouteReuseStrategy } from '@angular/router';

import { IonicModule, IonicRouteStrategy } from '@ionic/angular';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { NativeModule } from './native/native.module';
import { GlobalAppErrorHandler } from './error.handler';
import { LoggerService } from './services/logger.service';
import { BluetoothLE } from '@ionic-native/bluetooth-le/ngx';

@NgModule({
  declarations: [AppComponent],
  entryComponents: [],
  imports: [
    BrowserModule,
    IonicModule.forRoot(),
    NativeModule,
    AppRoutingModule,
  ],
  providers: [
    BluetoothLE,
    BLE,
    { provide: RouteReuseStrategy, useClass: IonicRouteStrategy },
    { provide: ErrorHandler, useClass: GlobalAppErrorHandler },
  ],
  bootstrap: [AppComponent],
})
export class AppModule {}
