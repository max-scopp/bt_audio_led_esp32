export class NoDeviceConnected extends Error {
  constructor(
    readonly message = 'No device is connected. Try pairing a new device.'
  ) {
    super(message);
  }
}
