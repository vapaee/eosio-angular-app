import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { Token } from 'src/app/services/utils.service';
import { Subscriber } from 'rxjs';


@Component({
    selector: 'vpe-tokens',
    templateUrl: './vpe-tokens.component.html',
    styleUrls: ['./vpe-tokens.component.scss']
})
export class VpeTokensComponent implements OnChanges {

    @Input() public tokens: Token[];
    
    
    constructor() {
        
    }
    

    ngOnChanges() {

    }

    onStateChange() {

    }
}
