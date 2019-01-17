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
export class VapaeeService {

    public contract:string;
    vapaeeauthor:string = "vapaeeauthor";
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
        this.contract = this.vapaeeauthor;
    }

    droptables() {
        console.log("VapaeeService.droptables()");
        return new Promise<any>((resolve, reject) => {
            try {
                this.scatter.getContract(this.vapaeeauthor).then(contract => {
                    try {
                        contract.droptables({}, this.scatter.authorization).then((response => {
                            console.log("response", response);
                            resolve(response);
                        })).catch(err => { console.error(err); reject(err); });
                    } catch (err) { console.error(err); reject(err); }
                }).catch(err => { console.error(err); reject(err); });
            } catch (err) { console.error(err); reject(err); }
        }).catch(err => console.error(err) );
    }

    registerPublisher(owner:string, nick:string, title:string) {
        console.log("VapaeeService.registerPublisher()", owner, nick, title);
        return new Promise<any>((resolve, reject) => {
            try {
                this.scatter.getContract(this.vapaeeauthor).then(contract => {
                    try {
                        contract.newpublisher({
                            owner:  owner,
                            nickstr: nick,
                            title: title
                        }, this.scatter.authorization).then((response => {
                            resolve(response);
                        })).catch(err => { reject(err); });
                    } catch (err) { reject(err); }
                }).catch(err => { reject(err); });
            } catch (err) { reject(err); }
        }).catch(err => console.error(err) );
    }

    registerApp(owner:string, contract:string, nick:string, title:string, inventory:number) {
        console.log("VapaeeService.registerApp()", owner, contract, nick, title, inventory);
        return new Promise<any>((resolve, reject) => {
            try {
                this.scatter.getContract(this.vapaeeauthor).then(smart_contract => {
                    try {
                        smart_contract.newapp({
                            owner:  owner,
                            contract: contract,
                            nickstr: nick,
                            title: title,
                            inventory: inventory
                        }, this.scatter.authorization).then((response => {
                            resolve(response);
                        })).catch(err => { reject(err); });
                    } catch (err) { reject(err); }
                }).catch(err => { reject(err); });
            } catch (err) { reject(err); }
        }).catch(err => console.error(err) );
    }
    

    /*
    registerPublisher(owner:string, nick:string, title:string) {
        console.log("NewAppPage.register()");
        return new Promise<any>((resolve, reject) => {
            try {
                this.scatter.getContract("vapaeeauthor").then(contract => {
                    try {
                        contract.newpublisher({
                            owner:  owner,
                            nickstr: nick,
                            title: title
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
    */    
}
