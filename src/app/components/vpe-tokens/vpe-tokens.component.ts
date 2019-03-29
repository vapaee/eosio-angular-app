import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { Token } from 'src/app/services/utils.service';
import { Subscriber } from 'rxjs';
import { VapaeeService } from 'src/app/services/vapaee.service';
import { LocalStringsService } from 'src/app/services/common/common.services';


@Component({
    selector: 'vpe-tokens',
    templateUrl: './vpe-tokens.component.html',
    styleUrls: ['./vpe-tokens.component.scss']
})
export class VpeTokensComponent implements OnChanges {

    @Input() public tokens: Token[];
    token_filter:string;
    
    
    constructor(
        public vapaee: VapaeeService,
        public local: LocalStringsService
        ) {
        this.token_filter = "";
    }
    

    ngOnChanges() {

    }

    onStateChange() {

    }
}
