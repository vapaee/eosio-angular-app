import { Component, OnInit, OnDestroy } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { BGBoxService } from 'src/app/services/bgbox.service';
import { CntService } from 'src/app/services/cnt.service';
import { Subscriber } from 'rxjs';
import { ActivatedRoute } from '@angular/router';
import { Profile } from 'src/app/services/utils.service';


@Component({
    selector: 'vpe-profile-page',
    templateUrl: './profile.page.html',
    styleUrls: ['./profile.page.scss', '../common.page.scss']
})
export class VpeProfilePage implements OnInit, OnDestroy {

    private subscriber: Subscriber<Profile>;
   
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        public route: ActivatedRoute,
        public bgbox: BGBoxService,
        public cnt: CntService,
    ) {
        this.subscriber = new Subscriber<Profile>(this.onCntCurrentProfileChange.bind(this));
    }

    ngOnDestroy() {
        this.subscriber.unsubscribe();
    }

    ngOnInit() {
        console.log("ProfilePage.ngOnInit()");
        // this.cnt.updateLogState();
        this.cnt.onCurrentProfileChange.subscribe(this.subscriber);
        var profile = this.route.snapshot.paramMap.get('profile');
        if (profile != "guest") {
            this.cnt.fetchProfile(profile).then(this.cnt.setCurrentProfile.bind(this.cnt));
        }
        
    }

    onCntCurrentProfileChange(profile: Profile) {
        var url = "/cnt/profile/" + profile.slugid.str;
        console.log("ProfilePage.onCntCurrentProfileChange()", [profile], " --> ", url);
        this.app.navigate(url);
    }

    selectProfile(user) {
        this.cnt.selectProfile(user);
    }

}
