import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { HistoryTx } from 'src/app/services/vapaee.service';


@Component({
    selector: 'vpe-history',
    templateUrl: './vpe-history.component.html',
    styleUrls: ['./vpe-history.component.scss']
})
export class VpeHistoryComponent implements OnChanges {

    @Input() public history: HistoryTx[];
    constructor() {
        
    }

    ngOnChanges() {

    }
}
