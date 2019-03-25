import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { ScatterService } from './scatter.service';
import { Utils } from './utils.service';

@Injectable()
export class VapaeeService {

    public loginState: string;
    /*
    public loginState: string;
    - 'no-scatter': Scatter no detected
    - 'no-logged': Scatter detected but user is not logged
    - 'account-ok': user logger with scatter
    */

    public utils: Utils;
    public logged: String;
    public current: String;
    public contract:string;   
    public onLoggedAccountChange:Subject<String> = new Subject();
    public onCurrentAccountChange:Subject<String> = new Subject();
    vapaeetokens:string = "vapaeetokens";
    
    private setReady: Function;
    public waitReady: Promise<any> = new Promise((resolve) => {
        this.setReady = resolve;
    });
    constructor(
        private scatter: ScatterService
        ) {
        this.logged = null;
        this.current = null;
        this.contract = this.vapaeetokens;
        this.scatter.onLogggedStateChange.subscribe(this.updateLogState.bind(this));
        this.updateLogState();
        this.utils = new Utils(this.contract, this.scatter);
    }

    // --
    login() {
        this.logout();
        this.scatter.login().then(() => {
            this.updateLogState();
        });
    }

    logout() {
        this.resetCurrentAccount(null);
        this.logged = null;
        this.onLoggedAccountChange.next(this.logged);
    }

    resetCurrentAccount(profile:String) {
        if (this.current != profile) {
            this.current = profile;
            this.onCurrentAccountChange.next(this.current);
        }
    }


    private updateLogState() {
        console.log("VapaeeService.updateLogState()", [this]);
        this.loginState = "no-scatter";
        this.scatter.waitConnected.then(() => {
            this.loginState = "no-logged";
            console.log("this.scatter", [this.scatter]);
            if (this.scatter.logged) {
                var trigger = this.scatter.account.name != this.logged;
                this.logged = this.scatter.account.name;
                this.loginState = "account-ok";
                if (trigger) this.onLoggedAccountChange.next(this.logged);
            }
        })
    }
    // --------------------------------------------------------------
    // API
    getTokens() {
        console.log("Vapaee.getTokens()");

        return Promise.all<any>([
            this.utils.getTable("tokens"),
            this.utils.getTable("ordertables"),
            this.utils.getTable("earnings"),
        ]).then(result => {
            
            var data = {
                tokens:result[0].rows,
                ordertables:result[1].rows,
                earnings:result[2].rows,
            };
            /*
            for (var i in data.tokens) {
                data.tokens[i].slugid = this.decodeSlug(data.tokens[i].slugid);
                var key = "id-" + data.tokens[i].id
                data.map[key] = Object.assign({}, data.tokens[i]);
                data.tokens[i] = data.map[key];
            }

            for (var i in data.ordertables) {
                Object.assign(data.map["id-" + data.ordertables[i].id], data.ordertables[i]);
                data.ordertables[i] = data.map["id-" + data.ordertables[i].id];
            }

            for (var i in data.earnings) {
                Object.assign(data.map["id-" + data.earnings[i].id], data.earnings[i]);
                data.earnings[i] = data.map["id-" + data.earnings[i].id];
            }

            for (var i in data.tokens) {
                var key = "id-" + data.tokens[i].id
                this.tokens[data.map[key].slugid.str] = data.map[key];
            }
            */

            console.log("getTokens() ----------> ", result, data);
            return data;
        });

    }    

}
