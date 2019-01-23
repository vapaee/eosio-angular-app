import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { ScatterService } from './scatter.service';
import { BGBoxService } from './bgbox.service';

export interface Publisher {
    slugid: string;
    account: string;
}

@Injectable()
export class CntService {

    public loginState: string;
    /*
    public loginState: string;
    - 'no-scatter': Scatter no detected
    - 'no-logged': Scatter detected but user is not logged
    - 'no-publishers': User logged but has no publishers in bg-box
    - 'no-selected': usr has several publishers but none selected 
    - 'no-registered': user has a selected publisher but is not registered in C&T
    - 'publisher-ok': user has selected a registered publisher (has C&T inventories in BG-Box memory)
    */

    public publisher: Publisher;
    public contract:string;
    cardsntokens:string = "cardsntokens";
    
    private setReady: Function;
    public waitReady: Promise<any> = new Promise((resolve) => {
        this.setReady = resolve;
    });
    constructor(
        private scatter: ScatterService,
        private bgbox: BGBoxService
        ) {
        this.contract = this.cardsntokens;
        this.publisher = {slugid:"guest", account:this.contract};
        this.updateScatterState();
    }

    updateScatterState() {
        this.loginState = "no-scatter";
        this.scatter.waitConnected.then(() => {
            this.loginState = "no-logged";
            console.log("this.scatter", this.scatter);
            if (this.scatter.logged) {
                this.loginState = "no-publishers";
                this.scatter.waitLogged.then(this.updateScatterState.bind(this));
            }
        })
    }

}
