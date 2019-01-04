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
    code_0:number;
    code_9:number;
    code_a:number;
    code_f:number;
    
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        private route: ActivatedRoute,
        public tokenMath: EosioTokenMathService
    ) {
        this.apps = [];
        this.code_0 = "0".charCodeAt(0);
        this.code_9 = "9".charCodeAt(0);
        this.code_a = "a".charCodeAt(0);
        this.code_f = "f".charCodeAt(0);

        console.log(this.code_0, this.code_9, this.code_a, this.code_f);
    }

    decodeNibble(nib, char) {
        var nibble = [0,0,0,0];
        var value = 0;
        if (this.code_0 <= nib && nib <= this.code_9) {
            value = nib - this.code_0;
        } else if (this.code_a <= nib && nib <= this.code_f) {
            value = nib - this.code_a + 10;
        }


        console.log("decodeNibble()", nib, char, value);

        return ;
    }

    decodeUint64(_num: string) {
        var num:string = _num.substr(2);
        console.log("num: ", num);
        for (var i=0; i<num.length; i++) {
            this.decodeNibble(num.charCodeAt(i), num[i]);
        }
    }

    decodificarSlug(nick:{top:string,low:string,str?:string}) {
        // decodificarSlug() 0x41ae9c04d34873482a78000000000000 0x00000000000000000000000000000010
        console.log("decodificarSlug()", nick.top, nick.low);
        // var top_array:string[] = nick.top.split(""); top_array.shift(); top_array.shift();
        // var low_array:string[] = nick.low.split(""); low_array.shift(); low_array.shift();
        var top = this.decodeUint64(nick.top);




        nick.str = nick.top + nick.low;
        return nick;
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
            for( var j = 0; j < result.rows.length; j++) {
                result.rows[j].nick = this.decodificarSlug(result.rows[j].nick);
            };
            this.apps = result.rows;
        });
    }
}
