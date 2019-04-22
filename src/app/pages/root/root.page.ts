import { Component, OnInit, Renderer2, ElementRef } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { ActivatedRoute } from '@angular/router';
import { CntService } from 'src/app/services/cnt.service';

declare var $:any;

@Component({
    selector: 'root-page',
    templateUrl: './root.page.html',
    styleUrls: ['./root.page.scss']
})
export class RootPage implements OnInit {

    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public elRef: ElementRef,
        public scatter: ScatterService,
        public cnt: CntService,
        private route: ActivatedRoute
    ) {
        console.error("NOT AUTHORIZED !! redirect -->  https://vapaee.io");
        window.location.href = "https://vapaee.io";
    }
    
    ngOnInit() {
        var network = this.route.snapshot.paramMap.get('network');
        console.log("RootPage.network: ---> ", network);
        if (network) {
            if ( this.scatter.network.slug != network || !this.scatter.connected ) {
                this.scatter.setNetwork(network).then(() => {
                    if (this.scatter.network.slug != network) {
                        this.app.navigate("/eos/home");
                    }    
                });
                this.scatter.connectApp("Cards & Tokens").catch(err => console.error(err));
            }            
        } else {
            this.scatter.setNetwork("eos");
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

    setNetwork(slug:string, index:number = 0) {
        // this.app.getRouteData()
        this.app.navigatePrefix(slug);
        this.scatter.setNetwork(slug, index);
    }
}
