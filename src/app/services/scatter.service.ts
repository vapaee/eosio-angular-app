import { Injectable } from '@angular/core';
import ScatterJS from 'scatterjs-core';
import ScatterEOS from 'scatterjs-plugin-eosjs'
import Eos from 'eosjs';

declare var ScatterJS:any;

export interface EOS {
    getInfo:Function,
    getAccount:Function,
    getCode:Function,
    getCodeHash:Function,
    getAbi:Function,
    getRawCodeAndAbi:Function,
    abiJsonToBin:Function,
    abiBinToJson:Function,
    getRequiredKeys:Function,
    getBlock:Function,
    getBlockHeaderState:Function,
    getTableRows:Function,
    getCurrencyBalance:Function,
    getCurrencyStats:Function,
    getProducers:Function,
    getProducerSchedule:Function,
    getScheduledTransactions:Function,
    pushBlock:Function,
    pushTransaction:Function,
    pushTransactions:Function,
    getActions:Function,
    getTransaction:Function,
    getKeyAccounts:Function,
    getControlledAccounts:Function,
    createTransaction:Function,
    transaction:Function,
    nonce:Function,
    transfer:Function,
    create:Function,
    issue:Function,
    bidname:Function,
    newaccount:Function,
    setcode:Function,
    setabi:Function,
    updateauth:Function,
    deleteauth:Function,
    linkauth:Function,
    unlinkauth:Function,
    canceldelay:Function,
    onerror:Function,
    buyrambytes:Function,
    sellram:Function,
    buyram:Function,
    delegatebw:Function,
    undelegatebw:Function,
    refund:Function,
    regproducer:Function,
    unregprod:Function,
    setram:Function,
    regproxy:Function,
    voteproducer:Function,
    claimrewards:Function,
    setpriv:Function,
    rmvproducer:Function,
    setalimits:Function,
    setglimits:Function,
    setprods:Function,
    reqauth:Function,
    setparams:Function,
    contract:Function
}

export interface Scatter {
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

export interface AccountData {
    account_name?: string,
    head_block_num?: number,
    head_block_time?: string,
    privileged?: false,
    last_code_update?: string,
    created?: string,
    core_liquid_balance?: string,
    ram_quota?: number,
    net_weight?: number,
    cpu_weight?: number,
    net_limit?: {
        used?: number,
        available?: number,
        max?: number
    },
    cpu_limit?: {
        used?: number,
        available?: number,
        max?: number
    },
    ram_usage?: number,
    permissions?: {
        perm_name?: string,
        parent?: string,
        required_auth?: {
            threshold?: 1,
            keys?: {
                key?: string,
                weight?: 1
            }[],
            accounts?: any[],
            waits?: any[]
        }
    }[],
    total_resources?: {
        owner?: string,
        net_weight?: string,
        cpu_weight?: string,
        ram_bytes?: number
    },
    self_delegated_bandwidth?: {
        from?: string,
        to?: string,
        net_weight?: string,
        cpu_weight?: string
    },
    refund_request?: {
        owner?: string,
        request_time?: string,
        net_amount?: string,
        cpu_amount?: string
    },
    voter_info?: {
        owner?: string,
        proxy?: string,
        producers?: string[],
        staked?: number,
        last_vote_weight?: string,
        proxied_vote_weight?: string,
        is_proxy?: number
    },
    returnedFields?: null
}

export interface Account {
    authority: string,
    blockchain: string,
    name: string,
    publicKey: string,
    data: AccountData
}

export interface Network {
    blockchain:string,
    protocol:string,
    host:string,
    port:number,
    chainId:string
}

@Injectable()
export class ScatterService {
    
    public error: string;
    private lib: Scatter;
    private network: Network;
    private eos: EOS;
    public account: Account;
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
    
    constructor() {
        var eosnet:Network = {
            blockchain:'eos',
            protocol:'https',
            host:'nodes.get-scatter.com',
            port:443,
            chainId:'aca376f206b8fc25a6ed44dbdc66547c36c6c33e3a119ffbeaef943642f0e906'
        }
        var telostest:Network = {
            blockchain:'eos',
            protocol:'http',
            host:'173.255.220.117',
            port:3888,
            chainId:'335e60379729c982a6f04adeaad166234f7bf5bf1191252b8941783559aec33e'
        }
        this.network = eosnet;
        this.initScatter();
    }

    initScatter() {
        console.log("ScatterService.initScatter()");
        ScatterJS.plugins( new ScatterEOS() );
        
        console.log("ScatterService.setScatter()");
        this.error = "";
        const connectionOptions = {initTimeout:10000}
        this.lib = ScatterJS.scatter;
        this.eos = this.lib.eos(this.network, Eos, { expireInSeconds:60 });
        this.setEosjs(this.eos);
        this.lib.connect("Cards & Tokens", connectionOptions).then(connected => {
            if(!connected) {
                this.error = "ERROR: can not connect to Scatter. Is it up and running?";
                console.error(this.error);
                return false;
            }

            (<any>window).ScatterJS = null;
            // Get a proxy reference to eosjs which you can use to sign transactions with a user's Scatter.
            console.log("ScatterService.setConnected()");
            this.setConnected();
            if (this.logged) {
                this.login().then(() => this.setReady());
            } else {
                console.log("ScatterService.setReady()");
                this.setReady();
            }
        });        
    }


    private setIdentity(identity:any) {
        console.log("ScatterService.setIdentity()");
        console.assert(!!this.lib, "ERROR: no instance of scatter found");
        this.error = "";
        this.lib.identity = identity;
        this.account = this.lib.identity.accounts.find(x => x.blockchain === 'eos');
        this.queryAccountData(this.account.name).then(account => {
            this.account.data = account;
            console.log("this.account: " , [this.account]);
        });
    }

    queryAccountData(name:string): Promise<AccountData> {
        return this.waitEosjs.then(() => {
            /*
            // get_table_rows
            code: "eosio"
            index_position: 1
            json: true
            key_type: "i64"
            limit: -1
            lower_bound: null
            scope: "gqydoobuhege"
            table: "delband"
            table_key: ""
            */
            return this.eos.getAccount({account_name: name});
        }).catch(error => console.error(error));
        
    }


    login() {
        console.log("ScatterService.login()");
        return this.waitConnected.then(() => {
            return this.lib.getIdentity({"accounts":[this.network]})
                .then( (identity)  => {
                    this.setIdentity(identity);
                    return identity;
                })
                .catch( err => { console.error(err); });            
        });
    }

    logout() {
        console.log("ScatterService.logout()");
        return this.waitConnected.then(() => {
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
