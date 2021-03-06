import { Component, OnInit, OnDestroy } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService, AccountData } from 'src/app/services/scatter.service';
import { ActivatedRoute } from '@angular/router';

import { EosioTokenMathService } from 'src/app/services/eosio.token-math.service';
import { BGBoxService } from 'src/app/services/bgbox.service';
import { Subscriber } from 'rxjs';


@Component({
    selector: 'bgbox-page',
    templateUrl: './bgbox.page.html',
    styleUrls: ['./bgbox.page.scss', '../common.page.scss']
})
export class BGBoxPage implements OnInit, OnDestroy {

    apps: any[];
    profiles: any[];
    loading: boolean;
    private subscriber: Subscriber<string>;
    
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        private route: ActivatedRoute,
        public tokenMath: EosioTokenMathService,
        public bgbox: BGBoxService
    ) {
        this.apps = [];
        this.profiles = [];
        this.subscriber = new Subscriber<string>(this.onStateChange.bind(this));
    }

    onStateChange(state:string) {
        console.log("BGBoxPage.onStateChange("+state+")");
        if (state == "bgbox") {
            this.update();
        }
    }

    ngOnDestroy() {
        this.subscriber.unsubscribe();
    }

    ngOnInit() {
        this.update();
        this.app.onStateChange.subscribe(this.subscriber);
    }

    update() {
        console.log("BGBoxPage.update()");
        this.loading = true;
        this.bgbox.getAuthors().then(result => {
            console.log("this.bgbox.getAuthors() authors: ", result.authors);
            console.log("this.bgbox.getAuthors() profiles: ", result.profiles);
            console.log("this.bgbox.getAuthors() apps: ", result.apps);
            this.apps = result.apps;
            this.profiles = result.profiles;
            this.loading = false;
        }).catch(err => {
            this.loading = false;
        });
    }

    droptables() {
        console.log("BGBoxPage.droptables()");
        this.app.loading = true;
        this.bgbox.droptables().then(() => {
            this.app.loading = false;
            this.update();
        }).catch(() => {
            this.app.loading = false;
        });;
    }
}
