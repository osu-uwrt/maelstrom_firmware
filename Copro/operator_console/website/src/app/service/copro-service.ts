import { Injectable, EventEmitter } from "@angular/core";
import { Observable, Subject, throwError } from "rxjs";
import { catchError, map, tap, filter } from "rxjs/operators";
import { delay, share } from "rxjs/operators";
import { HttpClient, HttpParams } from "@angular/common/http";
import { Command } from "selenium-webdriver";
import { connectableObservableDescriptor } from "rxjs/internal/observable/ConnectableObservable";

const COPRO_URL = "http://localhost:2000";

@Injectable()
export class CoproService {
  coproDisconnected = new EventEmitter();
  connected = true;

  constructor(private http: HttpClient) {}

  setMoboPower(state: boolean): Observable<any> {
    return this.command(0, state ? 1 : 0);
  }

  setJetsonPower(state: boolean): Observable<any> {
    return this.command(1, state ? 1 : 0);
  }

  setThrusterPower(state: boolean): Observable<any> {
    return this.command(2, state ? 1 : 0);
  }

  setPeltierPower(state: boolean): Observable<any> {
    return this.command(3, state ? 1 : 0);
  }

  getBatVoltages(): Observable<number[]> {
    return this.command(4).pipe(
      map(x => {
        return [(x[0] * 256 + x[1]) / 100.0, (x[2] * 256 + x[3]) / 100.0];
      })
    );
  }

  getBatCurrents(): Observable<number[]> {
    return this.command(5).pipe(
      map(x => {
        return [(x[0] * 256 + x[1]) / 100.0, (x[2] * 256 + x[3]) / 100.0];
      })
    );
  }

  getLogicCurrents(): Observable<number[]> {
	  return this.command(8).pipe(
		  map(x => {
			  return [(x[0] * 256 + x[1]) / 1000.0, (x[2] * 256 + x[3]) / 1000.0, (x[4] * 256 + x[5]) / 1000.0];
		  })
	  )
  }

  getTemperature(): Observable<number> {
    return this.command(6).pipe(map(x => (x[0] * 256 + x[1]) / 10.0));
  }

  getDepth(): Observable<number> {
	  return this.command(11).pipe(
		  map(x=> (x[0]*65536+x[1]*256+x[2])/100000.0)
	  );
  }

  private command(...args: number[]): Observable<number[]> {
    return this.http.post<number[]>(COPRO_URL, [...args]).pipe(
      catchError(err => {
        if (this.connected && err.status === 503) {
          this.connected = false;
          this.coproDisconnected.emit();
        }
        return throwError(err);
      }),
      tap(() => (this.connected = true))
    );
  }
}
