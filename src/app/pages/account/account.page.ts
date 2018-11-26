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
        private route: ActivatedRoute,
        public tokenMath: EosioTokenMathService
    ) {        
    }
    
    onNetworkChange(network) {
        this.app.loading = true;
        console.log("Account.onNetworkChange() *********************");
        this.scatter.queryAccountData(this.account.account_name).then((account) => {
            this.account = account;
            console.log("Account.onNetworkChange() !!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            this.app.loading = false;
        });
        this.data.amount = "0.0001 " + this.scatter.network.symbol;
    }

    ngOnInit() {
        this.data = {
            to: "gyztqojshage",
            amount: "0.0001 EOS",
            memo: "testing"
        };

        this.account = {
            account_name:"guesst",
            dummie: true
        };
        var name = this.route.snapshot.paramMap.get('name');
        // console.log("name", name);
        if (name) {
            this.account.account_name = name;
            if (this.scatter.account && this.scatter.account.name == name) {
                this.account = this.scatter.account.data;
            }
            this.scatter.onNetworkChange.subscribe(this.onNetworkChange.bind(this));
            setTimeout(() => {
                this.scatter.setNetwork(this.scatter.network.slug, this.scatter.network.index);
            }, 300);            
        } else {
            this.scatter.waitReady.then(() => {
                if (this.scatter.logged) {
                    this.app.navigate("/" + this.scatter.network.slug + "/account/" + this.scatter.account.name);
                }
            });
        }        
    }

    search(account_name) {
        console.log("AccountPage.search()", this.app.state, this.app.path);

        this.app.navigate("/" + this.scatter.network.slug + "/account/" + account_name);
        
        setTimeout(() => {
            this.scatter.setNetwork(this.scatter.network.slug, this.scatter.network.index);
        }, 300);        
    }

    login() {
        this.scatter.login().then(() => {
            if (this.scatter.logged) {
                this.app.navigate("/" + this.scatter.network.slug + "/account/" + this.scatter.account.name);
            }
        });
    }

    transfer() {
        // this.scatter.transfer(this.scatter.account.name, this.data.to, this.data.amount, this.data.memo);        
        this.app.loading = true;
        try {
            this.scatter.getContract("eosio.token").then(contract => {
                contract.transfer({
                    from:  this.scatter.account ? this.scatter.account.name : this.account.account_name,
                    memo: this.data.memo,
                    quantity: this.data.amount,
                    to: this.data.to
                }).then((response => {
                    console.log("response", response);
                    this.last_trx = response.transaction_id;
                    this.app.loading = false;
                })).catch(err => {
                    console.error("err", err);
                    this.app.loading = false;
                });
            }).catch(err => {
                console.error("err", err);
                this.app.loading = false;
            });
        } catch (e) {
            console.error("err", e);
            this.app.loading = false;
        }
    }

}
