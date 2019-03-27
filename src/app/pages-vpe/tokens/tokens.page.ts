import { Component, OnInit, OnDestroy } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService } from 'src/app/services/scatter.service';
import { BGBoxService } from 'src/app/services/bgbox.service';
import { CntService } from 'src/app/services/cnt.service';
import { VapaeeService } from 'src/app/services/vapaee.service';
import { Token } from 'src/app/services/utils.service';


@Component({
    selector: 'tokens-page',
    templateUrl: './tokens.page.html',
    styleUrls: ['./tokens.page.scss', '../common.page.scss']
})
export class TokensPage implements OnInit, OnDestroy {

    
   
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        public bgbox: BGBoxService,
        public cnt: CntService,
        public vapaee: VapaeeService
    ) {
    }

    get tokens(): Token[] {
        return this.vapaee.tokens;
    }

    ngOnDestroy() {
    }

    ngOnInit() {


    }
}
