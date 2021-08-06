/* eslint-disable no-underscore-dangle */
import { Injectable } from '@angular/core';

enum LogLevel {
  log,
  debug,
  warn,
  error,
  fatal,
}

@Injectable({
  providedIn: 'root',
})
export class LoggerService {
  constructor() {}

  log(...args: any[]) {
    return this._log(LogLevel.log, args);
  }

  debug(...args: any[]) {
    return this._log(LogLevel.debug, args);
  }

  warn(...args: any[]) {
    return this._log(LogLevel.warn, args);
  }

  error(...args: any[]) {
    return this._log(LogLevel.error, args);
  }

  fatal(...args: any[]) {
    return this._log(LogLevel.fatal, args);
  }

  private _log(level: LogLevel, values: any[]) {
    let log = console.log;
    const levelName = LogLevel[level].toUpperCase();

    switch (level) {
      case LogLevel.log:
        log = (logStr: string) =>
          console.log('%c' + logStr, 'color: lightskyblue');
        break;
      case LogLevel.debug:
        log = (logStr: string) =>
          console.log('%c' + logStr, 'color: blueviolet');
        break;
      case LogLevel.warn:
        log = console.warn;
        break;
      case LogLevel.error:
        log = console.error;
        break;
      case LogLevel.fatal:
        log = (logStr: string) =>
          console.error('%c[FATAL] ' + logStr, 'color: orangered');
        break;
    }

    log(`[${levelName}]: ${String(values.join(' '))}`);
  }
}
