import { Component, OnInit } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService, AccountData } from 'src/app/services/scatter.service';
import { ActivatedRoute } from '@angular/router';

import { EosioTokenMathService } from 'src/app/services/eosio.token-math.service';
import { BGBoxService } from 'src/app/services/bgbox.service';
import { VapaeeService } from 'src/app/services/vapaee.service';

@Component({
    selector: 'bgbox-page',
    templateUrl: './bgbox.page.html',
    styleUrls: ['./bgbox.page.scss', '../common.page.scss']
})
export class BGBoxPage implements OnInit {

    apps: any[];
    publishers: any[];
    loading: boolean;
    
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        private route: ActivatedRoute,
        public tokenMath: EosioTokenMathService,
        public bgbox: BGBoxService,
        public vapaee: VapaeeService
    ) {
        this.apps = [];
        this.publishers = [];
    }

    ngOnInit() {
        this.updateAuthors();
    }

    updateAuthors() {
        var contract = this.vapaee.contract;
        var scope = contract;
        var table = "authors";
        var table_key = "0";
        var lower_bound = "0";
        var upper_bound = "-1";
        var limit = 10;
        var key_type = "i64"
        var index_position = "1";
        this.loading = true;
        this.apps = [];
        this.publishers = [];
        this.scatter.getTableRows(contract, scope, table, table_key, lower_bound, upper_bound, limit, key_type, index_position).then((result) => {
            for( var j = 0, row = null; j < result.rows.length; j++) {
                row = result.rows[j];
                row.nick = this.bgbox.decodeSlug(row.nick);
                if (row.contract) {
                    this.apps.push(row);
                } else {
                    this.publishers.push(row);
                }
            };
            this.loading = false;
        });        
    }

    droptablesAuthors() {
        console.log("BGBoxPage.droptablesAuthors()");
        this.app.loading = true;
        this.vapaee.droptables().then(() => {
            this.app.loading = false;
            this.updateAuthors();
        }).catch(() => {
            this.app.loading = false;
        });;
    }

    droptablesBgbox() {
        console.log("BGBoxPage.droptablesBgbox()");
        this.app.loading = true;
        this.bgbox.droptables().then(() => {
            this.app.loading = false;
            this.updateAuthors();
        }).catch(() => {
            this.app.loading = false;
        });;
    }
}
