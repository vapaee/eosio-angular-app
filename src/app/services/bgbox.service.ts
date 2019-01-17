import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { ScatterService } from './scatter.service';
import { VapaeeService, SlugUtils } from './vapaee.service';

@Injectable()
export class BGBoxService {

    public contract:string;
    boardgamebox:string = "boardgamebox";

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
    constructor(private vapaee: VapaeeService, private scatter: ScatterService) {
        this.slug = new SlugUtils();
        this.contract = this.boardgamebox;
    }

    decodeSlug(nick:{top:string,low:string,str?:string}) {
        return this.slug.decodeSlug(nick);
    }

    // -------------------------
    droptables() {
        console.log("BGBoxService.droptables()");
        return new Promise<any>((resolve, reject) => {
            try {
                this.scatter.getContract(this.boardgamebox).then(contract => {
                    try {
                        contract.droptables({}, this.scatter.authorization).then((response => {
                            console.log("response", response);
                            resolve(response);
                        })).catch(err => { console.error(err); reject(err); });
                    } catch (err) { console.error(err); reject(err); }
                }).catch(err => { console.error(err); reject(err); });
            } catch (err) { console.error(err); reject(err); }
        }).catch(err => console.error("AAAAAAAA", "ESTO FUNCIONA; SACÁ LO DEMÁS!!!!!", err) );
    }

    registerApp(owner:string, contract:string, nick:string, title:string, inventory:number) {
        return this.vapaee.registerApp(owner, contract, nick, title, inventory);
    }

    registerPublisher(owner:string, nick:string, title:string) {
        return this.vapaee.registerPublisher(owner, nick, title);
    }       
}
