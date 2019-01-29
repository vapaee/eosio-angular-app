import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { ScatterService } from './scatter.service';
import BigNumber from "bignumber.js";
import Eos from 'eosjs';

export interface SlugId {
    low?: string;
    str?: string;
    top?: string;
}
export interface Profile {
    id?:string;
    slugid: SlugId;
    account: string;
}

export interface AuthorsCache {
    [key:string]:Profile;
}

export class Utils {
    code_0:number;
    code_1:number;
    code_4:number;
    code_9:number;
    code_a:number;
    code_f:number;
    code_z:number;
    
    constructor() {
        this.code_0 = "0".charCodeAt(0);
        this.code_1 = "1".charCodeAt(0);
        this.code_4 = "4".charCodeAt(0);
        this.code_9 = "9".charCodeAt(0);
        this.code_a = "a".charCodeAt(0);
        this.code_f = "f".charCodeAt(0);        
        this.code_z = "z".charCodeAt(0);        
    }    

    decodeNibble(nib:number) {
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

    encodeNibble(index:number, bits:number[]) {
        var value = 0;
        value += bits[index + 0] == 1 ? 8 : 0;
        value += bits[index + 1] == 1 ? 4 : 0;
        value += bits[index + 2] == 1 ? 2 : 0;
        value += bits[index + 3] == 1 ? 1 : 0;
        if (0 <= value && value <= 9) {
            return "" + value;
        }
        switch(value) {
            case 10: return "a";
            case 11: return "b";
            case 12: return "c";
            case 13: return "d";
            case 14: return "e";
            case 15: return "f";
        }
        return "?";
    }

    decodeUint64(_num: string) {
        var bits:number[] = [];
        var num:string = _num.substr(2);
        for (var i=0; i<num.length; i++) {
            bits = bits.concat(this.decodeNibble(num.charCodeAt(i)));
        }
        return bits;
    }

    encodeUnit64(bits:number[]) {
        var slugid:SlugId = {top:"0x",low:"0x"};
        var str = "top";
        for (var i=0; i<bits.length; i+=4) {
            if (i>=128) str = "low";
            slugid[str] += this.encodeNibble(i, bits);
        }
        return slugid;
    }

    extractLength(bits:number[]) {
        if(bits.length != 256) console.error("ERROR: extractLength(bits) bits must be an array of 256 bits");
        return bits[250] * 32 + bits[251] * 16 + bits[252] * 8 + bits[253] * 4 + bits[254] * 2 + bits[255] * 1;
    }

    insertLength(bits:number[], length: number) {
        if(bits.length != 256) console.error("ERROR: extractLength(bits) bits must be an array of 256 bits");
        bits[250] = (length & 32) ? 1 : 0;
        bits[251] = (length & 16) ? 1 : 0;
        bits[252] = (length &  8) ? 1 : 0;
        bits[253] = (length &  4) ? 1 : 0;
        bits[254] = (length &  2) ? 1 : 0;
        bits[255] = (length &  1) ? 1 : 0;
    }

    valueToChar(v:number) {
        if (v == 0) return '.';
        if (v == 1) return '-';
        if (v < 6) return String.fromCharCode(v + this.code_0 - 1);
        if (v < 32) return String.fromCharCode(v + this.code_a - 6);
        console.assert(false, "ERROR: value out of range [0-31]", v);
        return '?';                   
    }

    charToValue(c:string) {
        console.assert(c.length == 1, "ERROR: c MUST be a character (string with length == 1). Got", typeof c, c);
        if (c == ".") return 0;
        if (c == "-") return 1;
        if (this.code_0 < c.charCodeAt(0) && c.charCodeAt(0) <= this.code_4) return c.charCodeAt(0) - this.code_1 + 2;
        if (this.code_a <= c.charCodeAt(0) && c.charCodeAt(0) <= this.code_z) return c.charCodeAt(0) - this.code_a + 6;
        console.assert(false, "ERROR: character '" + c + "' is not in allowed character set for slugid ");
        return -1;
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

    insertChar(value:number, j:number, bits:number[]) {
        var encode = 5;
        var index = j * encode;
        bits[index + 0] = (value & 16) > 0 ? 1 : 0;
        bits[index + 1] = (value &  8) > 0 ? 1 : 0;
        bits[index + 2] = (value &  4) > 0 ? 1 : 0;
        bits[index + 3] = (value &  2) > 0 ? 1 : 0;            
        bits[index + 4] = (value &  1) > 0 ? 1 : 0;
    }

    decodeSlug(sluig:SlugId) {
        // decodeSlug() 0x41ae9c04d34873482a78000000000000 0x00000000000000000000000000000010
        // console.log("decodeSlug()", nick.top, nick.low);
        var bits:number[] = [];
        bits = this.decodeUint64(sluig.top).concat(this.decodeUint64(sluig.low));
        var length = bits[250] * 32 + bits[251] * 16 + bits[252] * 8 + bits[253] * 4 + bits[254] * 2 + bits[255] * 1;
        // console.log("length: ", length);
        var str:string = "";
        for (var i=0; i<length; i++) {
            str += this.extractChar(i, bits);
        }
        // console.log("str: ", str);
        sluig.str = str;
        return sluig;
    }

    encodeSlug(name:string):SlugId {
        var bits = [];
        for (var i=0; i<256; i++) {
            bits.push(0);
        }
        for (var i=0; i<name.length; i++) {
            var value = this.charToValue(name[i]);
            this.insertChar(value, i, bits);
        }
        this.insertLength(bits, name.length);
        var slug = this.encodeUnit64(bits);

        slug = this.decodeSlug(slug);
        console.assert(slug.str == name, "ERROR: slug.str: ", slug.str, [slug.str], [name]);

        return slug;
    }

    slugTo128bits(slug:SlugId):string {
        var str = "0x";
        var topbits = this.decodeUint64(slug.top);
        var lowbits = this.decodeUint64(slug.low);
        var mixbits = [];
        for (var i=0; i<topbits.length; i++) {
            mixbits.push(topbits[i] ^ lowbits[i] ? 1 : 0);
        }
        for (var i=0; i<mixbits.length; i+=4) {
            str += this.encodeNibble(i, mixbits);
        }        
        return str;
    }

    encodeName(name:string):BigNumber {
        return new BigNumber(Eos.modules.format.encodeName(name, false));
    }
    
}


@Injectable()
export class BGBoxService {

    authors: AuthorsCache;
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
    
    public utils: Utils;
    constructor(private scatter: ScatterService) {
        this.utils = new Utils();
        this.contract = this.boardgamebox;
        this.authors = {};
    }

    decodeSlug(nick:{top:string,low:string,str?:string}) {
        return this.utils.decodeSlug(nick);
    }

    // auxiliar functions -------------------------------------------

    excecute(action: string, params: any) {
        console.log("BGBoxService.excecute()", action, [params]);
        return new Promise<any>((resolve, reject) => {
            try {
                this.scatter.getContract(this.boardgamebox).then(contract => {
                    try {
                        contract[action](params, this.scatter.authorization).then((response => {
                            console.log("BGBoxService.excecute() ---> ", [response]);
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
            limit?:number, 
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
        console.log("BGBoxService.getAuthors()");

        return Promise.all<any>([
            this.getTable("authors"),
            this.getTable("profiles"),
            this.getTable("apps"),
        ]).then(result => {
            
            var data = {
                map:{},
                authors:result[0].rows,
                profiles:result[1].rows,
                apps:result[2].rows,
            };

            for (var i in data.authors) {
                data.authors[i].slugid = this.decodeSlug(data.authors[i].slugid);
                var key = "id-" + data.authors[i].id
                data.map[key] = Object.assign({}, data.authors[i]);
                data.authors[i] = data.map[key];
            }

            for (var i in data.profiles) {
                Object.assign(data.map["id-" + data.profiles[i].id], data.profiles[i]);
                data.profiles[i] = data.map["id-" + data.profiles[i].id];
            }

            for (var i in data.apps) {
                Object.assign(data.map["id-" + data.apps[i].id], data.apps[i]);
                data.apps[i] = data.map["id-" + data.apps[i].id];
            }

            for (var i in data.authors) {
                var key = "id-" + data.authors[i].id
                this.authors[data.map[key].slugid.str] = data.map[key];
            }

            console.log("getAuthors() ----------> ", result, data);
            return data;
        });

    }

    getAuthorsFor(account: string) {
        console.log("BGBoxService.getAuthorsFor()", account);
        return new Promise<any>((resolve, reject) => {
            // https://eosio.stackexchange.com/questions/813/eosjs-gettablerows-lower-and-upper-bound-on-account-name
            var encodedName = new BigNumber(Eos.modules.format.encodeName(account, false))
            var params = {
                lower_bound: encodedName.toString(), 
                upper_bound: encodedName.plus(1).toString(), 
                limit: 25, 
                key_type: "i64",
                index_position: "2"
            }
            // console.log("---------------------------------------------");
            // console.log("BGBoxService.getAuthorsFor() params ", params);

            this.getTable("authors", params).then(result => {
                // console.log("BGBoxService.getAuthorsFor() --> ", result.rows);
                var data = {map:{},authors:result.rows,profiles:[]};
                var promises:Promise<any>[] = [];
                for (var i in result.rows) {
                    var author = result.rows[i];
                    author.slugid = this.decodeSlug(author.slugid);

                    data.map["id-"+author.id] = author;
                    var id_p1 = new BigNumber(author.id).plus(1);
                    var i_params = {lower_bound:author.id ,upper_bound: author.id};
                    var promise = this.getTable("profiles", i_params).then(i_result => {
                        // console.log("BGBoxService.getAuthorsFor() --> ", i_result.rows);
                        if (i_result.rows.length == 1) {
                            var profile = i_result.rows[0];
                            var key = "id-"+profile.id;
                            data.map[key] = Object.assign(
                                data.map[key],
                                profile
                            );
                            data.profiles.push(data.map[key]);
                            this.authors[data.map[key].slugid.str] = data.map[key];
                        }
                    });
                    promises.push(promise);
                }

                Promise.all(promises).then(() => {
                    console.log("BGBoxService.getAuthorsFor() [", account, "] --> ",data);
                    // console.log("data: ", data);
                    // console.log("---------------------------------------------");
                    resolve(data);
                }).catch(err => {
                    console.error("ERROR: ", err);
                    reject(err);
                })
                
            });
        });
    }

    getAuthorBySlug(slugid: string) {
        console.log("BGBoxService.getAuthorBySlug()", slugid);
        if (this.authors[slugid]) return Promise.resolve(this.authors[slugid]);

        return new Promise<any>((resolve, reject) => {
            var author = null;
            
            var slug: SlugId = this.utils.encodeSlug(slugid);
            console.log("---> ", [slug]);
            var big = this.utils.slugTo128bits(slug);
            console.log("---> ", [big.toString()]);
            
            var params = {
                lower_bound: big, 
                upper_bound: big, 
                limit: 1, 
                key_type: "i128",
                index_position: "3"
            }

            this.getTable("authors", params).then(result => {
                // console.log("BGBoxService.getAuthorsFor() --> ", result.rows);
                var data = {map:{},authors:result.rows,profiles:[]};
                
                var promises:Promise<any>[] = [];
                for (var i in result.rows) {
                    var author = result.rows[i];
                    author.slugid = this.decodeSlug(author.slugid);

                    data.map["id-"+author.id] = author;
                    var id_p1 = new BigNumber(author.id).plus(1);
                    var i_params = {lower_bound:author.id ,upper_bound: author.id};
                    var promise = this.getTable("profiles", i_params).then(i_result => {
                        // console.log("BGBoxService.getAuthorsFor() --> ", i_result.rows);
                        
                        if (i_result.rows.length == 1) {
                            var profile = i_result.rows[0];
                            data.map["id-"+profile.id] = Object.assign(
                                data.map["id-"+profile.id],
                                profile
                            );
                            data.profiles.push(data.map["id-"+profile.id]);                                
                        }
                    });
                    promises.push(promise);
                }

                Promise.all(promises).then(() => {
                    // console.log("data: ", data);
                    console.log("---------------------------------------------", data.authors[0]);
                    resolve(data.authors[0]);
                }).catch(err => {
                    console.error("ERROR: ", err);
                    reject(err);
                })
                
            });

            
        });
    }    

    registerProfile(owner:string, slugid:string, name:string) {
        console.log("BGBoxService.registerProfile()", owner, slugid, name);
        return this.excecute("newprofile", {owner:owner, slugid:slugid, name:name})
    }

    registerApp(owner:string, contract:string, slugid:string, title:string, inventory:number) {
        console.log("BGBoxService.registerApp()", owner, contract, slugid, title, inventory);
        return this.excecute("newapp", {
            owner:  owner,
            contract: contract,
            slugid: slugid,
            title: title,
            inventory: inventory
        });
    }

    signUpProfileForApp(owner, profileid, appid, rampayer) {
        console.log("BGBoxService.signUpProfileForApp()", owner, profileid, appid, rampayer);
        return this.excecute("profile4app", {owner:owner, profile: profileid, app:appid, ram_payer:rampayer})
    }

    // -------------------------
    droptables() {
        console.log("BGBoxService.droptables()");
        return this.excecute("droptables", {});
    }
    

    
}
