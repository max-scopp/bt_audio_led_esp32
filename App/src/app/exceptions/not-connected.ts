export class NotConnected extends Error {
  constructor(readonly message = 'Not connected to a device.') {
    super(message);
  }
}
