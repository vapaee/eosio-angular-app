import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { TokenOrders } from 'src/app/services/vapaee.service';


@Component({
    selector: 'vpe-orders',
    templateUrl: './vpe-orders.component.html',
    styleUrls: ['./vpe-orders.component.scss']
})
export class VpeOrdersComponent implements OnChanges {

    @Input() public orders: TokenOrders;
    constructor() {
        
    }

    ngOnChanges() {

    }
}
