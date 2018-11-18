import { Component, OnInit } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';

@Component({
    selector: 'home-page',
    templateUrl: './home.page.html',
    styleUrls: ['./home.page.scss', '../common.page.scss']
})
export class HomePage implements OnInit {

    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService) {
    }

    ngOnInit() {
    }

    login() {
        this.scatter.login().then((identity) => {
            console.log("identity", identity);
        });
    }

    logout() {
        this.scatter.logout();
    }
}
