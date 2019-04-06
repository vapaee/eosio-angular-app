import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { VapaeeService, Asset } from 'src/app/services/vapaee.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { Token } from 'src/app/services/utils.service';


@Component({
    selector: 'vpe-panel-order-editor',
    templateUrl: './vpe-panel-order-editor.component.html',
    styleUrls: ['./vpe-panel-order-editor.component.scss']
})
export class VpePanelOrderEditorComponent implements OnChanges {

    payment: Asset;
    fee: Asset;
    money: Asset;
    asset: Asset;
    can_sell: boolean;
    can_buy: boolean;

    wants: string;

    @Input() public comodity: Token;
    @Input() public currency: Token;
    @Input() public deposits: Asset[];
    price: Asset;
    amount: Asset;
    constructor(
        public vapaee: VapaeeService,
        public local: LocalStringsService
    ) {
        
    }

    calculate() {
        setTimeout(_ => {
            var a = this.price.amount;
            this.payment.amount = this.price.amount.multipliedBy(this.amount.amount);
    
            // check if the user can sell. Does he/she have comodity?
            this.asset = new Asset("0.0 " + this.comodity.symbol, this.vapaee);
            this.can_sell = false;
            for (var i in this.deposits) {
                if (this.deposits[i].token == this.comodity) {
                    if (this.deposits[i].amount.toNumber() > 0) {
                        this.can_sell = true;
                        this.asset = this.deposits[i];
                    }
                }
            }
            // Does he/she have enough comodity?
            if (this.can_sell) {
                if (this.asset.amount.isLessThan(this.amount.amount)) {
                    this.can_sell = false;
                }
            }
    
            // check if the user can buy. Does he/she have currency?
            this.money = new Asset("0.0 " + this.currency.symbol, this.vapaee);
            this.can_buy = false;
            for (var i in this.deposits) {
                if (this.deposits[i].token == this.currency) {
                    if (this.deposits[i].amount.toNumber() > 0) {
                        this.can_buy = true;
                        this.money = this.deposits[i];
                    }
                }
            }
            // Does he/she have enough comodity?
            if (this.can_buy) {
                if (this.money.amount.isLessThan(this.payment.amount)) {
                    this.can_buy = false;
                }
            }
            
            if (!this.wants) {
                this.wantsTo("sell");
                this.wantsTo("buy");
            }

            /*if (!this.can_buy && !this.can_sell) {
                this.wants = "";
            }*/
    
            console.log("VpeOrderEditorComponent.calculate() ---->", [this.deposits, this.money, this.asset]);    
        });
    }

    wantsTo(what) {
        console.assert(what == "sell" || what == "buy", "ERROR: wantsTo what??", what);
        if (what == "sell" && !this.can_sell) return;
        if (what == "buy" && !this.can_buy) return;
        this.wants = what;
        this.calculate();
    }

    setPrice(a:Asset) {
        this.price = a;
        this.ngOnChanges();
    }
    
    setAmount(a:Asset) {
        this.amount = a;
        this.ngOnChanges();
    }
    
    onChange(event:any) {
        this.calculate();
    }

    reset() {
        this.amount = null;
        this.payment = null;
        this.comodity = null;
        this.currency = null;
        this.price = null;
        this.ngOnChanges();
    }

    ngOnChanges() {
        console.log("VpePanelOrderEditorComponent.ngOnChanges()");
        // changes from outside
        this.vapaee.waitReady.then(_ => {
            if (this.comodity && !this.amount) {
                this.amount = new Asset("0.0000 " + this.comodity.symbol, this.vapaee);
            }

            if (this.currency && !this.payment) {
                this.payment = new Asset("0.0000 " + this.currency.symbol, this.vapaee);
            }

            if (this.currency && !this.price) {
                this.price = new Asset("0.0000 " + this.currency.symbol, this.vapaee);
            }

            if (this.price && this.amount && this.deposits && this.deposits.length > 0) {
                this.calculate();
            }
        });
    }

    debug(obj:any) {
        console.log("-------");
        console.log(obj);
        console.log("-------");
    }

    buy() {
        if (!this.can_buy) return;
        console.log("BUY");
        this.vapaee.createOrder("buy", this.amount, this.price);
    }

    sell() {
        if (!this.can_sell) return;
        console.log("SELL");
        this.vapaee.createOrder("sell", this.amount, this.price);
    }

}

