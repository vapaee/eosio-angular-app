import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { VapaeeService } from 'src/app/services/vapaee.service';
import { LocalStringsService } from 'src/app/services/common/common.services';


@Component({
    selector: 'vpe-wallet',
    templateUrl: './vpe-wallet.component.html',
    styleUrls: ['./vpe-wallet.component.scss']
})
export class VpeWalletComponent implements OnChanges {

    @Input() public deposits: any[];
    constructor(
        public vapaee: VapaeeService,
        public local: LocalStringsService
        ) {
        
    }

    ngOnChanges() {

    }
}
