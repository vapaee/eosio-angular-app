import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { ScatterService } from './scatter.service';


export class SlugUtils {
    code_0:number;
    code_9:number;
    code_a:number;
    code_f:number;
    
    constructor() {
        this.code_0 = "0".charCodeAt(0);
        this.code_9 = "9".charCodeAt(0);
        this.code_a = "a".charCodeAt(0);
        this.code_f = "f".charCodeAt(0);        
    }    

    decodeNibble(nib) {
        var nibble = [0,0,0,0];
        var value = 0;
        if (this.code_0 <= nib && nib <= this.code_9) {
            value = nib - this.code_0;
        } else if (this.code_a <= nib && nib <= this.code_f) {
            value = nib - this.code_a + 10;
        }
        nibble[0] = (value & 8) > 0 ? 1 : 0;
        nibble[1] = (value & 4) > 0 ? 1 : 0;
        nibble[2] = (value & 2) > 0 ? 1 : 0;
        nibble[3] = (value & 1) > 0 ? 1 : 0;
        return nibble;
    }

    decodeUint64(_num: string) {
        var bits:number[] = [];
        var num:string = _num.substr(2);
        for (var i=0; i<num.length; i++) {
            bits = bits.concat(this.decodeNibble(num.charCodeAt(i)));
        }
        return bits;
    }

    extractLength(bits:number[]) {
        if(bits.length != 256) console.error("ERROR: extractLength(bits) bits must be an array of 256 bits");
        return bits[250] * 32 + bits[251] * 16 + bits[252] * 8 + bits[253] * 4 + bits[254] * 2 + bits[255] * 1;
    }

    valueToChar(v:number) {
        if (v == 0) return '.';
        if (v == 1) return '-';
        if (v < 6) return String.fromCharCode(v + this.code_0 - 1);
        if (v < 32) return String.fromCharCode(v + this.code_a - 6);
        console.assert(false, "ERROR: value out of range [0-31]", v);
        return '?';                   
    }

    extractChar(c:number, bits:number[]) {
        var encode = 5;
        var pot = Math.pow(2, encode-1); // 16
        var value = 0;
        var index = c * encode;
        for (var i=0; i<encode; i++, pot = pot/2) {
            value += bits[index + i] * pot;
        }
        var char = this.valueToChar(value);
        return char;
    }

    decodeSlug(nick:{top:string,low:string,str?:string}) {
        // decodeSlug() 0x41ae9c04d34873482a78000000000000 0x00000000000000000000000000000010
        console.log("decodeSlug()", nick.top, nick.low);
        var bits:number[] = [];
        bits = this.decodeUint64(nick.top).concat(this.decodeUint64(nick.low));
        var length = bits[250] * 32 + bits[251] * 16 + bits[252] * 8 + bits[253] * 4 + bits[254] * 2 + bits[255] * 1;
        console.log("length: ", length);
        var str:string = "";
        for (var i=0; i<length; i++) {
            str += this.extractChar(i, bits);
        }
        console.log("str: ", str);
        nick.str = str;
        return nick;
    }
}


@Injectable()
export class BGBoxService {

    public contract:string;
    boardgamebox:string = "boardgamebox";
    vapaeetokens:string = "vapaeetokens";

    public onEvent:Subject<any> = new Subject();
    private setReady: Function;
    public waitReady: Promise<any> = new Promise((resolve) => {
        this.setReady = resolve;
    });
    private setConnected: Function;
    public waitConnected: Promise<any> = new Promise((resolve) => {
        this.setConnected = resolve;
    });
    private setEosjs: Function;
    public waitEosjs: Promise<any> = new Promise((resolve) => {
        this.setEosjs = resolve;
    });
    private setEndpoints: Function;
    public waitEndpoints: Promise<any> = new Promise((resolve) => {
        this.setEndpoints = resolve;
    });    
    
    private slug: SlugUtils;
    constructor(private scatter: ScatterService) {
        this.slug = new SlugUtils();
        this.contract = this.boardgamebox;
    }

