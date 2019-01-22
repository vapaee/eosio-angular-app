import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { ScatterService } from './scatter.service';

export interface Publisher {
    namespace: string;
    account: string;
}

@Injectable()
export class CntService {
    
    public publisher: Publisher;
    public contract:string;
    cardsntokens:string = "cardsntokens";
    
    private setReady: Function;
    public waitReady: Promise<any> = new Promise((resolve) => {
        this.setReady = resolve;
    });
    constructor(private scatter: ScatterService) {
        this.contract = this.cardsntokens;
        this.publisher = {namespace:"guest", account:this.contract};
    }

}
