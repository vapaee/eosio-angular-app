import { Component, OnInit, ViewChild, Input, OnChanges } from '@angular/core';

declare var jdenticon:any;
declare var $:any;

@Component({
    selector: 'identicon-comp',
    templateUrl: './identicon.component.html',
    styleUrls: ['./identicon.component.scss']
})
export class IdenticonComponent implements OnChanges {

    @ViewChild('svg') private svg;
    @Input() value: string;
    
    constructor() {
        
    }

    ngOnChanges() {
        console.log("IdenticonComponent.ngOnChanges()", this.value, typeof this.value);
        if (typeof this.value == "string") {
            $(this.svg.nativeElement).attr("data-jdenticon-value", this.value);
            jdenticon();
        }
    }
}
