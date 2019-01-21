import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { ScatterService } from './scatter.service';

@Injectable()
export class CntService {

    public publisher: {slug?:string};
    public contract:string;
    cardsntokens:string = "cardsntokens";

    private setReady: Function;
    public waitReady: Promise<any> = new Promise((resolve) => {
        this.setReady = resolve;
    });
    constructor(private scatter: ScatterService) {
        this.contract = this.cardsntokens;
        this.publisher = {slug:"guest"}
    }
}
