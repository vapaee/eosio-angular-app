import { Component, OnInit, OnDestroy } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { BGBoxService } from 'src/app/services/bgbox.service';
import { CntService } from 'src/app/services/cnt.service';
import { Subscriber } from 'rxjs';
import { ActivatedRoute } from '@angular/router';
import { Profile } from 'src/app/services/utils.service';
import { VapaeeService } from 'src/app/services/vapaee.service';


@Component({
    selector: 'vpe-profile-page',
    templateUrl: './profile.page.html',
    styleUrls: ['./profile.page.scss', '../common.page.scss']
})
export class VpeProfilePage implements OnInit, OnDestroy {

    private subscriber: Subscriber<string>;
   
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        public route: ActivatedRoute,
        public bgbox: BGBoxService,
        public vapaee: VapaeeService,
    ) {
        this.subscriber = new Subscriber<string>(this.onCntCurrentProfileChange.bind(this));
    }

    ngOnDestroy() {
        this.subscriber.unsubscribe();
    }

    ngOnInit() {
        console.log("ProfilePage.ngOnInit()");
        // this.cnt.updateLogState();
        this.vapaee.onCurrentAccountChange.subscribe(this.subscriber);
        var profile = this.route.snapshot.paramMap.get('profile');       
    }

    onCntCurrentProfileChange(profile: Profile) {
        var url = "/exchange/profile/";
        if (profile) {
            url += "profile.slugid.str";
        } else {
            url += "guest";
        };
        console.log("ProfilePage.onCntCurrentProfileChange()", [profile], " --> ", url);
        this.app.navigate(url);
    }

}
