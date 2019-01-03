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
    selector: 'bgbox-page',
    templateUrl: './bgbox.page.html',
    styleUrls: ['./bgbox.page.scss', '../common.page.scss']
})
export class BGBoxPage implements OnInit {

    apps: any[];
    
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        private route: ActivatedRoute,
        public tokenMath: EosioTokenMathService
    ) {
        this.apps = [];
    }
    
    ngOnInit() {
        var contract = "vapaeeauthor";
        var scope = "vapaeeauthor";
        var table = "authors";
        var table_key = "0";
        var lower_bound = "0";
        var upper_bound = "-1";
        var limit = 10;
        var key_type = "i64"
        var index_position = "1";
        this.scatter.getTableRows(contract, scope, table, table_key, lower_bound, upper_bound, limit, key_type, index_position).then((result) => {
            console.log("AAAAAAAAAAAAA this.scatter.getTableRows--> ", result);
            for( var j = 0; j < result.rows.length; j++) {
                console.log("result.rows[j]", result.rows[j]);
            };
            this.apps = result.rows;
        })
    }

    /*
    
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
    */


}
