import { Injectable, EventEmitter } from '@angular/core';
import { Observable, Subject, throwError } from 'rxjs';
import { catchError } from 'rxjs/operators'
import { delay, share } from 'rxjs/operators';
import { HttpClient, HttpParams } from '@angular/common/http';
import { Command } from 'selenium-webdriver';
import { connectableObservableDescriptor } from 'rxjs/internal/observable/ConnectableObservable';

const COPRO_URL = "http://localhost:8080"

@Injectable()
export class CoproService {

    coproDisconnected = new EventEmitter();

    constructor(private http: HttpClient) {

    }

    setMoboPower(state: boolean): Observable<any>{
        return this.command(0, state?1:0);
    }

    setJetsonPower(state: boolean): Observable<any>{
        return this.command(1, state?1:0);
    }

    setThrusterPower(state: boolean): Observable<any>{
        return this.command(2, state?1:0);
    }

    private command(...args: number[]): Observable<number[]> {
        return this.http.post<number[]>(COPRO_URL, [...args]).pipe(
            catchError(err => {
                if(err.status == 503)
                    this.coproDisconnected.emit();
                return throwError(err);
            })
        )
    }
}
