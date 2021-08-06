import { TestBed } from '@angular/core/testing';

import { BluetoothletestService } from './bluetoothletest.service';

describe('BluetoothletestService', () => {
  let service: BluetoothletestService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(BluetoothletestService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
