import { TestBed } from '@angular/core/testing';

import { LedService } from './led.service';

describe('LedService', () => {
  let service: LedService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(LedService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
