import { Component, OnInit, OnDestroy } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { BGBoxService } from 'src/app/services/bgbox.service';
import { CntService } from 'src/app/services/cnt.service';
import { ActivatedRoute } from '@angular/router';
import { Token } from 'src/app/services/utils.service';
import { VapaeeService, Asset } from 'src/app/services/vapaee.service';
import { Subscriber } from 'rxjs';


@Component({
    selector: 'trade-page',
    templateUrl: './trade.page.html',
    styleUrls: ['./trade.page.scss', '../common.page.scss']
})
export class TradePage implements OnInit, OnDestroy {

    scope:string;
    comodity:Token;
    currency:Token;
    private onStateSubscriber: Subscriber<string>;
   
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        public bgbox: BGBoxService,
        public cnt: CntService,
        public vapaee: VapaeeService,
        public route: ActivatedRoute

    ) {
        this.onStateSubscriber = new Subscriber<string>(this.onStateChange.bind(this));
    }      

    async init() {
        this.scope = this.route.snapshot.paramMap.get('scope');
        var com:string = this.scope.split(".")[0];
        var cur:string = this.scope.split(".")[1];        
        this.comodity = await this.vapaee.getToken(com);
        this.currency = await this.vapaee.getToken(cur);

        

        this.vapaee.getSellOrders(this.comodity, this.currency);
        this.vapaee.getBuyOrders(this.comodity, this.currency);
        this.vapaee.getTransactionHistory(this.comodity, this.currency);
        this.vapaee.getDeposits();
        console.log("ORDERS:", this.orders);
    }

    get deposits(): Asset[] {
        return this.vapaee.deposits;
    }

    get balances(): Asset[] {
        return this.vapaee.balances;
    }

    get history() {
        var scope = this.vapaee.scopes[this.scope];
        return scope ? scope.history : [];
    }

    get orders() {
        var scope = this.vapaee.scopes[this.scope];
        return scope ? scope.orders : {sell:[],buy:[]};
    }

    get tokens() {
        return this.vapaee.tokens;
    }

    ngOnDestroy() {
        this.onStateSubscriber.unsubscribe();
    }

    ngOnInit() {
        this.init();
        this.app.onStateChange.subscribe(this.onStateSubscriber);
    }

    onStateChange(state:string) {
        console.log("TradePage.onStateChange("+state+")");
        if (state == "trade") {
            this.init();
        }
    } 
}
