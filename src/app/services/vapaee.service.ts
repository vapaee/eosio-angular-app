import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { ScatterService } from './scatter.service';
import { Utils, Token } from './utils.service';
import BigNumber from "bignumber.js";

@Injectable()
export class VapaeeService {

    public loginState: string;
    /*
    public loginState: string;
    - 'no-scatter': Scatter no detected
    - 'no-logged': Scatter detected but user is not logged
    - 'account-ok': user logger with scatter
    */
    public telos: Token;
    public tokens: Token[];
    public scopes: TableMap;
    public utils: Utils;
    public current: String;
    public default: String;
    public contract:string;   
    public deposits: any;
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
        this.scopes = {};
        this.default = 'guest';
        this.current = this.default;
        this.contract = this.vapaeetokens;
        this.scatter.onLogggedStateChange.subscribe(this.onLoggedChange.bind(this));
        this.updateLogState();
        this.utils = new Utils(this.contract, this.scatter);
        this.fetchTokens().then(data => {
            this.tokens = data.tokens;
            this.tokens.sort(function(a:Token, b:Token){
                if(a.appname < b.appname) return -1;
                if(a.appname > b.appname) return 1;
                return 0;
            });
            this.setReady();
        });        
    }

    // getters ------------
    get logged() {
        return this.scatter.logged ? this.scatter.account.name : null;
    }

    get account() {
        return this.scatter.logged ? this.scatter.account : {};
    }

    // --
    login() {
        this.logout();
        this.scatter.login().then(() => {
            this.updateLogState();
        });
    }

    logout() {
        this.scatter.logout();
    }

    onLogout() {
        console.log("VapaeeService.onLogout()");
        this.resetCurrentAccount(this.default);
        this.updateLogState();
        this.onLoggedAccountChange.next(this.logged);
    }
    
    onLogin(name:string) {
        console.log("VapaeeService.onLogin()", name);
        this.resetCurrentAccount(name);
        this.getDeposits();
        this.updateLogState();
        this.onLoggedAccountChange.next(this.logged);
    }

    onLoggedChange() {
        console.log("VapaeeService.onLoggedChange()");
        if (this.scatter.logged) {
            this.onLogin(this.scatter.account.name);
        } else {
            this.onLogout();
        }
    }

    resetCurrentAccount(profile:String) {
        console.log("VapaeeService.resetCurrentAccount() current:", this.current, "next:", profile);
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
            if (this.scatter.logged) {
                this.loginState = "account-ok";
            }
        });
    }

    // --------------------------------------------------------------
    getTokenNow(sym:string): Token {
        for (var i in this.tokens) {
            if (this.tokens[i].symbol.toUpperCase() == sym.toUpperCase()) {
                return this.tokens[i];
            }
        }
        return null;
    }

    async getToken(sym:string): Promise<Token> {
        return this.waitReady.then(_ => {
            return this.getTokenNow(sym);
        });
    }

    async getDeposits(): Promise<any> {
        console.log("VapaeeService.getDeposits()");
        return this.waitReady.then(async _ => {
            this.deposits = [];
            if (this.logged) {
                var result = await this.fetchDeposits(this.logged);
                for (var i in result.rows) {
                    this.deposits.push(new Asset(result.rows[i].amount, this));
                }
            }
            return this.deposits;
        });
    }

    async getTransactionHistory(comodity:Token, currency:Token, force:boolean = false): Promise<any> {
        var scope:string = comodity.symbol.toLowerCase() + "." + currency.symbol.toLowerCase();
        if (comodity == this.telos) {
            scope = currency.symbol.toLowerCase() + "." + comodity.symbol.toLowerCase();
        }
        var aux = null;
        var result = null;
        aux = this.waitReady.then(async _ => {
            var history = await this.fetchHistory(scope);
            console.log("-------------");
            console.log("History crudo:", history);
            this.scopes[scope] = this.auxCreateScope(scope);
            this.scopes[scope].history = [];

            for (var i=0; i < history.rows.length; i++) {
                var transaction:HistoryTx = {
                    id: history.rows[i].id,
                    amount: new Asset(history.rows[i].amount, this),
                    payment: new Asset(history.rows[i].payment, this),
                    buyfee: new Asset(history.rows[i].buyfee, this),
                    sellfee: new Asset(history.rows[i].sellfee, this),
                    price: new Asset(history.rows[i].price, this),
                    buyer: history.rows[i].buyer,
                    seller: history.rows[i].seller,
                    date: new Date(history.rows[i].date)
                }

                this.scopes[scope].history.push(transaction);
            }

            this.scopes[scope].history.sort(function(a:HistoryTx, b:HistoryTx){
                if(a.date < b.date) return 1;
                if(a.date > b.date) return -1;
                return 0;
            });            

            console.log("History final:", this.scopes[scope].history);
            console.log("-------------");
            return history;
        });

        if (this.scopes[scope] && !force) {
            result = this.scopes[scope].history;
        } else {
            result = aux;
        }
        return result;
    }

    async getSellOrders(comodity:Token, currency:Token, force:boolean = false): Promise<any> {
        var scope:string = comodity.symbol.toLowerCase() + "." + currency.symbol.toLowerCase();
        var aux = null;
        var result = null;
        aux = this.waitReady.then(async _ => {
            var orders = await this.fetchOrders(scope);
            console.log("-------------");
            console.log("Sell crudo:", orders);
            this.scopes[scope] = this.auxCreateScope(scope);
            this.scopes[scope].orders.sell = [];
            for (var i=0; i < orders.rows.length; i++) {
                var order:Order = {
                    id: orders.rows[i].id,
                    price: new Asset(orders.rows[i].price, this),
                    inverse: new Asset(orders.rows[i].inverse, this),
                    total: new Asset(orders.rows[i].selling, this),
                    deposit: new Asset(orders.rows[i].total, this),
                    telos: new Asset(orders.rows[i].selling, this),
                    fee: new Asset(orders.rows[i].fee, this),
                    owner: orders.rows[i].owner
                }
                if (order.total.token == this.telos) {
                    order.telos = order.total;
                } else if (order.deposit.token == this.telos) {
                    order.telos = order.deposit;
                }

                this.scopes[scope].orders.sell.push(order);
            }


            this.scopes[scope].orders.sell.sort(function(a:Order, b:Order){
                if(a.price.amount > b.price.amount) return 1;
                if(a.price.amount < b.price.amount) return -1;
                return 0;
            });

            console.log("Sell final:", this.scopes[scope].orders.sell);
            console.log("-------------");
            return orders;
        });

        if (this.scopes[scope] && !force) {
            result = this.scopes[scope].orders.sell;
        } else {
            result = aux;
        }
        return result;
    }
    
    async getBuyOrders(comodity:Token, currency:Token, force:boolean = false): Promise<any> {
        var invere_scope:string = currency.symbol.toLowerCase() + "." + comodity.symbol.toLowerCase();
        var scope:string = comodity.symbol.toLowerCase() + "." + currency.symbol.toLowerCase();
        var aux = null;
        var result = null;
        aux = this.waitReady.then(async _ => {
            var orders = await this.fetchOrders(invere_scope);
            console.log("-------------");
            console.log("Buy crudo:", orders);
            this.scopes[scope] = this.auxCreateScope(scope);
            this.scopes[scope].orders.buy = [];
            for (var i=0; i < orders.rows.length; i++) {
                var order:Order = {
                    id: orders.rows[i].id,
                    price: new Asset(orders.rows[i].inverse, this),
                    inverse: new Asset(orders.rows[i].price, this),
                    total: new Asset(orders.rows[i].total, this),
                    deposit: new Asset(orders.rows[i].selling, this),
                    telos: new Asset(orders.rows[i].selling, this),
                    fee: new Asset(orders.rows[i].fee, this),
                    owner: orders.rows[i].owner
                }
                if (order.total.token == this.telos) {
                    order.telos = order.total;
                } else if (order.deposit.token == this.telos) {
                    order.deposit = order.total;
                }

                // order.price = order.price.inverse(order.deposit.token);

                this.scopes[scope].orders.buy.push(order);
            }

            this.scopes[scope].orders.buy.sort(function(a:Order, b:Order){
                if(a.price.amount < b.price.amount) return 1;
                if(a.price.amount > b.price.amount) return -1;
                return 0;
            });

            console.log("Buy final:", this.scopes[scope].orders.buy);
            console.log("-------------");
            return orders;
        });

        if (this.scopes[scope] && !force) {
            result = this.scopes[scope].orders.buy;
        } else {
            result = aux;
        }
        return result;
    }    
    //
    // --------------------------------------------------------------
    // Aux functions

    private auxCreateScope(scope:string) {
        return this.scopes[scope] || {
            scope: scope,
            orders: { sell: [], buy: [] },
            history: []
        };        
    }

    private fetchDeposits(account): Promise<any> {
        return this.utils.getTable("deposits", {scope:account}).then(result => {
            return result;
        });
    }

    private fetchOrders(scope): Promise<any> {
        return this.utils.getTable("sellorders", {scope:scope}).then(result => {
            return result;
        });
    }

    private fetchHistory(scope): Promise<any> {
        return this.utils.getTable("history", {scope:scope}).then(result => {
            return result;
        });
    }

    private fetchTokenStats(token): Promise<any> {
        return this.utils.getTable("stat", {contract:token.contract, scope:token.symbol}).then(result => {
            token.stat = result.rows[0];
            return token;
        });
    }

    private fetchTokens(extended: boolean = true) {
        console.log("Vapaee.fetchTokens()");

        return this.utils.getTable("tokens").then(result => {
            var data = {
                tokens: <Token[]>result.rows
            }

            for (var i in data.tokens) {
                data.tokens[i].scope = data.tokens[i].symbol.toLowerCase() + ".tlos";
                if (data.tokens[i].symbol == "TLOS") {
                    this.telos = data.tokens[i];
                }
            }            

            if (!extended) return data;

            var priomises = [];
            for (var i in data.tokens) {
                // console.log(data.tokens[i]);
                priomises.push(this.fetchTokenStats(data.tokens[i]));
            }

            return Promise.all<any>(priomises).then(result => {
                return data;
            });
        });

    }    

}




