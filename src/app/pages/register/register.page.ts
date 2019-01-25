import { Component, OnInit } from '@angular/core';
import { AppService } from 'src/app/services/common/app.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { ScatterService, AccountData } from 'src/app/services/scatter.service';
import { ActivatedRoute } from '@angular/router';

import { EosioAccountComponent } from 'src/app/components/eosio-account/eosio-account.component';
import { IdenticonComponent } from 'src/app/components/identicon/identicon.component';
import { EosioTokenMathService } from 'src/app/services/eosio.token-math.service';
import { Observable, Subject } from 'rxjs';
import { BGBoxService } from 'src/app/services/bgbox.service';

@Component({
    selector: 'register-page',
    templateUrl: './register.page.html',
    styleUrls: ['./register.page.scss', '../common.page.scss']
})
export class RegisterPage implements OnInit {

    data: any;
    last_trx:string;
    
    constructor(
        public app: AppService,
        public local: LocalStringsService,
        public scatter: ScatterService,
        private route: ActivatedRoute,
        public tokenMath: EosioTokenMathService,
        private bgbox: BGBoxService
    ) {
        this.data = {};
        this.data.owner = "unknown";
        this.data.nick = "unknown";
        this.data.title = "";
    }
    
    ngOnInit() {
        this.scatter.waitReady.then(() => {
            var owner = this.scatter.account ? this.scatter.account.name : "unknown";
            this.data = {
                owner: owner,
                isapp: false,
                nick: owner,
                title: this.scatter.username,
                contract: this.bgbox.contract,
                inventory: -1,
            };
            // ---------------------------------------
            this.data = {
                owner: owner,
                isapp: true,
                nick: "cards-and-tokens",
                title: "Cards & Tokens",
                contract: "cardsntokens",
                inventory: 8,
            };
            // ---------------------------------------
        });
    }

    setPublisher() {
        this.data.isapp = false;
        this.data.contract = this.bgbox.contract;
        this.data.inventory =  -1;
    }

    setApp() {
        this.data.isapp = true;
        this.data.contract = (this.data.contract == this.bgbox.contract) ? "" : this.data.contract;        
        this.data.inventory = this.data.inventory < 0 ? 8 : this.data.inventory;
    }

    register() {
        return new Promise<any>((resolve, reject) => {
            if (this.data.isapp) {
                this.bgbox.registerApp(this.data.owner, this.data.contract, this.data.nick, this.data.title, this.data.inventory).then((result) => {
                    this.app.loading = false;
                    if (result.transaction_id) {
                        console.log("OK ??", [result]);
                        resolve(result.transaction_id);
                    } else {
                        console.log("NOT OK ??", [result]);
                        reject(result);
                    }
                }).catch((err) => {
                    console.log("NOT OK", [err]);
                    reject(err);
                    this.app.loading = false;
                });
            } else {
                this.bgbox.registerPublisher(this.data.owner, this.data.nick, this.data.title).then((result) => {
                    this.app.loading = false;
                    if (result.transaction_id) {
                        console.log("OK ??", [result]);
                        resolve(result.transaction_id);
                    } else {
                        console.log("NOT OK ??", [result]);
                        reject(result);
                    }
                }).catch((err) => {
                    console.log("NOT OK", [err]);
                    reject(err);
                    this.app.loading = false;
                });
            }                
        });
    }





    consultar_addressbook() {
        var contract = "addressbook";
        var scope = "addressbook";
        var table = "people";
        var table_key = "0";
        var lower_bound = "0";
        var upper_bound = "-1";
        var limit = 10;
        var key_type = "i64"
        var index_position = "1";
        return new Promise<any>(resolve => {
            this.scatter.getTableRows(contract, scope, table, table_key, lower_bound, upper_bound, limit, key_type, index_position).then((result) => {
                console.log("-----------", contract, "-----------");
                for( var j = 0, row = null; j < result.rows.length; j++) {
                    console.log(result.rows[j]);
                };
                resolve();
            });    
        });       
    }

    consultar_abcounter() {
        var contract = "abcounter";
        var scope = "abcounter";
        var table = "counts";
        var table_key = "0";
        var lower_bound = "0";
        var upper_bound = "-1";
        var limit = 10;
        var key_type = "i64"
        var index_position = "1";
        return new Promise<any>(resolve => {
            this.scatter.getTableRows(contract, scope, table, table_key, lower_bound, upper_bound, limit, key_type, index_position).then((result) => {
                console.log("-----------", contract, "-----------");
                for( var j = 0, row = null; j < result.rows.length; j++) {
                    console.log(result.rows[j]);
                };
                resolve();
            });    
        });
    }

    consultar_tablas() {
        this.consultar_addressbook().then(this.consultar_abcounter.bind(this));
    }

    prueba(){
        console.log("prueba()");
        return new Promise<any>((resolve, reject) => {
            try {
                this.scatter.getContract("addressbook").then(smart_contract => {
                    try {
                        smart_contract.upsert({
                            user:  this.scatter.account.name,
                            first_name: "first_name",
                            last_name: "last_name",
                            street: "street",
                            city: "city",
                            state: "state",
                            age: 1
                        }, this.scatter.authorization).then((response => {
                            console.log("response", response);
                            resolve(response);
                        })).catch(err => {
                            console.error(err);
                            reject(err);
                        });
                    } catch (err) {
                        console.error(err);
                        reject(err);
                    }
                }).catch(err => {
                    console.error(err);
                    reject(err);
                });
            } catch (err) {
                console.error(err);
                reject(err);
            }
        });
    }

}
