import { Component, OnInit, Renderer2, ElementRef } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { ActivatedRoute } from '@angular/router';

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
        // this is to resolve the /:network part of the url path -------
        public scatter: ScatterService,
        private route: ActivatedRoute
    ) {
        console.log("CONSTRUCTOR ROOT");
    }
    
    ngOnInit() {
        var network = this.route.snapshot.paramMap.get('network');
        console.log("RootPage.network: ---> ", network);
        if (network) {
            this.scatter.setNetwork(network).then(() => {
                if (this.scatter.network.slug != network) {
                    this.app.navigate("/eos/home");
                }    
            });
        } else {
            this.scatter.setNetwork("eos");
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
