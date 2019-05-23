import { Component, OnInit, Renderer2, ElementRef } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { ActivatedRoute } from '@angular/router';
import { CntService } from 'src/app/services/cnt.service';
import { BGBoxService } from 'src/app/services/bgbox.service';
import { VapaeeService } from 'src/app/services/vapaee.service';

declare var $:any;

@Component({
    selector: 'vpe-root-page',
    templateUrl: './vpe-root.page.html',
    styleUrls: ['./vpe-root.page.scss']
})
export class VpeRootPage implements OnInit {

    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public elRef: ElementRef,
        public scatter: ScatterService,
        public cnt: CntService,
        public bgbox: BGBoxService,
        public vapaee: VapaeeService
    ) {
    }
    
    ngOnInit() {
        var network = "telos-testnet";
        network = "telos";
        // network = "local";
        if ( this.scatter.network.slug != network || !this.scatter.connected ) {
            this.scatter.setNetwork(network);
            this.scatter.connectApp("Vapaée - Telos DEX").catch(err => console.error(err));    
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
        console.log("VPE", [this.vapaee]);
        console.log("Scatter", [this.scatter]);
        console.log("--------------------------------");
    }

}
