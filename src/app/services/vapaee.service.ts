import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { ScatterService, Account, AccountData } from './scatter.service';
import { Utils, Token, TableResult, TableParams } from './utils.service';
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

    public zero_telos: Asset;
    public telos: Token;
    public tokens: Token[];
    public scopes: TableMap;
    public utils: Utils;
    public current: Account;
    public last_logged: string;
    public contract: string;   
    public deposits: Asset[];
    public balances: Asset[];
    public userorders: UserOrdersMap;
    public onLoggedAccountChange:Subject<string> = new Subject();
    public onCurrentAccountChange:Subject<string> = new Subject();
    public onHistoryChange:Subject<string> = new Subject();
    public onSummaryChange:Subject<string> = new Subject();
    vapaeetokens:string = "vapaeetokens";    
    
    private setOrdertables: Function;
    public waitOrdertables: Promise<any> = new Promise((resolve) => {
        this.setOrdertables = resolve;
    });

    private setReady: Function;
    public waitReady: Promise<any> = new Promise((resolve) => {
        this.setReady = resolve;
    });
    constructor(
        private scatter: ScatterService
    ) {
        this.scopes = {};
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
            this.zero_telos = new Asset("0.0000 TLOS", this);
            this.setReady();
            this.getAllTablesSumaries();
        });        
    }

    // getters ------------
    get default(): Account {
        return this.scatter.default;
    }

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
        this.resetCurrentAccount(this.default.name);
        this.updateLogState();
        this.onLoggedAccountChange.next(this.logged);
        setTimeout(_  => { this.last_logged = this.logged; }, 400);
    }
    
    onLogin(name:string) {
        console.log("VapaeeService.onLogin()", name);
        this.resetCurrentAccount(name);
        this.getDeposits();
        this.getBalances();
        this.updateLogState();
        this.getUserOrders();
        this.onLoggedAccountChange.next(this.logged);
        this.last_logged = this.logged;
    }

    onLoggedChange() {
        console.log("VapaeeService.onLoggedChange()");
        if (this.scatter.logged) {
            this.onLogin(this.scatter.account.name);
        } else {
            this.onLogout();
        }
    }

    async resetCurrentAccount(profile:string) {
        console.log("VapaeeService.resetCurrentAccount() current:", this.current, "next:", profile);
        if (this.current.name != profile && (this.current.name == this.last_logged || profile != "guest")) {
            this.current = this.default;
            this.current.name = profile;
            if (profile != "guest") {
                this.current.data = await this.getAccountData(this.current.name);
            }
            // this.scopes = {};
            this.balances = [];
            this.userorders = {};            
            console.log("this.onCurrentAccountChange.next(this.current.name) !!!!!!");
            this.onCurrentAccountChange.next(this.current.name);
            this.updateCurrentUser();
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

    private async getAccountData(name: string): Promise<AccountData>  {
        return this.scatter.queryAccountData(name).catch(async _ => {
            return this.default.data;
        });
    }

    // Actions --------------------------------------------------------------
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

    cancelOrder(type:string, comodity:Token, currency:Token, orders:number[]) {
        // '["alice", "buy", "CNT", "TLOS", [1,0]]'
        // name owner, name type, const asset & total, const asset & price
        return this.utils.excecute("cancel", {
            owner:  this.scatter.account.name,
            type: type,
            comodity: comodity.symbol,
            currency: currency.symbol,
            orders: orders
        }).then(async result => {
            var _ = await this.updateTrade(comodity, currency);
            return result;
        });
    }

    deposit(quantity:Asset) {
        // name owner, name type, const asset & total, const asset & price
        var util = new Utils(quantity.token.contract, this.scatter);
        return util.excecute("transfer", {
            from:  this.scatter.account.name,
            to: this.vapaeetokens,
            quantity: quantity.toString(),
            memo: "deposit"
        }).then(async result => {
            await this.getDeposits();
            await this.getBalances();
            return result;
        });
    }    

    withdraw(quantity:Asset) {
        return this.utils.excecute("withdraw", {
            owner:  this.scatter.account.name,
            quantity: quantity.toString()
        }).then(async result => {
            await this.getDeposits();
            await this.getBalances();
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

    async getDeposits(account:string = null): Promise<any> {
        console.log("VapaeeService.getDeposits()");
        return this.waitReady.then(async _ => {
            var deposits: Asset[] = [];
            if (!account && this.current.name) {
                account = this.current.name;
            }
            if (account) {
                var result = await this.fetchDeposits(account);
                for (var i in result.rows) {
                    deposits.push(new Asset(result.rows[i].amount, this));
                }
            }
            this.deposits = deposits;
            return this.deposits;
        });
    }

    async getBalances(account:string = null): Promise<any> {
        console.log("VapaeeService.getBalances()");
        return this.waitReady.then(async _ => {
            var balances: Asset[];
            if (!account && this.current.name) {
                account = this.current.name;
            }            
            if (account) {
                balances = await this.fetchBalances(account);
            }
            this.balances = balances;
            return this.balances;
        });
    }

    async getThisSellOrders(table:string, ids:number[]): Promise<any[]> {
        return this.waitReady.then(async _ => {
            var result = [];
            for (var i in ids) {
                var id = ids[i];
                var gotit = false;
                for (var j in result) {
                    if (result[j].id == id) {
                        gotit = true;
                        break;
                    }
                }
                if (gotit) {
                    continue;
                }
                var res:TableResult = await this.fetchOrders({scope:table, limit:10, lower_bound:id.toString()});
                result = result.concat(res.rows);
            }
            return result;
        });    
    }

    async getUserOrders(account:string = null) {
        console.log("VapaeeService.getUserOrders()");
        return this.waitReady.then(async _ => {
            var userorders: TableResult;
            if (!account && this.current.name) {
                account = this.current.name;
            }            
            if (account) {
                userorders = await this.fetchUserOrders(account);
            }
            var list: UserOrders[] = <UserOrders[]>userorders.rows;
            var map: UserOrdersMap = {};
            for (var i=0; i<list.length; i++) {
                var ids = list[i].ids;
                var table = list[i].table;
                var orders = await this.getThisSellOrders(table, ids);
                map[table] = {
                    table: table,
                    orders: this.auxProcessRowsToOrders(orders),
                    ids:ids
                };
            }
            this.userorders = map;
            console.log(this.userorders);
            return this.userorders;
        });
                
    }

    async updateTrade(comodity:Token, currency:Token): Promise<any> {
        return Promise.all([
            this.getTransactionHistory(comodity, currency, -1, -1, true),
            this.getSellOrders(comodity, currency, true),
            this.getBuyOrders(comodity, currency, true),
            this.getTableSummary(comodity, currency, true),
            this.getOrderTables(),
            this.updateCurrentUser()
        ]);
    }

    async updateCurrentUser(): Promise<any> {
        console.log("VapaeeService.updateCurrentUser()");
        return Promise.all([
            this.getUserOrders(),
            this.getDeposits(),
            this.getBalances()
        ]);       
    }

    async getTransactionHistory(comodity:Token, currency:Token, page:number = -1, pagesize:number = -1, force:boolean = false): Promise<any> {
        var scope:string = comodity.symbol.toLowerCase() + "." + currency.symbol.toLowerCase();
        if (comodity == this.telos) {
            scope = currency.symbol.toLowerCase() + "." + comodity.symbol.toLowerCase();
        }
        var aux = null;
        var result = null;
        aux = this.waitOrdertables.then(async _ => {
            if (pagesize == -1) {
                pagesize = 20;                
            }
            if (page == -1) {
                var mod = this.scopes[scope].deals % pagesize;
                var dif = this.scopes[scope].deals - mod;
                var pages = dif / pagesize;
                page = pages-2;
                console.log("página -------------");
                console.log("deals", this.scopes[scope].deals);
                console.log("mod", mod);
                console.log("dif", dif);
                console.log("pages", pages);
                console.log("page", page);
                console.log("-------------");
            }

            var history1:TableResult = await this.fetchHistory(scope, page, pagesize);
            var history2:TableResult = await this.fetchHistory(scope, page+1, pagesize);
            var history3:TableResult = await this.fetchHistory(scope, page+2, pagesize);
            var history = {rows: history1.rows.concat(history2.rows).concat(history3.rows)};

            console.log("-------------");
            console.log("History crudo:", history);
            console.log(this.scopes[scope].deals);
            this.scopes[scope] = this.auxAssertScope(scope);
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

            // console.log("History final:", this.scopes[scope].history);
            // console.log("-------------");
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
            var orders = await this.fetchOrders({scope:scope, limit:50, index_position: "2", key_type: "i64"});
            this.scopes[scope] = this.auxAssertScope(scope);
            // console.log("-------------");
            // console.log("Sell crudo:", orders);
            var sell: Order[] = this.auxProcessRowsToOrders(orders.rows);
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
                        row = list[list.length-1];
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
                        total: order.total.clone(),
                        telos: order.telos.clone(),
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
            // console.log("Sell final:", this.scopes[scope].orders.sell);
            // console.log("-------------");
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
            var orders = await await this.fetchOrders({scope:invere_scope, limit:50, index_position: "2", key_type: "i64"});
            this.scopes[scope] = this.auxAssertScope(scope);
            // console.log("-------------");
            // console.log("Buy crudo:", orders);            
            var buy: Order[] = this.auxProcessRowsToOrders(orders.rows);
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
                        row = list[list.length-1];
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
                        total: order.total.clone(),
                        telos: order.telos.clone(),
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
            // console.log("Buy final:", this.scopes[scope].orders.buy);
            // console.log("-------------");
            return orders;
        });

        if (this.scopes[scope] && !force) {
            result = this.scopes[scope].orders.buy;
        } else {
            result = aux;
        }
        return result;
    }
    
    async getOrderTables(): Promise<any> {
        console.log("VapaeeService.getOrderTables()");
        var tables = await this.fetchOrderTables();

        for (var i in tables.rows) {
            var scope:string = tables.rows[i].table;
            var comodity = scope.split(".")[0].toUpperCase();
            var currency = scope.split(".")[1].toUpperCase();
            this.scopes[scope] = this.auxAssertScope(scope);
            if (tables.rows[i].pay == comodity && tables.rows[i].sell == currency) {
                this.scopes[scope].header.buy.total = new Asset(tables.rows[i].total, this);
                this.scopes[scope].header.buy.orders = tables.rows[i].orders;
            }
            if (tables.rows[i].pay == currency && tables.rows[i].sell == comodity) {
                this.scopes[scope].header.sell.total = new Asset(tables.rows[i].total, this);
                this.scopes[scope].header.sell.orders = tables.rows[i].orders;
                this.scopes[scope].deals = tables.rows[i].deals;
            }
        }

        if (tables.rows.length > 0) {
            this.setOrdertables();
            this.waitOrdertables = new Promise((resolve) => {
                this.setOrdertables = resolve;
            });
        }
    }

    async getTableSummary(comodity:Token, currency:Token, force:boolean = false): Promise<any> {
        var scope:string = comodity.symbol.toLowerCase() + "." + currency.symbol.toLowerCase();
        if (comodity == this.telos) {
            scope = currency.symbol.toLowerCase() + "." + comodity.symbol.toLowerCase();
        }
        var aux = null;
        var result = null;
        aux = this.waitReady.then(async _ => {
            var summary = await this.fetchSummary(scope);
            // console.log("---------------------------------------------------");
            // console.log("Summary crudo:", summary.rows);

            this.scopes[scope] = this.auxAssertScope(scope);
            this.scopes[scope].summary = {
                price: new Asset("0.0000 TLOS", this),
                volume: new Asset("0.0000 TLOS", this),
                percent: 0.3,
                records: summary.rows
            };

            var now:Date = new Date();
            var now_sec: number = Math.floor(now.getTime() / 1000);
            var now_hour: number = Math.floor(now_sec / 3600);
            var start_hour = now_hour - 23;
            // console.log("now_hour:", now_hour);
            // console.log("start_hour:", start_hour);

            // proceso los datos crudos 
            var crude = {};
            for (var i=0; i<summary.rows.length; i++) {
                var hh = summary.rows[i].hour;
                crude[hh] = summary.rows[i];
            }
            // console.log("crude:", crude);

            // genero una entrada por cada una de las últimas 24 horas
            // var first_price = new Asset(, this);
            var last_24h = {};
            var ZERO_TLOS = "0.00000000 TLOS";
            var price = ZERO_TLOS;
            var volume = new Asset(ZERO_TLOS, this);
            var first:Asset = null;
            for (var i=0; i<24; i++) {
                var current = start_hour+i;
                var current_date = new Date(current * 3600 * 1000);
                // console.log("current_date:", current_date.toISOString());
                last_24h[current] = crude[current] || {
                    label: this.auxGetLabelForHour(current % 24),
                    price: price,
                    volume: ZERO_TLOS,
                    date: current_date.toISOString().split(".")[0],
                    hour: current
                };
                price = last_24h[current].price;
                var vol = new Asset(last_24h[current].volume, this);
                volume.amount = volume.amount.plus(vol.amount);
                if (price != ZERO_TLOS && !first) {
                    first = new Asset(price, this);
                }
            }
            if (!first) {
                first = new Asset(last_24h[start_hour].price, this);
            }
            var last =  new Asset(last_24h[now_hour].price, this);
            var diff = last.clone();
            // diff.amount 
            diff.amount = last.amount.minus(first.amount);
            var ratio:number = 0;
            if (first.amount.toNumber() != 0) {
                ratio = diff.amount.dividedBy(first.amount).toNumber();
            }            
            var percent = Math.floor(ratio * 10000) / 100;

            // console.log("last_24h:", last_24h);
            // console.log("first:", first.toString(8));
            // console.log("last:", last.toString(8));
            // console.log("diff:", diff.toString(8));
            // console.log("percent:", percent);
            // console.log("ratio:", ratio);
            // console.log("volume:", volume.str);

            this.scopes[scope].summary.price = last;
            this.scopes[scope].summary.percent_str = (isNaN(percent) ? 0 : percent) + "%";
            this.scopes[scope].summary.percent = isNaN(percent) ? 0 : percent;
            this.scopes[scope].summary.volume = volume;

            // console.log("Summary final:", this.scopes[scope].summary);
            // console.log("---------------------------------------------------");
            return summary;
        });

        if (this.scopes[scope] && !force) {
            result = this.scopes[scope].history;
        } else {
            result = aux;
        }

        this.onSummaryChange.next(result);

        return result;
    }

    async getAllTablesSumaries(): Promise<any> {
        return this.waitReady.then(async _ => {
            for (var i in this.tokens) {
                var token = this.tokens[i];
                if (token != this.telos) {
                    this.getTableSummary(token, this.telos, true);
                }
            }
        })
    }
    

    //
    // --------------------------------------------------------------
    // Aux functions
    private auxProcessRowsToOrders(rows:any[]): Order[] {
        var result: Order[] = [];
        for (var i=0; i < rows.length; i++) {
            var price = new Asset(rows[i].price, this);
            var inverse = new Asset(rows[i].inverse, this);
            var selling = new Asset(rows[i].selling, this);
            var total = new Asset(rows[i].total, this);
            var order:Order;

            if (this.telos.symbol == price.token.symbol) {
                order = {
                    id: rows[i].id,
                    price: price,
                    inverse: inverse,
                    total: selling,
                    deposit: selling,
                    telos: total,
                    owner: rows[i].owner
                }
            } else {
                order = {
                    id: rows[i].id,
                    price: inverse,
                    inverse: price,
                    total: total,
                    deposit: selling,
                    telos: selling,
                    owner: rows[i].owner
                }
            }
            result.push(order);
        }
        return result;

/*
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

*/


    }

    private auxGetLabelForHour(hh:number): string {
        var hours = [
            "h.zero",
            "h.one",
            "h.two",
            "h.three",
            "h.four",
            "h.five",
            "h.six",
            "h.seven",
            "h.eight",
            "h.nine",
            "h.ten",
            "h.eleven",
            "h.twelve",
            "h.thirteen",
            "h.fourteen",
            "h.fifteen",
            "h.sixteen",
            "h.seventeen",
            "h.eighteen",
            "h.nineteen",
            "h.twenty",
            "h.twentyone",
            "h.twentytwo",
            "h.twentythree"
        ]
        return hours[hh];
    }

    private auxAssertScope(scope:string): Table {
        var comodity_sym = scope.split(".")[0].toUpperCase();
        var currency_sym = scope.split(".")[1].toUpperCase();
        return this.scopes[scope] || {
            scope: scope,
            deals: 0,
            orders: { sell: [], buy: [] },
            history: [],
            summary: {},
            header: { 
                sell: {total:new Asset("0.0 " + comodity_sym, this), orders:0}, 
                buy: {total:new Asset("0.0 " + currency_sym, this), orders:0}
            },
        };        
    }

    private fetchDeposits(account): Promise<TableResult> {
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
                    scope: account || this.current.name
                });
                for (var i in result.rows) {
                    balances.push(new Asset(result.rows[i].balance, this));
                }
            }
            return balances;
        });
    }

    private fetchOrders(params:TableParams): Promise<TableResult> {
        return this.utils.getTable("sellorders", params).then(result => {
            return result;
        });
    }

    private fetchOrderTables(): Promise<TableResult> {
        return this.utils.getTable("ordertables").then(result => {
            return result;
        });
    }

    private fetchHistory(scope:string, page:number = 0, pagesize:number = 25): Promise<TableResult> {
        return this.utils.getTable("history", {scope:scope, limit:pagesize, lower_bound:""+(page*pagesize)}).then(result => {
            return result;
        });
    }

    private fetchUserOrders(user:string): Promise<TableResult> {
        return this.utils.getTable("userorders", {scope:user, limit:200}).then(result => {
            return result;
        });
    }
    
    private fetchSummary(scope): Promise<TableResult> {
        return this.utils.getTable("tablesummary", {scope:scope}).then(result => {
            return result;
        });
    }

    private fetchTokenStats(token): Promise<TableResult> {
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
    
    constructor(a: any = null, b: any = null) {
        if (a == null && b == null) {
            this.amount = new BigNumber(0);
            this.token = {symbol:"SYS"};
            return;
        }

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
    [key:string]: Table;
}

export interface Table {
    scope: string;
    deals: number;
    orders: TokenOrders;
    history: HistoryTx[];
    summary: Summary;
    header: TableHeader;
}

export interface Summary {
    price?:Asset,
    volume?:Asset,
    percent?:number,
    percent_str?:string,
    records?: any[]
}

export interface TableHeader {
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
    owner: string;
}

export interface UserOrdersMap {
    [key:string]: UserOrders;
}

export interface UserOrders {
    table: string;
    ids: number[];
    orders?:any[];
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