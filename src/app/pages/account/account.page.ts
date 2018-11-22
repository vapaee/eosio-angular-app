import { Component, OnInit } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService, AccountData } from 'src/app/services/scatter.service';
import { ActivatedRoute } from '@angular/router';

import { EosioAccountComponent } from 'src/app/components/eosio-account/eosio-account.component';
import { IdenticonComponent } from 'src/app/components/identicon/identicon.component';

@Component({
    selector: 'account-page',
    templateUrl: './account.page.html',
    styleUrls: ['./account.page.scss', '../common.page.scss']
})
export class AccountPage implements OnInit {

    account: AccountData;
    data: any;
    response: any;
    last_trx:string;

    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        private route: ActivatedRoute
    ) {

        

        this.data = {
            to: "dailyselfies",
            amount: "0.0001 EOS",
            memo: "testing"
        };

        this.account = {
            account_name:"guesst"
        };
        var name = this.route.snapshot.paramMap.get('name');
        // console.log("name", name);
        if (name) {
            this.account.account_name = name;
            this.scatter.queryAccountData(name).then((account) => {
                this.account = account;
                console.log("this.account", this.account);
            });
        } else {
            this.scatter.waitReady.then(() => {
                if (this.scatter.logged) {
                    this.app.navigate("account/" + this.scatter.account.name);
                }
            });
        }
    }    

    ngOnInit() {}

    login() {
        this.scatter.login().then(() => {
            if (this.scatter.logged) {
                this.app.navigate("account/" + this.scatter.account.name);
            }
        });
    }

    transfer() {
        // this.scatter.transfer(this.scatter.account.name, this.data.to, this.data.amount, this.data.memo);        
        this.scatter.getContract("eosio.token").then(contract => {
            contract.transfer({
                from:  this.scatter.account.name,
                memo: this.data.memo,
                quantity: this.data.amount,
                to: this.data.to
            }).then((response => {
                console.log("response", response);
                this.last_trx = response.transaction_id;
            }));
        })        
    }

    gas() {
        this.scatter.getContract("eosio.token").then(contract => {
            // console.log("contract.fc.schema.transfer.fields", contract.fc.schema.transfer.fields);
            // console.log("contract.fc.schema.issue.fields", contract.fc.schema.issue.fields);
            for (var i in contract) {
                console.log("contract."+i, typeof contract[i], [contract[i]]);
            }
        })
    }
}
