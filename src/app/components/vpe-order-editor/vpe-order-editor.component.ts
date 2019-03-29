import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { VapaeeService } from 'src/app/services/vapaee.service';
import { LocalStringsService } from 'src/app/services/common/common.services';


@Component({
    selector: 'vpe-order-editor',
    templateUrl: './vpe-order-editor.component.html',
    styleUrls: ['./vpe-order-editor.component.scss']
})
export class VpeOrderEditorComponent implements OnChanges {

    constructor(
        public vapaee: VapaeeService,
        public local: LocalStringsService
        ) {
        
    }

    ngOnChanges() {

    }
}
