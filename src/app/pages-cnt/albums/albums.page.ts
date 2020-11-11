import { Component, OnInit, OnDestroy } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { BGBoxService } from 'src/app/services/bgbox.service';
import { CntService } from 'src/app/services/cnt.service';


@Component({
    selector: 'albums-page',
    templateUrl: './albums.page.html',
    styleUrls: ['./albums.page.scss', '../common.page.scss']
})
export class AlbumsPage implements OnInit, OnDestroy {
   
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        public bgbox: BGBoxService,
        public cnt: CntService,
    ) {
    }

    ngOnDestroy() {
    }

    ngOnInit() {
    }
}
