export class ConnectionError extends Error {
  constructor(readonly message = 'Unable to connect.') {
    super(message);
  }
}
