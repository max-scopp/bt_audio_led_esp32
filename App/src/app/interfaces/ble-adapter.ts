import { LoggerService } from '../services/logger.service';
import Comm from './comm';

export type RemoveListenerFunction = () => void;

export interface SendOptionsForAdapter<P, M extends Comm.Meta> {
  spec: Comm.Operation<P, M>;
  to: BLECharacteristicsAdapter;
  expectResponse: boolean;
}

export interface BLEAdapter {
  readonly connecting: boolean;
  readonly connected: Promise<boolean>;

  connect(targetService: string): Promise<any>;
  pairNew(targetService: string): Promise<any>;
  disconnect(): Promise<void>;

  onConnected(connectedFn: () => any): RemoveListenerFunction;
  onDisconnected(disconnectFn: () => any): RemoveListenerFunction;

  send<P = unknown, M extends Comm.Meta = any>(
    options: SendOptionsForAdapter<P, M>
  ): Promise<Comm.Envelope<P, M>>;

  getService(serviceUUID: string): Promise<BLEServiceAdapter>;
}

export interface BLEServiceAdapter {
  uuid: string;
  getCharacteristic(
    characteristicsUUID: string
  ): Promise<BLECharacteristicsAdapter>;
}

export interface BLECharacteristicsAdapter {
  uuid: string;
  lastValue: ArrayBuffer | undefined;

  readValue(): Promise<ArrayBuffer>;

  /**
   * The `willRespond` flag is only needed for the protocol - it will not handle getting the response for you!
   * @param v
   * @param willRespond
   */
  writeValue(v: Uint8Array, willRespond: boolean): Promise<void>;

  startNotifications(): Promise<void>;
  stopNotifications(): Promise<void>;

  onNotify(notifyFn: (data: ArrayBuffer) => any): RemoveListenerFunction;
}
