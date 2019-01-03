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

export interface ScatterJSDef {
    plugins?:any,
    scatter?:any
}
