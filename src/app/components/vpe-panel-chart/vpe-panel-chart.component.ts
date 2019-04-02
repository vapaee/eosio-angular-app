import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { VapaeeService } from 'src/app/services/vapaee.service';
import { LocalStringsService } from 'src/app/services/common/common.services';


@Component({
    selector: 'vpe-panel-chart',
    templateUrl: './vpe-panel-chart.component.html',
    styleUrls: ['./vpe-panel-chart.component.scss']
})
export class VpePanelChartComponent implements OnChanges {

    constructor(
        public vapaee: VapaeeService,
        public local: LocalStringsService
    ) {
        
    }

    ngOnChanges() {

    }
}
