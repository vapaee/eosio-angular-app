import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { VapaeeService, Asset } from 'src/app/services/vapaee.service';
import { LocalStringsService } from 'src/app/services/common/common.services';


@Component({
    selector: 'vpe-panel-wallet',
    templateUrl: './vpe-panel-wallet.component.html',
    styleUrls: ['./vpe-panel-wallet.component.scss']
})
export class VpePanelWalletComponent implements OnChanges {

    @Input() public deposits: Asset[];
    @Input() public balances: Asset[];
    @Input() public actions: boolean;
    @Input() public hideuser: boolean;
    @Input() public hideheader: boolean;
    @Input() public title: string;
    @Input() public loading: boolean;
    @Input() public error: string;

    @Output() confirmDeposit: EventEmitter<any> = new EventEmitter();
    @Output() confirmWithdraw: EventEmitter<any> = new EventEmitter();
    public deposit: Asset;
    public withdraw: Asset;
    constructor(
        public vapaee: VapaeeService,
        public local: LocalStringsService
    ) {
        this.hideuser = false;
        this.hideheader = false;
        this.actions = false;
        this.deposit = new Asset();
        this.withdraw = new Asset();
    }

    depositForm(asset:Asset) {
        if (this.deposit.token.symbol == asset.token.symbol) {
            this.deposit = new Asset();
        } else {
            this.deposit = asset.clone();
        }        
        this.withdraw = new Asset();
    }

    withdrawForm(asset:Asset) {
        if (this.withdraw.token.symbol == asset.token.symbol) {
            this.withdraw = new Asset();
        } else {
            this.withdraw = asset.clone();
        }
        this.deposit = new Asset();
    }

    ngOnChanges() {
        
    }

    onChange() {
        
    }

    onConfirmWithdraw() {
        this.confirmWithdraw.next(this.withdraw);
    }

    onConfirmDeposit() {
        this.confirmDeposit.next(this.deposit);
    }
}
