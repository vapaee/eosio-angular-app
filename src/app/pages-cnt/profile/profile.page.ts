import { Component, OnInit, OnDestroy } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { BGBoxService, Publisher } from 'src/app/services/bgbox.service';
import { CntService } from 'src/app/services/cnt.service';
import { Subscriber } from 'rxjs';
import { ActivatedRoute } from '@angular/router';


@Component({
    selector: 'profile-page',
    templateUrl: './profile.page.html',
    styleUrls: ['./profile.page.scss', '../common.page.scss']
})
export class ProfilePage implements OnInit, OnDestroy {

    private subscriber: Subscriber<Publisher>;
   
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        public route: ActivatedRoute,
        public bgbox: BGBoxService,
        public cnt: CntService,
    ) {
        this.subscriber = new Subscriber<Publisher>(this.onPublisherChange.bind(this));
    }

    ngOnDestroy() {
        this.subscriber.unsubscribe();
    }

    ngOnInit() {
        console.log("ProfilePage.ngOnInit()");
        this.cnt.updateLogState();
        this.cnt.onPublisherChange.subscribe(this.subscriber);
        var publisher = this.route.snapshot.paramMap.get('publisher');
        if (publisher != "guest") {
            this.cnt.fetchPublisher(publisher).then(this.cnt.selectPublisher.bind(this.cnt));
        }
    }

    onPublisherChange(publisher: Publisher) {
        console.log("*************** ProfilePage.onPublisherChange()", [publisher]);
        this.app.navigate("/cnt/profile/" + publisher.slugid.str);
    }

    selectPublisher(user) {
        this.cnt.selectPublisher(user);
    }

}
