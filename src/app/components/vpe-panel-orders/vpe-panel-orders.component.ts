import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { TokenOrders, VapaeeService } from 'src/app/services/vapaee.service';
import { LocalStringsService } from 'src/app/services/common/common.services';


@Component({
    selector: 'vpe-panel-orders',
    templateUrl: './vpe-panel-orders.component.html',
    styleUrls: ['./vpe-panel-orders.component.scss']
})
export class VpePanelOrdersComponent implements OnChanges {

    @Input() public orders: TokenOrders;
    constructor(
        public vapaee: VapaeeService,
        public local: LocalStringsService
    ) {
        
    }

    ngOnChanges() {

    }
}
