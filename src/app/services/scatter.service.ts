import { Injectable } from '@angular/core';
import ScatterJS from 'scatterjs-core';
import ScatterEOS from 'scatterjs-plugin-eosjs'
import Eos from 'eosjs';
import { Subject } from 'rxjs';
import { HttpClient } from '@angular/common/http';

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
    dummie?: boolean,
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

export interface Endpoint {
    protocol:string,
    host:string,
    port:number
}

export interface Eosconf {
    blockchain:string,
    protocol:string,
    host:string,
    port:number,
    chainId:string
}

export interface Network {
    slug?: string,
    index?: number,
    eosconf?: Eosconf,
    explorer?: string,
    symbol: string,
    name: string,
    chainId:string,
    endpoints: Endpoint[]
}

export interface ScatterJS {
    plugins?:any,
    scatter?:any
}

@Injectable()
export class ScatterService {
    
    public error: string;
    private lib: Scatter;
    private ScatterJS: ScatterJS;
    private _network: Network;
    private _networks: {[key:string]:Network};
    private _networks_slugs: string[];
    private _account_queries: {[key:string]:Promise<AccountData>};
    private eos: EOS;
    public onNetworkChange:Subject<Network> = new Subject();
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
    private setEndpoints: Function;
    public waitEndpoints: Promise<any> = new Promise((resolve) => {
        this.setEndpoints = resolve;
    });    
    
    constructor(private http: HttpClient) {
        this._networks_slugs = [];
        this._networks = {};
        this._network = {
            "name": "EOS MainNet",
            "symbol": "EOS",
            "explorer": "https://www.bloks.io",
            "chainId":"aca376f206b8fc25a6ed44dbdc66547c36c6c33e3a119ffbeaef943642f0e906",
            "endpoints": [{
                "protocol":"https",
                "host":"nodes.get-scatter.com",
                "port":443
            }]
        };
        // console.error("scatter interrumpido --------------------------------");
        this.http.get<any>("assets/endpoints.json").toPromise().then((response) => {
            this._networks = response;
            for (var i in this._networks) {
                this._networks_slugs.push(i);
            }
            this.setEndpoints();
        });
        this._account_queries = {};
    }

    setNetwork(name:string, index: number = 0) {
        console.log("setNetwork("+name+","+index+")");
        return this.waitEndpoints.then(() => {
            var n = this.getNetwork(name, index);
            if (n) {
                this._network = n;
                this.resetIdentity();
                this.initScatter();
                this.onNetworkChange.next(this.getNetwork(name));
            } else {
                console.error("ERROR: Scatter.setNetwork() unknown network name-index. Got ("
                    + name + ", " + index + "). Availables are:", this._networks);
                console.error("Falling back to eos mainnet");
                return this.setNetwork("eos");
            }    
        });
    }

    getNetwork(slug:string, index: number = 0): Network {
        if (this._networks[slug]) {
            if (this._networks[slug].endpoints.length > index && index >= 0) {
                var network: Network = this._networks[slug];
                var endpoint:Endpoint = network.endpoints[index];
                network.slug = slug;
                network.index = index;
                network.eosconf = {
                    blockchain: "eos",
                    chainId: network.chainId,
                    host: endpoint.host,
                    port: endpoint.port,
                    protocol: endpoint.protocol,
                }                
                return network;
            } else {
                console.error("ERROR: Scatter.getNetwork() index out of range. Got "
                    + index + " expected number between [0.."+this._networks[slug].endpoints.length+"]", );
            }            
        } else {
            console.error("ERROR: Scatter.getNetwork() unknown network slug. Got "
                + slug + " expected one of", this.networks);
        }
        return null;
    }

    get networks(){
        return this._networks_slugs;
    }

    get network(): Network {
        return this._network;
    }

    resetIdentity() {
        this.error = "";
        // this.eos = null;
        if (this.lib) this.lib.identity = null;
    }

    private resetPromises() {
        this.waitEosjs.then(r => {
            this.waitEosjs = null;
            var p = new Promise((resolve) => {
                if (this.waitEosjs) return;
                this.waitEosjs = p;
                this.setEosjs = resolve;
                this.resetPromises();
            });
        });
        this.waitConnected.then(r => {
            this.waitConnected = null;
            var p = new Promise((resolve) => {
                if (this.waitConnected) return;
                this.waitConnected = p;
                this.setConnected = resolve;
                this.resetPromises();
            });
        });
        this.waitReady.then(r => {
            this.waitReady = null;
            var p = new Promise((resolve) => {
                if (this.waitReady) return;
                this.waitReady = p;
                this.setReady = resolve;
                this.resetPromises();
            });
        });
    }

