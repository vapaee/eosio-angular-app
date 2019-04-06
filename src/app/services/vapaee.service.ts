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
    public deposits: Asset[];
    public balances: Asset[];
    public onLoggedAccountChange:Subject<String> = new Subject();
    public onCurrentAccountChange:Subject<String> = new Subject();
    public onHistoryChange:Subject<String> = new Subject();
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
        this.getBalances();
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
    createOrder(type:string, amount:Asset, price:Asset) {
        // "alice", "buy", "2.50000000 CNT", "0.40000000 TLOS"
        // name owner, name type, const asset & total, const asset & price
        return this.utils.excecute("order", {
            owner:  this.scatter.account.name,
            type: type,
            total: amount.toString(8),
            price: price.toString(8)
        }).then(async result => {
            var _ = await this.updateTrade(amount.token, price.token);
            return result;
        });
    }

    // --------------------------------------------------------------
    // Getters 
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
            var deposits: Asset[] = [];
            if (this.logged) {
                var result = await this.fetchDeposits(this.logged);
                for (var i in result.rows) {
                    deposits.push(new Asset(result.rows[i].amount, this));
                }
            }
            this.deposits = deposits;
            return this.deposits;
        });
    }

    async getBalances(): Promise<any> {
        console.log("VapaeeService.getBalances()");
        return this.waitReady.then(async _ => {
            var balances: Asset[];
            if (this.logged) {
                balances = await this.fetchBalances(this.logged);
            }
            this.balances = balances;
            return this.balances;
        });
    }

    async updateTrade(comodity:Token, currency:Token): Promise<any> {
        return Promise.all([
            this.getTransactionHistory(comodity, currency, true),
            this.getSellOrders(comodity, currency, true),
            this.getBuyOrders(comodity, currency, true),
            this.getOrderTables(comodity, currency, true),
            this.getDeposits()
        ])
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
                    date: new Date(history.rows[i].date),
                    isbuy: !!history.rows[i].isbuy
                }

                this.scopes[scope].history.push(transaction);
            }

            this.scopes[scope].history.sort(function(a:HistoryTx, b:HistoryTx){
                if(a.date < b.date) return 1;
                if(a.date > b.date) return -1;
                if(a.id < b.id) return 1;
                if(a.id > b.id) return -1;
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

        this.onHistoryChange.next(result);

        return result;
    }

    async getSellOrders(comodity:Token, currency:Token, force:boolean = false): Promise<any> {
        var scope:string = comodity.symbol.toLowerCase() + "." + currency.symbol.toLowerCase();
        var aux = null;
        var result = null;
        aux = this.waitReady.then(async _ => {
            var orders = await this.fetchOrders(scope);
            var sell: Order[] = [];
            console.log("-------------");
            console.log("Sell crudo:", orders);
            this.scopes[scope] = this.auxCreateScope(scope);
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

                sell.push(order);
            }


            sell.sort(function(a:Order, b:Order){
                if(a.price.amount > b.price.amount) return 1;
                if(a.price.amount < b.price.amount) return -1;
                return 0;
            });
            
            // grouping together orders with the same price.
            var list: OrderRow[] = [];
            var row: OrderRow;
            if (sell.length > 0) {
                for(var i=0; i<sell.length; i++) {
                    var order: Order = sell[i];
                    if (list.length > 0) {
                        row = list[i-1];
                        if (row.price.amount.eq(order.price.amount)) {
                            row.total.amount = row.total.amount.plus(order.total.amount);
                            row.telos.amount = row.telos.amount.plus(order.telos.amount);
                            row.owners[order.owner] = true;
                            row.orders.push(order);
                            continue;
                        }    
                    }
                    row = {
                        orders: [],
                        price: order.price,
                        total: order.total,
                        telos: order.telos,
                        inverse: order.inverse,
                        sum: null,
                        sumtelos: null,
                        owners: {}
                    }

                    row.owners[order.owner] = true;
                    row.orders.push(order);
                    list.push(row);
                }
            }

            var sum = new BigNumber(0);
            var sumtelos = new BigNumber(0);
            for (var j in list) {
                var order_row = list[j];
                sumtelos = sumtelos.plus(order_row.telos.amount);
                sum = sum.plus(order_row.total.amount);
                order_row.sumtelos = new Asset(sumtelos, order_row.telos.token);
                order_row.sum = new Asset(sum, order_row.total.token);
            }

            this.scopes[scope].orders.sell = list;
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
            var buy: Order[] = [];
            console.log("-------------");
            console.log("Buy crudo:", orders);
            this.scopes[scope] = this.auxCreateScope(scope);
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

                buy.push(order);
            }

            buy.sort(function(a:Order, b:Order){
                if(a.price.amount < b.price.amount) return 1;
                if(a.price.amount > b.price.amount) return -1;
                return 0;
            });

            // grouping together orders with the same price.
            var list: OrderRow[] = [];
            var row: OrderRow;
            if (buy.length > 0) {
                for(var i=0; i<buy.length; i++) {
                    var order: Order = buy[i];
                    if (list.length > 0) {
                        row = list[i-1];
                        if (row.price.amount.eq(order.price.amount)) {
                            row.total.amount = row.total.amount.plus(order.total.amount);
                            row.telos.amount = row.telos.amount.plus(order.telos.amount);
                            row.owners[order.owner] = true;
                            row.orders.push(order);
                            continue;
                        }    
                    }
                    row = {
                        orders: [],
                        price: order.price,
                        total: order.total,
                        telos: order.telos,
                        inverse: order.inverse,
                        sum: null,
                        sumtelos: null,
                        owners: {}
                    }

                    row.owners[order.owner] = true;
                    row.orders.push(order);
                    list.push(row);
                }
            }            

            var sum = new BigNumber(0);
            var sumtelos = new BigNumber(0);
            for (var j in list) {
                var order_row = list[j];
                sumtelos = sumtelos.plus(order_row.telos.amount);
                sum = sum.plus(order_row.total.amount);
                order_row.sumtelos = new Asset(sumtelos, order_row.telos.token);
                order_row.sum = new Asset(sum, order_row.total.token);
            }

            this.scopes[scope].orders.buy = list;
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
    
    async getOrderTables(comodity:Token, currency:Token, force:boolean = false): Promise<any> {
        var scope:string = comodity.symbol.toLowerCase() + "." + currency.symbol.toLowerCase();
        this.scopes[scope] = this.auxCreateScope(scope);
        var tables = await this.fetchOrderTables(comodity, currency);

        for (var i in tables.rows) {
            if (tables.rows[i].pay == comodity.symbol && tables.rows[i].sell == currency.symbol) {
                this.scopes[scope].tables.buy.total = new Asset(tables.rows[i].total, this);
                this.scopes[scope].tables.buy.orders = tables.rows[i].orders;
            }
            if (tables.rows[i].pay == currency.symbol && tables.rows[i].sell == comodity.symbol) {
                this.scopes[scope].tables.sell.total = new Asset(tables.rows[i].total, this);
                this.scopes[scope].tables.sell.orders = tables.rows[i].orders;
            }
        }

    }
    //
    // --------------------------------------------------------------
    // Aux functions

    private auxCreateScope(scope:string) {
        var comodity_sym = scope.split(".")[0].toUpperCase();
        var currency_sym = scope.split(".")[1].toUpperCase();
        return this.scopes[scope] || {
            scope: scope,
            orders: { sell: [], buy: [] },
            history: [],
            tables: { 
                sell: {total:new Asset("0.0 " + comodity_sym, this), orders:0}, 
                buy: {total:new Asset("0.0 " + currency_sym, this), orders:0}
            },
        };        
    }

    private fetchDeposits(account): Promise<any> {
        return this.utils.getTable("deposits", {scope:account}).then(result => {
            return result;
        });
    }

    private async fetchBalances(account): Promise<any> {
        return this.waitReady.then(async _ => {
            var contracts = {};
            var balances = [];
            for (var i in this.tokens) {
                contracts[this.tokens[i].contract] = true;
            }
            for (var contract in contracts) {
                var result = await this.utils.getTable("accounts", {
                    contract:contract,
                    scope:this.scatter.account.name
                });
                for (var i in result.rows) {
                    balances.push(new Asset(result.rows[i].balance, this));
                }
            }
            return balances;
        });
    }

    private fetchOrders(scope): Promise<any> {
        return this.utils.getTable("sellorders", {scope:scope}).then(result => {
            return result;
        });
    }

    private fetchOrderTables(token1:Token, token2:Token): Promise<any> {
        return this.utils.getTable("ordertables").then(result => {
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
    clone(): Asset {
        return new Asset(this.amount, this.token);
    }

    parse(text: string, vapaee: VapaeeService) {
        var sym = text.split(" ")[1];
        this.token = vapaee.getTokenNow(sym);
        var amount_str = text.split(" ")[0];
        this.amount = new BigNumber(amount_str);
        console.assert(!!this.token, "ERROR: string malformed of token not found:", text)
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
        if (decimal.length > precision) {
            decimal = decimal.substr(0, precision);
        }
        if (precision == 0) {
            return integer;
        } else {
            return integer + "." + decimal;
        }
    }

    get str () {
        return this.toString();
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
        history: HistoryTx[],
        tables: Summary
    };
}

export interface Summary {
    sell:TableSummary,
    buy:TableSummary
}

export interface TableSummary {
    total: Asset;
    orders: number;
}

export interface TokenOrders {
    sell:OrderRow[],
    buy:OrderRow[]
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
    isbuy: boolean;
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

export interface OrderRow {
    orders: Order[];
    price: Asset;
    inverse: Asset;
    total: Asset;
    sum: Asset;
    sumtelos: Asset;
    telos: Asset;
    owners: {[key:string]:boolean}
}