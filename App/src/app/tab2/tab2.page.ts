import { Component, OnInit } from '@angular/core';
import {
  IonRange,
  PopoverController,
  ViewDidEnter,
  ViewWillEnter,
} from '@ionic/angular';
import { throttle } from 'throttle-debounce';
import { UnexpectedEmptyResponse } from '../exceptions/unexpected-empty-response';
import Comm from '../interfaces/comm';
import { LedService } from '../services/led.service';
import { LoggerService } from '../services/logger.service';
import { nice } from '../util/try-nice';
import { EffectsPopoverComponent } from './effects-popover/effects-popover.component';

@Component({
  selector: 'app-tab2',
  templateUrl: 'tab2.page.html',
  styleUrls: ['tab2.page.scss'],
})
export class Tab2Page implements ViewWillEnter {
  state: Comm.StateResponse | null = null;
  isRefreshing = false;

  onBrightnessChange = throttle(80, async (event) => {
    const response = await this.ledService.brightness(event.target.value);
    this.loggerService.log('setBrightness took ', response.took);
  });

  constructor(
    private readonly loggerService: LoggerService,
    private readonly popoverController: PopoverController,
    private readonly ledService: LedService
  ) {}

  ionViewWillEnter() {
    this.refresh();
  }

  get configKeys() {
    return Object.keys(this.state?.ec || {});
  }

  valueForKey(key: string) {
    const ec = this.state?.ec;

    if (ec) {
      return ec[key];
    }

    return null;
  }

  typeOfKey(key: string) {
    return typeof this.valueForKey(key);
  }

  onKeyChange(key: string, event: Event) {
    const target = event.target as HTMLIonRangeElement | HTMLIonToggleElement;

    this.ledService.configureEffect({
      [key]: target.tagName.includes('TOGGLE')
        ? // eslint-disable-next-line @typescript-eslint/dot-notation
          target['checked']
        : target.value,
    });
  }

  async refresh() {
    nice({
      try: async () => {
        this.isRefreshing = true;
        const response = await this.ledService.getState();

        if (response.data) {
          this.state = response.data;
        } else {
          throw new UnexpectedEmptyResponse();
        }
      },
      finally: () => (this.isRefreshing = false),
    });
  }

  async showEffectsPopover(event: Event) {
    const popover = await this.popoverController.create({
      event,
      component: EffectsPopoverComponent,
      translucent: true,
      showBackdrop: true,
    });

    popover.present();
  }
}
