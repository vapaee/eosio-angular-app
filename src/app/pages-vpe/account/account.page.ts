import { Component, OnInit, OnDestroy } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { BGBoxService } from 'src/app/services/bgbox.service';
import { CntService } from 'src/app/services/cnt.service';
import { Subscriber } from 'rxjs';
import { ActivatedRoute } from '@angular/router';
import { Profile } from 'src/app/services/utils.service';
import { VapaeeService, Asset } from 'src/app/services/vapaee.service';


@Component({
    selector: 'vpe-account-page',
    templateUrl: './account.page.html',
    styleUrls: ['./account.page.scss', '../common.page.scss']
})
export class VpeAccountPage implements OnInit, OnDestroy {

    private subscriber: Subscriber<string>;
    current_mode: boolean;
   
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

    updateAccount() {
        // this.vapaee.account
        
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
        this.vapaee.deposit(amount).then(_ => {
            console.log("------------------>", amount.toString());
        });
    }

    onWalletConfirmWithdraw(amount: Asset) {
        console.log("------------------>", amount.toString());
        this.vapaee.withdraw(amount).then(_ => {
            console.log("------------------>", amount.toString());
        });
    }

}
