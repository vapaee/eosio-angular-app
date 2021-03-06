import { Component, OnInit, Renderer2, ElementRef } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { ActivatedRoute } from '@angular/router';
import { CntService } from 'src/app/services/cnt.service';
import { BGBoxService } from 'src/app/services/bgbox.service';

declare var $:any;

@Component({
    selector: 'cntroot-page',
    templateUrl: './cntroot.page.html',
    styleUrls: ['./cntroot.page.scss']
})
export class CntRootPage implements OnInit {

    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public elRef: ElementRef,
        public scatter: ScatterService,
        public cnt: CntService,
        public bgbox: BGBoxService
    ) {
        console.error("NOT AUTHORIZED !! redirect -->  https://vapaee.io");
        window.location.href = "https://vapaee.io";
    }
    
    ngOnInit() {
        if ( this.scatter.network.slug != "local" || !this.scatter.connected ) {
            this.scatter.setNetwork("local");
            this.scatter.connectApp("Cards & Tokens").catch(err => console.error(err));    
        }
    }

    collapseMenu() {
        var target = this.elRef.nativeElement.querySelector("#toggle-btn");
        var navbar = this.elRef.nativeElement.querySelector("#navbar");
        if (target && $(navbar).hasClass("show")) {
            $(target).click();
        }
    }
    
    debug(){
        console.log("--------------------------------");
        console.log("CNT", [this.cnt]);
        console.log("BOX", [this.bgbox]);
        console.log("--------------------------------");
    }

}
