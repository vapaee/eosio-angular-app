import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { ScatterService } from './scatter.service';
import { BGBoxService, Profile } from './bgbox.service';
import BigNumber from 'bignumber.js';

@Injectable()
export class CntService {

    public onProfileChange:Subject<Profile> = new Subject();
    public loginState: string;
    /*
    public loginState: string;
    - 'no-scatter': Scatter no detected
    - 'no-logged': Scatter detected but user is not logged
    - 'no-profiles': User logged but has no profiles in bg-box
    - 'no-selected': usr has several profiles but none selected 
    - 'no-registered': user has a selected profile but is not registered in C&T
    - 'profile-ok': user has selected a registered profile (has C&T inventories in BG-Box memory)
    */

    public profile: Profile;
    public profiles: Profile[];
    public contract:string;
    cardsntokens:string = "cardsntokens";
    cntAuthorId: string;

    private setReady: Function;
    public waitReady: Promise<any> = new Promise((resolve) => {
        this.setReady = resolve;
    });
    constructor(
        private scatter: ScatterService,
        private bgbox: BGBoxService
        ) {
        this.resetProfiles();
        this.contract = this.cardsntokens;
        this.scatter.onLogggedStateChange.subscribe(this.updateLogState.bind(this));
        this.onProfileChange.subscribe(this.updateLogState.bind(this));

        var encodedName = this.bgbox.utils.encodeName(this.cardsntokens);

        var params = {
            lower_bound: encodedName.toString(), 
            upper_bound: encodedName.toString(), 
            limit: 1, 
            key_type: "i64",
            index_position: "2"
        }            
        this.bgbox.getTable("apps", params).then(result => {
            if (result.rows.length > 0) {
                this.cntAuthorId = result.rows[0].id;
                console.log("****************************************");
                console.log("****************************************");
                console.log("****************************************");
                console.log("this.cntAuthorId:", this.cntAuthorId, [result]);
                console.log("****************************************");
                console.log("****************************************");
                console.log("****************************************");
                console.log("****************************************");
            }
        });
    }

    resetProfiles() {
        this.profiles = [];
        this.profile = {slugid:{str:"guest"}, account:this.contract};
        this.onProfileChange.next(this.profile);
    }

    login() {
        this.resetProfiles();
        this.scatter.login().then(() => {
            this.updateLogState();
        })
    }

    fetchProfile(profile:string) {
        return this.bgbox.getAuthorBySlug(profile);
    }

    selectProfile(profile:any) {
        if (typeof profile  == "string") {
            for (var i in this.profiles) {
                if (this.profiles[i].slugid.str == profile) {
                    if (this.profile != this.profiles[i]) {
                        this.profile = this.profiles[i];
                        this.onProfileChange.next(this.profile);
                    }
                }
            }
        } else {
            if (this.profile != profile) {
                this.profile = profile;
                this.profiles.push(profile);
                this.onProfileChange.next(this.profile);
            }            

        }
        console.log("CntService.selectProfile(",[profile],")");
        this.updateLogState();
    }

    registerProfile() {
        console.log("CntService.registerProfile()");
        return this.bgbox.signUpProfileForApp(
            this.scatter.account.name,
            this.profile.id,
            this.cntAuthorId,
            this.bgbox.contract);
    }

    updateLogState() {
        console.log("CntService.updateLogState()");
        this.loginState = "no-scatter";
        this.scatter.waitConnected.then(() => {
            this.loginState = "no-logged";
            console.log("this.scatter", [this.scatter]);
            if (this.scatter.logged) {
                this.loginState = "no-profiles";
                if (this.profiles.length == 0) {
                    this.bgbox.getAuthorsFor(this.scatter.account.name).then(data => {
                        if (data.profiles.length > 0) {
                            this.loginState = "no-selected";
                            this.profiles = [];
                            for (var i in data.authors) {
                                this.profiles.push(data.authors[i]);
                            }
                        }
                    });
                } else {
                    if (!this.profile || this.profile.slugid.str == "guest") {
                        this.loginState = "no-selected";
                    } else {
                        // TODO: tengo que averiguar si ya está registrado y ponerlo como 'profile-ok'
                        console.log("TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!");
                        this.loginState = "no-registered";
                    }
                }
            }
        })
    }

}
