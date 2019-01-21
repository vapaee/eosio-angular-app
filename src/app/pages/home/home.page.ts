import { Component, OnInit } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { HttpClient } from '@angular/common/http';
import { Title } from '@angular/platform-browser';


@Component({
    selector: 'home-page',
    templateUrl: './home.page.html',
    styleUrls: ['./home.page.scss', '../common.page.scss']
})
export class HomePage implements OnInit {

    numblock: number = 0;
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        public http: HttpClient
        ) {
    }

    ngOnInit() {
        console.log("HomePage.this.scatter.network: ---> ", this.scatter.network);
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