    decodeSlug(nick:{top:string,low:string,str?:string}) {
        return this.slug.decodeSlug(nick);
    }

    // auxiliar functions -------------------------------------------

    excecute(action: string, params: any) {
        console.log("BGBoxService.excecute()", action, [params]);
        return new Promise<any>((resolve, reject) => {
            try {
                this.scatter.getContract(this.boardgamebox).then(contract => {
                    try {
                        contract[action](params, this.scatter.authorization).then((response => {
                            resolve(response);
                        })).catch(err => { reject(err); });
                    } catch (err) { reject(err); }
                }).catch(err => { reject(err); });
            } catch (err) { reject(err); }
        }).catch(err => console.error(err) );
    }

    getTable(table:string, params:{
            contract?:string, 
            scope?:string, 
            table_key?:string, 
            lower_bound?:string, 
            upper_bound?:string, 
            limit?:string, 
            key_type?:string, 
            index_position?:string
        } = {}) {

        var _p = Object.assign({
            contract: this.contract, 
            scope: this.contract, 
            table_key: "0", 
            lower_bound: "0", 
            upper_bound: "-1", 
            limit: 25, 
            key_type: "i64", 
            index_position: "1"
        }, params);

        return this.scatter.getTableRows(
            _p.contract,
            _p.scope, table,
            _p.table_key,
            _p.lower_bound,
            _p.upper_bound,
            _p.limit,
            _p.key_type,
            _p.index_position
        );
    }
    // --------------------------------------------------------------
    // API
    getAuthors() {
        var table = "authors";

        return Promise.all<any>([
            this.getTable("authors"),
            this.getTable("users"),
            this.getTable("apps"),
        ]).then(result => {
            
            var data = {
                map:{},
                authors:result[0].rows,
                publishers:result[1].rows,
                apps:result[2].rows,
            };

            for (var i in data.authors) {
                data.authors[i].nick = this.decodeSlug(data.authors[i].nick);
                data.map["id-" + data.authors[i].id] = Object.assign({}, data.authors[i]);
                data.authors[i] = data.map["id-" + data.authors[i].id];
            }

            for (var i in data.publishers) {
                Object.assign(data.map["id-" + data.publishers[i].id], data.publishers[i]);
                data.publishers[i] = data.map["id-" + data.publishers[i].id];
            }

            for (var i in data.apps) {
                Object.assign(data.map["id-" + data.apps[i].id], data.apps[i]);
                data.apps[i] = data.map["id-" + data.apps[i].id];
            }

            console.log("getAuthors() ----------> ", result, data);
            return data;
        });
/*        
        this.scatter.getTableRows(contract, scope, table, table_key, lower_bound, upper_bound, limit, key_type, index_position).then((result) => {
            this.apps = [];
            this.publishers = [];
            for( var j = 0, row = null; j < result.rows.length; j++) {
                row = result.rows[j];
                row.nick = this.bgbox.decodeSlug(row.nick);
                if (row.contract) {
                    console.log("row.contract: ", row.contract);
                    this.apps.push(row);
                } else {
                    this.publishers.push(row);
                }
            };
            this.loading = false;
        });
*/        
    }

    registerPublisher(owner:string, nick:string, name:string) {
        console.log("BGBoxService.registerPublisher()", owner, nick, name);
        return this.excecute("newpublisher", {owner:owner, nickstr:nick, name:name})
    }

    registerApp(owner:string, contract:string, nick:string, title:string, inventory:number) {
        console.log("BGBoxService.registerApp()", owner, contract, nick, title, inventory);
        return this.excecute("newapp", {
            owner:  owner,
            contract: contract,
            nickstr: nick,
            title: title,
            inventory: inventory
        });
    }        

    // -------------------------
    droptables() {
        console.log("BGBoxService.droptables()");
        return this.excecute("droptables", {});
    }
    

    
}
