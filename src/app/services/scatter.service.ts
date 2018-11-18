import { Injectable } from '@angular/core';
import ScatterJS from 'scatterjs-core';
import ScatterEOS from 'scatterjs-plugin-eosjs'

declare var ScatterJS:any;

export interface ScatterInstance {
    eos?:Function,
    eosHook:any,
    identity: any,
    isExtension: boolean,
    // -----------------
    authenticate: Function,
    connect: Function,
    constructor: Function,
    createTransaction: Function,
    disconnect: Function,
    forgetIdentity: Function,
    getArbitrarySignature: Function,
    getIdentity: Function,
    getIdentityFromPermissions: Function,
    getPublicKey: Function,
    getVersion: Function,
    hasAccountFor: Function,
    isConnected: Function,
    isPaired: Function,
    linkAccount: Function,
    loadPlugin: Function,
    requestSignature: Function,
    requestTransfer: Function,
    suggestNetwork: Function
}

@Injectable()
export class ScatterService {
    
    private lib: ScatterInstance;
    private setReady: Function;
    public waitReady: Promise<any> = new Promise((resolve) => {
        this.setReady = resolve;
    });
    
    constructor() {
        this.initScatter();
    }

    initScatter() {
        console.log("ScatterService.initScatter()");
        ScatterJS.plugins( new ScatterEOS() );
        this.setScatter(ScatterJS.scatter);
    }

    setScatter(_scatter:any) {
        console.log("ScatterService.setScatter()");
        const connectionOptions = {initTimeout:10000}
        _scatter.connect("Cards & Tokens", connectionOptions).then(connected => {
            if(!connected) { return false; }

            (<any>window).ScatterJS = null;
            this.lib = _scatter;
            console.log("ScatterService.setReady()");
            console.log("this.lib", this.lib);
            this.setReady(this.lib);
        });                
    }

    login() {
        console.log("ScatterService.login()");
        return this.waitReady.then(() => {
            return this.lib.getIdentity()
                .then( (identity)  => {
                    console.log("this.lib", this.lib);
                    return identity;
                })
                .catch( err => { console.error(err); });            
        });
    }

    logout() {
        console.log("ScatterService.logout()");
        return this.waitReady.then(() => {
            return this.lib.forgetIdentity()
                .then( (err)  => {
                    console.log("disconnect", err);
                })
                .catch( err => { console.error(err); });            
        });
    }

    get logged(): boolean {
        if (!this.lib) return false;
        return !!this.lib.identity;
    }

    get username(): string {
        if (!this.lib) return "";
        return this.lib.identity ? this.lib.identity.name : "";
    }

}
