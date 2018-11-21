import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { AppRoutingModule } from './app-routing.module';
import { CommonServicesModule } from './services/common/common.module';

import { AppComponent } from './app.component';

import { HomePage } from './pages/home/home.page';
import { NotFoundPage } from './pages/not-found/not-found.page';
import { RootPage } from './pages/root/root.page';
import { ScatterService } from './services/scatter.service';
import { AccountPage } from './pages/account/account.page';
import { IdenticonComponent } from './components/identicon/identicon.component';
import { EosioAccountComponent } from './components/eosio-account/eosio-account.component';

@NgModule({
    declarations: [
        AppComponent,
        HomePage,
        NotFoundPage,
        RootPage,
        AccountPage,
        IdenticonComponent,
        EosioAccountComponent
    ],
    imports: [
        BrowserModule,
        AppRoutingModule,
        CommonServicesModule
    ],
    providers: [
        ScatterService
    ],
    bootstrap: [AppComponent]
})
export class AppModule { }
