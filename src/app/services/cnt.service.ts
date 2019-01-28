import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { ScatterService } from './scatter.service';
import { BGBoxService, Publisher } from './bgbox.service';
import BigNumber from 'bignumber.js';

@Injectable()
export class CntService {

    public onPublisherChange:Subject<Publisher> = new Subject();
    public loginState: string;
    /*
    public loginState: string;
    - 'no-scatter': Scatter no detected
    - 'no-logged': Scatter detected but user is not logged
    - 'no-publishers': User logged but has no publishers in bg-box
    - 'no-selected': usr has several publishers but none selected 
    - 'no-registered': user has a selected publisher but is not registered in C&T
    - 'publisher-ok': user has selected a registered publisher (has C&T inventories in BG-Box memory)
    */

    public publisher: Publisher;
    public publishers: Publisher[];
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
        this.resetPublishers();
        this.contract = this.cardsntokens;
        this.scatter.onLogggedStateChange.subscribe(this.updateLogState.bind(this));
        this.onPublisherChange.subscribe(this.updateLogState.bind(this));

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
            }
        });
    }

    resetPublishers() {
        this.publishers = [];
        this.publisher = {slugid:{str:"guest"}, account:this.contract};
        this.onPublisherChange.next(this.publisher);
    }

    login() {
        this.resetPublishers();
        this.scatter.login().then(() => {
            this.updateLogState();
        })
    }

    fetchPublisher(publisher:string) {
        return this.bgbox.getAuthorBySlug(publisher);
    }

    selectPublisher(publisher:any) {
        if (typeof publisher  == "string") {
            for (var i in this.publishers) {
                if (this.publishers[i].slugid.str == publisher) {
                    if (this.publisher != this.publishers[i]) {
                        this.publisher = this.publishers[i];
                        this.onPublisherChange.next(this.publisher);
                    }
                }
            }
        } else {
            if (this.publisher != publisher) {
                this.publisher = publisher;
                this.publishers.push(publisher);
                this.onPublisherChange.next(this.publisher);
            }            

        }
        console.log("CntService.selectPublisher(",[publisher],")");
        this.updateLogState();
    }

    registerPublisher() {
        console.log("CntService.registerPublisher()");
        return this.bgbox.signUpPublisherForApp(
            this.scatter.account.name,
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
                this.loginState = "no-publishers";
                if (this.publishers.length == 0) {
                    this.bgbox.getAuthorsFor(this.scatter.account.name).then(data => {
                        if (data.publishers.length > 0) {
                            this.loginState = "no-selected";
                            this.publishers = [];
                            for (var i in data.authors) {
                                this.publishers.push(data.authors[i]);
                            }
                        }
                    });
                } else {
                    if (!this.publisher || this.publisher.slugid.str == "guest") {
                        this.loginState = "no-selected";
                    } else {
                        this.loginState = "no-registered";
                    }
                }
            }
        })
    }

}
