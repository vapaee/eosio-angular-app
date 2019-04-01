import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { VapaeeService, Asset } from 'src/app/services/vapaee.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { Token } from 'src/app/services/utils.service';


@Component({
    selector: 'vpe-order-editor',
    templateUrl: './vpe-order-editor.component.html',
    styleUrls: ['./vpe-order-editor.component.scss']
})
export class VpeOrderEditorComponent implements OnChanges {

    price: Asset;
    amount: Asset;
    @Input() public comodity: Token;
    @Input() public currency: Token;
    constructor(
        public vapaee: VapaeeService,
        public local: LocalStringsService
    ) {
        
    }
    
    onChange(ctrl:any, event:any) {
        // console.log("VpeOrderEditorComponent.onChange("+event+") ---->", ctrl);
    }

    ngOnChanges() {
        this.vapaee.waitReady.then(_ => {
            if (this.comodity) {
                this.amount = new Asset("0.0000 " + this.comodity.symbol, this.vapaee);
            }
            if (this.currency) {
                this.price = new Asset("0.0000 " + this.currency.symbol, this.vapaee);
            }
        });

    }

    debug(obj:any) {
        console.log("-------");
        console.log(obj);
        console.log("-------");
    }
}
