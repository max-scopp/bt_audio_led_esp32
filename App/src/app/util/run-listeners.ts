// eslint-disable-next-line @typescript-eslint/ban-types
export const runListeners = (listeners: Function[], args: any[] = []) =>
  listeners.forEach((listener) => listener(...args));
