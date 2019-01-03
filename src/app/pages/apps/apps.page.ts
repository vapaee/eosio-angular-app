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
    selector: 'apps-page',
    templateUrl: './apps.page.html',
    styleUrls: ['./apps.page.scss', '../common.page.scss']
})
export class AppsPage implements OnInit {

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
    
    ngOnInit() {
    }


}
