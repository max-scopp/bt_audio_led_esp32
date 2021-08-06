export class DeviceChooserAborted extends Error {
  constructor(readonly message = 'No device was chosen.') {
    super(message);
  }
}
