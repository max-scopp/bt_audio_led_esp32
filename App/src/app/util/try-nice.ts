import { displayError } from './error';

interface TryOptions {
  try: AnyFunction;
  catch?: <E extends Error>(error?: E, ...args: any[]) => any;
  finally?: AnyFunction;
}

export const nice = (opts: TryOptions) => {
  try {
    return opts.try();
  } catch (e) {
    if (opts.catch) {
      opts.catch(e);
    }

    displayError(e);
  } finally {
    if (opts.finally) {
      opts.finally();
    }
  }
};
