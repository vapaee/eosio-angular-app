import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { VapaeeService, Asset } from 'src/app/services/vapaee.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { Token } from 'src/app/services/utils.service';


@Component({
    selector: 'vpe-panel-tokens-card-deck',
    templateUrl: './vpe-panel-tokens-card-deck.component.html',
    styleUrls: ['./vpe-panel-tokens-card-deck.component.scss']
})
export class VpePanelTokensCardDeckComponent implements OnChanges {

    @Input() public tokens: Token[];
    @Input() public hideheader: boolean;
    @Input() public title: string;
    @Input() public loading: boolean;
    @Input() public error: string;
    @Input() public hidebackground: boolean;
    @Input() public limit: number;


    @Output() confirmDeposit: EventEmitter<any> = new EventEmitter();
    @Output() confirmWithdraw: EventEmitter<any> = new EventEmitter();
    public deposit: Asset;
    public withdraw: Asset;
    constructor(
        public vapaee: VapaeeService,
        public local: LocalStringsService
    ) {
        this.hideheader = false;
        this.hidebackground = false;
        this.limit = 0;
        this.vapaee.waitReady.then(_ => {
            setTimeout(_ => {
                if (this.limit == 0) {
                    this.limit = this.vapaee.tokens.length;
                }    
            });
        });
    }

    get get_tokens() {
        var tokens = []
        for (var i in this.tokens) {
            var token = this.tokens[i];
            if (token.verified) {
                tokens.push(token);
            }
        }
        return tokens
    }

    ngOnChanges() {
        
    }

    onChange() {
        
    }

    summary(_scope) {
        var scope = this.vapaee.scopes[_scope];
        var _summary = Object.assign({
            percent: 0,
            percent_str: "0%",
            price: this.vapaee.zero_telos.clone(),
            records: [],
            volume: this.vapaee.zero_telos.clone()
        }, scope ? scope.summary : {});
        return _summary;
    }

    onConfirmWithdraw() {
        this.confirmWithdraw.next(this.withdraw);
    }

    onConfirmDeposit() {
        this.confirmDeposit.next(this.deposit);
    }
}
