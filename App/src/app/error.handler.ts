import { ErrorHandler } from '@angular/core';
import { LoggerService } from './services/logger.service';
import { displayError } from './util/error';

export class GlobalAppErrorHandler implements ErrorHandler {
  async handleError(gotError) {
    const possibleError = await gotError?.rejection;
    const errorToDisplay =
      possibleError?.error || possibleError?.errorMessage || possibleError;

    console.error(errorToDisplay);
    displayError(errorToDisplay);
  }
}
