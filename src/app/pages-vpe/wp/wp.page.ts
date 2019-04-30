import { Component, OnInit, OnDestroy } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { BGBoxService } from 'src/app/services/bgbox.service';
import { CntService } from 'src/app/services/cnt.service';
import { VapaeeService } from 'src/app/services/vapaee.service';
import { Token, Utils } from 'src/app/services/utils.service';
import { Feedback } from 'src/app/services/feedback.service';
import { CookieService } from 'ngx-cookie-service';


@Component({
    selector: 'vpe-wp-page',
    templateUrl: './wp.page.html',
    styleUrls: ['./wp.page.scss']
})
export class VpeWPPage implements OnInit, OnDestroy {

    public utils: Utils;
    public feed: Feedback;
    public proposalID: string = "16";
    public user_voted_us: boolean;
    public user_dismiss: boolean;
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        public bgbox: BGBoxService,
        public cnt: CntService,
        public vapaee: VapaeeService,
        public cookie: CookieService
    ) {
        this.user_dismiss = !!this.cookie.get("user_dismiss");
        this.utils = new Utils("eosio.trail", this.scatter);
        this.feed = new Feedback();
        this.findOutIfUserVotedUs();
    }

    ngOnDestroy() {
    }

    ngOnInit() {
    }

    resetError() {
        console.log("******************");
        this.feed.setError("voting", null);
    }

    get error() {
        return this.feed.error("voting");
    }

    voteForUs() {
        console.log("voteForUs()");
        this.feed.setLoading("voting", true);
        // telosmain push action eosio.trail castvote '["viterbotelos",16,1]' -p viterbotelos
        // { "voter" : "viterbotelos", "ballot_id" : 16, "direction" : 1 }
        return this.utils.excecute("castvote", {
            voter:  this.scatter.account.name,
            ballot_id: this.proposalID,
            direction: 1
        }).then(async result => {
            var _ = await this.findOutIfUserVotedUs();
            this.feed.setLoading("voting", false);
            return result;
        }).catch(e => {
            if (typeof e == "string") {
                this.feed.setError("voting", e);
            } else {
                this.feed.setError("voting", JSON.stringify(e));
            }
            this.feed.setLoading("voting", false);
        });

    }

    dismiss() {
        this.user_dismiss = true;
        this.cookie.set("user_dismiss", "dismiss");
    }

    findOutIfUserVotedUs(){
        this.user_voted_us = false;
        this.feed.setLoading("user-voted", true);
        this.utils.getTable("votereceipts", {scope:this.vapaee.current.name, limit:1, lower_bound:this.proposalID}).then(result => {
            console.log("**********************", result);
            if (result.rows.length > 0) {
                console.assert(result.rows[0].ballot_id == this.proposalID, result.rows[0].ballot_id, typeof result.rows[0].ballot_id, this.proposalID, typeof this.proposalID);
                if (result.rows[0].directions.length == 1 && result.rows[0].directions[0] == 1) {
                    this.user_voted_us = true;
                } else {
                    this.cookie.delete("user_dismiss");
                }
            }
            this.feed.setLoading("user-voted", false);
        }).catch(e => {
            console.error(e);
            this.feed.setLoading("user-voted", false);
        });
    }
}
