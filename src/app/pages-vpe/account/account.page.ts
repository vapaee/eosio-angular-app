import { Component, OnInit, OnDestroy } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { BGBoxService } from 'src/app/services/bgbox.service';
import { CntService } from 'src/app/services/cnt.service';
import { Subscriber } from 'rxjs';
import { ActivatedRoute } from '@angular/router';
import { Profile } from 'src/app/services/utils.service';
import { VapaeeService, Asset, UserOrders, UserOrdersMap } from 'src/app/services/vapaee.service';


@Component({
    selector: 'vpe-account-page',
    templateUrl: './account.page.html',
    styleUrls: ['./account.page.scss', '../common.page.scss']
})
export class VpeAccountPage implements OnInit, OnDestroy {

    private subscriber: Subscriber<string>;
    current_mode: boolean;
    loading: boolean;
    error: string;
   
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        public route: ActivatedRoute,
        public bgbox: BGBoxService,
        public vapaee: VapaeeService,
    ) {
        this.subscriber = new Subscriber<string>(this.onCntCurrentAccountChange.bind(this));
        this.current_mode = true;
        // this.onCntCurrentAccountChange('guest');
    }


    get deposits(): Asset[] {
        return this.vapaee.deposits;
    }

    get balances(): Asset[] {
        return this.vapaee.balances;
    }    

    get userorders(): UserOrdersMap {
        return this.vapaee.userorders;
    }    

    updateAccount() {
    }

    ngOnDestroy() {
        this.subscriber.unsubscribe();
    }

    ngOnInit() {
        console.log("VpeAccountPage.ngOnInit()");
        // this.cnt.updateLogState();
        this.vapaee.onCurrentAccountChange.subscribe(this.subscriber);
        var name = this.route.snapshot.paramMap.get('name');
        this.onCntCurrentAccountChange(name);
    }

    onCntCurrentAccountChange(account: string) {
        console.log("VaeProfilePage.onCntCurrentAccountChange() ----------------->", account);
        var url = "/exchange/account/";
        if (account) {
            url += account;
        } else {
            url += "guest";
        };
        console.log("accountPage.onCntCurrentAccountChange()", [account], " --> ", url);
        this.app.navigate(url);
    }

    onWalletConfirmDeposit(amount: Asset) {
        console.log("------------------>", amount.toString());
        this.loading = true;
        this.error = null;
        this.vapaee.deposit(amount).then(_ => {
            console.log("------------------>", amount.toString());
            this.loading = false;
        }).catch(e => {
            console.error(typeof e, e);
            // this.error = "ERROR: " + JSON.stringify(typeof e == "string" ? JSON.parse(e) : e, null, 4);
            if (typeof e == "string") {
                this.error = "ERROR: " + JSON.stringify(JSON.parse(e), null, 4);
            } else {
                this.error = null;
            }
            this.loading = false;
        });
    }

    onWalletConfirmWithdraw(amount: Asset) {
        console.log("------------------>", amount.toString());
        this.loading = true;
        this.error = null;
        this.vapaee.withdraw(amount).then(_ => {
            console.log("------------------>", amount.toString());
            this.loading = false;
        }).catch(e => {
            console.error(typeof e, e);
            // this.error = "ERROR: " + JSON.stringify(typeof e == "string" ? JSON.parse(e) : e, null, 4);
            if (typeof e == "string") {
                this.error = "ERROR: " + JSON.stringify(JSON.parse(e), null, 4);
            } else {
                this.error = null;
            }
            this.loading = false;
        });        
    }

}
