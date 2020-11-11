import { Component, OnInit } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';


@Component({
    selector: 'video-page',
    templateUrl: './video.page.html',
    styleUrls: ['./video.page.scss', '../common.page.scss']
})
export class VideoPage implements OnInit {

    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService) {
    }

    ngOnInit() {
        console.log("VideoPage.this.scatter.network: ---> ", this.scatter.network);
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
