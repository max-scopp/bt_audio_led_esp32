// This function keeps calling "toTry" until promise resolves or has
// retried "max" number of times. First retry has a delay of "delay" seconds.
// "success" is called upon success.
// TODO: Refactor last 3 args to object
export const exponentialBackoff = async (
  max: number,
  delay: number,
  // eslint-disable-next-line @typescript-eslint/ban-types
  toTry: Function,
  // eslint-disable-next-line @typescript-eslint/ban-types
  fail: Function
) =>
  new Promise(async (resolve, reject) => {
    try {
      resolve(await toTry());
    } catch (error) {
      if (max === 0) {
        reject(fail(error));
      }

      console.warn('exponentialBackoff(): Failed, retrying... ', {
        delay,
        error,
      });
      setTimeout(async () => {
        const result = await exponentialBackoff(--max, delay * 2, toTry, fail);
        resolve(result);
      }, delay);
    }
  });