export class Asset {
    amount:BigNumber;
    token:Token;
    
    constructor(a: any, b: any) {
        if (a instanceof BigNumber) {
            this.amount = a;
            this.token = b;
            return;
        }

        if (b instanceof VapaeeService) {
            this.parse(a,b);
        }
    }

    parse(text: string, vapaee: VapaeeService) {
        var sym = text.split(" ")[1];
        this.token = vapaee.getTokenNow(sym);
        var amount_str = text.split(" ")[0];
        this.amount = new BigNumber(amount_str);
        console.assert(!!this.token, "ERROR: string malformed of token not foun:", text)
    }

    valueToString(decimals:number = -1): string {
        var parts = ("" + this.amount).split(".");
        var integer = parts[0];
        var precision = this.token.precision;
        var decimal = (parts.length==2 ? parts[1] : "");
        if (decimals != -1) {
            precision = decimals;
        }
        for (var i=decimal.length; i<precision; i++) {
            decimal += "0";
        }
        return integer + "." + decimal;
    }

    toString(decimals:number = -1): string {
        return this.valueToString(decimals) + " " + this.token.symbol.toUpperCase();
    }

    inverse(token: Token): Asset {
        var result = new BigNumber(1).dividedBy(this.amount);
        var asset =  new Asset(result, token);
        return asset;
    }
}

export interface TableMap {
    [key: string]: {
        scope: string,
        orders: TokenOrders,
        history: HistoryTx[]
    };
}

export interface TokenOrders {
    sell:Order[],
    buy:Order[]
}

export interface HistoryTx {
    id: number;
    price: Asset;
    amount: Asset;
    payment: Asset;
    buyfee: Asset;
    sellfee: Asset;
    buyer: string;
    seller: string;
    date: Date;
}

export interface Order {
    id: number;
    price: Asset;
    inverse: Asset;
    total: Asset;
    deposit: Asset;
    telos: Asset;
    fee: Asset;
    owner: string;
}