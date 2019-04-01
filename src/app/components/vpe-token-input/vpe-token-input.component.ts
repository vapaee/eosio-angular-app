import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { Asset, VapaeeService } from 'src/app/services/vapaee.service';


@Component({
    selector: 'vpe-token-input',
    templateUrl: './vpe-token-input.component.html',
    styleUrls: ['./vpe-token-input.component.scss']
})
export class VpeTokenInputComponent implements OnChanges {

    private prev: string;
    @Input() public asset: Asset;
    @Input() public placeholder: string;
    @Input() public precision: number;
    @Input() public disabled: boolean;
    @Output() valueChange: EventEmitter<any> = new EventEmitter();
    public text: string;
    constructor(
        public vapaee: VapaeeService
    ) {
        this.precision = -1;
    }

    init() {
        
    }

    private updateAsset(ctrl:any, text:any) {
        try {
            if (this.asset && text.length > 0) {
                var newAsset = new Asset(text + " " + this.asset.token.symbol, this.vapaee);
                this.asset.amount = newAsset.amount;
                this.ngOnChanges();
            }
        } catch(e) {
            console.error(e);
        }
    }

    onChange(ctrl:any, text:any) {
        if (text == "") text = "0.0";
        this.updateAsset(ctrl, text);
    }

    onBlur(ctrl:any, event:any) {
        this.ngOnChanges();
    }

    ngOnChanges() {
        if (this.asset) {
            var precision = this.asset.token.precision;
            if (this.precision != -1) precision = this.precision;
            this.text = this.asset.toString(precision).split(" ")[0];
            if (this.text != this.prev) {
                this.prev = this.text;
                this.valueChange.emit(this.asset);
            }    
        }
    }
}
