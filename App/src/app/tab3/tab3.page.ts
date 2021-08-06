import {
  Component,
  ElementRef,
  TemplateRef,
  ViewChild,
  ViewRef,
} from '@angular/core';
import { AlertController, IonVirtualScroll } from '@ionic/angular';
import Comm from '../interfaces/comm';
import { BluetoothService, HistoryEntry } from '../services/bluetooth.service';

@Component({
  selector: 'app-tab3',
  templateUrl: 'tab3.page.html',
  styleUrls: ['tab3.page.scss'],
})
export class Tab3Page {
  @ViewChild('log')
  scroller: ElementRef<any>;

  commandHistory: HistoryEntry<any, any>[] = [];

  constructor(
    readonly bluetoothService: BluetoothService,
    private readonly alertController: AlertController
  ) {
    this.bluetoothService.history$.subscribe((notif) => {
      this.commandHistory = [notif, ...this.commandHistory];
    });
  }

  trackByFn = (index: number, item: HistoryEntry<any, any>) => item.at;

  itemHeightFn = () => 22;

  showCompleteEntry = async (atIndex: number) => {
    const entry = this.commandHistory[atIndex];
    const prompt = await this.alertController.create({
      header: `BLE Packet`,
      subHeader: `${!entry.valid ? 'INVALID' : ''} @ ${entry.at}`,
      message: JSON.stringify(entry.value),
      backdropDismiss: true,
      buttons: ['OK'],
    });

    prompt.present();
  };
}
