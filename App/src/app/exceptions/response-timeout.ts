export class ResponseTimeoutError extends Error {
  constructor(
    readonly message = 'Controller did not notify the required response within the expected time frame.'
  ) {
    super(message);
  }
}