    initScatter() {
        console.log("ScatterService.initScatter()");
        this.error = "";
        const connectionOptions = {initTimeout:10000}
        if ((<any>window).ScatterJS) {
            this.ScatterJS = (<any>window).ScatterJS;
            this.lib = this.ScatterJS.scatter;  
            this.ScatterJS.plugins( new ScatterEOS() );
            (<any>window).ScatterJS = null;
        }
        console.log("EOSJS()",[this.network.eosconf]);
        this.eos = this.lib.eos(this.network.eosconf, Eos, { expireInSeconds:60 });
        console.log("this.eos <<--------");
        this.setEosjs(this.eos);
        this.lib.connect("Cards & Tokens", connectionOptions).then(connected => {
            console.log("this.lib.connect()", connected);
            if(!connected) {
                this.error = "ERROR: can not connect to Scatter. Is it up and running?";
                console.error(this.error);
                return false;
            }
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
        console.log("ScatterService.setIdentity()", identity);
        console.assert(!!this.lib, "ERROR: no instance of scatter found");
        this.error = "";
        this.lib.identity = identity;
        this.account = this.lib.identity.accounts.find(x => x.blockchain === 'eos');
        console.log("ScatterService.setIdentity() -> ScatterService.queryAccountData() : " , [this.account.name]);
        this.queryAccountData(this.account.name).then(account => {
            this.account.data = account;
            console.log("this.account: " , [this.account]);
        });
    }

    
    queryAccountData(name:string): Promise<AccountData> {
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
        console.log("ScatterService.queryAccountData("+name+") ");
        this._account_queries[name] = this._account_queries[name] || new Promise<AccountData>((resolve, reject) => {
            console.log("PASO 1 ------", [this._account_queries])
            this.waitEosjs.then(() => {
                console.log("PASO 2 (eosjs) ------");
                this.eos.getAccount({account_name: name}).then((response) => {
                    console.log("PASO 3 (eosjs.getAccount) ------", response);
                    resolve(response);
                }).catch((err) => {
                    reject(err);
                });
            }).catch((error) => {
                console.error(error);
                reject(error);
            });
        });

        var promise = this._account_queries[name];
        promise.then((r) => {
            setTimeout(() => {
                delete this._account_queries[r.account_name];
            });
        });
        
        return promise;
    }

    getContract(account_name): Promise<any> {
        return new Promise((resolve, reject) => {
            this.waitEosjs.then(() => {
                this.eos.contract(account_name).then(contract => {
                    console.log("contract -> ", contract);
                    for (var i in contract) {
                        if(typeof contract[i] == "function") console.log("contract."+i+"()", [contract[i]]);
                    }
                    resolve(contract);
                }).catch(error => {
                    console.error(error);
                }); 
            }).catch((error) => {
                console.error(error);
                reject(error);
            });   
        }); 
    }

    transfer(from:string, to:string, amount:string, memo:string) {
        return new Promise((resolve, reject) => {
            this.waitEosjs.then(() => {
                const transactionOptions = { authorization:[`${this.account.name}@${this.account.authority}`] };
                console.log("Scatter.transfer():", from, to, amount, memo, transactionOptions);
                this.eos.transfer(from, to, amount, memo, transactionOptions).then(trx => {
                    // That's it!
                    console.log(`Transaction ID: ${trx.transaction_id}`, trx);
                    // en Notas está el json que describe el objeto trx
                    resolve(trx);
                }).catch(error => {
                    console.error(error);
                }); 
            }).catch((error) => {
                console.error(error);
                reject(error);
            });   
        });
    }


    login() {
        console.log("ScatterService.login()");
        return this.waitConnected.then(() => {
            return this.lib.getIdentity({"accounts":[this.network.eosconf]})
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
                    this.resetIdentity();
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

    getTableRows(contract, scope, table, tkey, lowerb, upperb, limit, ktype, ipos) {
        // https://github.com/EOSIO/eosjs-api/blob/master/docs/api.md#eos.getTableRows
        return new Promise<any>((resolve, reject) => {
            this.waitEosjs.then(() => {
                this.eos.getTableRows(true, contract, scope, table, tkey, lowerb, upperb, limit, ktype, ipos).then(function (_data) {
                    resolve(_data);
                }).catch(error => {
                    console.error(error);
                }); 
            }).catch((error) => {
                console.error(error);
                reject(error);
            });   
        });        

    }

    testScatterOnLocalNetwork() {
        console.log("ScatterService.testScatterOnLocalNetwork()");
        
        if ((<any>window).ScatterJS) {
            this.ScatterJS = (<any>window).ScatterJS;
            this.lib = this.ScatterJS.scatter;  
            this.ScatterJS.plugins( new ScatterEOS() );
            (<any>window).ScatterJS = null;
        }
        var network = {
            blockchain: "eos",
            chainId: "cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4f",
            host: "127.0.0.1",
            port: 8888,
            protocol: "http",          
        }

        this.eos = this.ScatterJS.scatter.eos(network, Eos, { expireInSeconds:60 });
        
        
        this.ScatterJS.scatter.connect("Cards & Tokens", {initTimeout:10000}).then(connected => {
            if(!connected) return console.error(this.error);

            return this.ScatterJS.scatter.getIdentity({"accounts":[network]}).then( (identity)  => {
                var account = identity.accounts[0];
                
                const transactionOptions = { authorization:[`${account.name}@${account.authority}`] };
                console.log("Scatter.transfer() authority:", transactionOptions);
                this.eos.transfer(account.name, "bob", "0.0000 SYS", "memo", transactionOptions).then(trx => {
                    // That's it!
                    console.log(`Transaction ID: ${trx.transaction_id}`, trx);
                    // en Notas está el json que describe el objeto trx
                    
                }).catch(error => {
                    console.error(error);
                });
                
            }).catch( err => { console.error(err); });            

        });        
    }
}
