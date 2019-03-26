import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { ScatterService } from './scatter.service';
import { Utils, Token } from './utils.service';

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
    getTokenStats(token): Promise<any> {
        return this.utils.getTable("stat", {contract:token.contract, scope:token.symbol}).then(result => {
            token.stat = result.rows[0];
            return token;
        });
    }

    getTokens(extended: boolean = true) {
        console.log("Vapaee.getTokens()");

        return this.utils.getTable("tokens").then(result => {
            var data = {
                tokens: <Token[]>result.rows
            }

            if (!extended) return data;

            var priomises = [];
            for (var i in data.tokens) {
                // console.log(data.tokens[i]);
                priomises.push(this.getTokenStats(data.tokens[i]));
            }

            return Promise.all<any>(priomises).then(result => {
                return data;
            });
        });

    }    

}
