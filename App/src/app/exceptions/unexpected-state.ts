export class UnexpectedState extends Error {
  constructor(
    readonly message = 'An application service went into an unexpected state.'
  ) {
    super(message);
  }
}
