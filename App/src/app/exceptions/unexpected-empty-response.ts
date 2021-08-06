export class UnexpectedEmptyResponse extends Error {
  constructor(
    readonly message = 'Controller responded with no data where data was expected.'
  ) {
    super(message);
  }
}
