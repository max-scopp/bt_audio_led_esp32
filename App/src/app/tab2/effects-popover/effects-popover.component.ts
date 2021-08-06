import { Component, Input, OnInit } from '@angular/core';
import { UnexpectedEmptyResponse } from 'src/app/exceptions/unexpected-empty-response';
import Comm from 'src/app/interfaces/comm';
import { LedService } from 'src/app/services/led.service';
import { displayError } from 'src/app/util/error';
import { nice } from 'src/app/util/try-nice';

@Component({
  selector: 'app-effects-popover',
  templateUrl: './effects-popover.component.html',
  styleUrls: ['./effects-popover.component.scss'],
})
export class EffectsPopoverComponent implements OnInit {
  @Input()
  popover: HTMLIonPopoverElement;

  items: Comm.EffectsResponse['e'] = [];
  isLoading = true;

  constructor(private readonly ledService: LedService) {}

  async ngOnInit() {
    nice({
      try: async () => {
        const response = await this.ledService.getEffects();

        if (response.data) {
          this.items = response.data.e;
          this.isLoading = false;
        } else {
          throw new UnexpectedEmptyResponse();
        }
      },
      catch: () => {
        this.popover.remove();
      },
    });
  }

  async activateEffect(effectName: string) {
    nice({
      try: () => this.ledService.setEffect(effectName),
      finally: () => this.popover.dismiss(),
    });
  }
}
