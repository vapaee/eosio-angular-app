import { Component, OnInit } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService, AccountData } from 'src/app/services/scatter.service';
import { ActivatedRoute } from '@angular/router';

import { EosioAccountComponent } from 'src/app/components/eosio-account/eosio-account.component';
import { IdenticonComponent } from 'src/app/components/identicon/identicon.component';
import { EosioTokenMathService } from 'src/app/services/eosio.token-math.service';
import { Observable, Subject } from 'rxjs';

@Component({
    selector: 'newapp-page',
    templateUrl: './newapp.page.html',
    styleUrls: ['./newapp.page.scss', '../common.page.scss']
})
export class NewAppPage implements OnInit {

    data: any;
    last_trx:string;
    
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        private route: ActivatedRoute,
        public tokenMath: EosioTokenMathService
    ) {
        this.data = {
            nick: "cards-and-tokens",
            title: "Cards & Tokens"
        };
    }
    
    ngOnInit() {
        this.scatter.waitReady.then(() => {
            this.data.owner = this.scatter.account ? this.scatter.account.name : "unknown";
        });
    }

    register() {
        console.log("NewAppPage.register()");
        this.app.loading = true;
        try {
            this.scatter.getContract("vapaeeauthor").then(contract => {
                try {
                    const transactionOptions = {
                        authorization:[`${this.scatter.account.name}@${this.scatter.account.authority}`]
                    };
                    contract.registernick({
                        owner:  this.data.owner,
                        nickstr: this.data.nick,
                        title: this.data.title
                    }, transactionOptions).then((response => {
                        console.log("response", response);
                        this.last_trx = response.transaction_id;
                        this.app.loading = false;
                    })).catch(err => {
                        console.error("err", err);
                        this.app.loading = false;
                    });
                } catch (e) {
                    console.error("err", e);
                    this.app.loading = false;
                }
            }).catch(err => {
                console.error("err", err);
                this.app.loading = false;
            });
        } catch (e) {
            console.error("err", e);
            this.app.loading = false;
        }
    }


    testScatterOnLocalNetwork(){
        this.scatter.testScatterOnLocalNetwork();
    }


}
