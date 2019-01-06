import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { AppRoutingModule } from './app-routing.module';
import { CommonServicesModule } from './services/common/common.module';

import { AppComponent } from './app.component';

import { HomePage } from './pages/home/home.page';
import { NotFoundPage } from './pages/not-found/not-found.page';
import { RootPage } from './pages/root/root.page';
import { AccountPage } from './pages/account/account.page';
import { IdenticonComponent } from './components/identicon/identicon.component';
import { EosioAccountComponent } from './components/eosio-account/eosio-account.component';
import { EosioTokenMathService } from './services/eosio.token-math.service';
import { LoadingOverall } from './services/common/app.service';
import { VideoPage } from './pages/video/video.page';
import { BGBoxPage } from './pages/bgbox/bgbox.page';
import { AppsPage } from './pages/apps/apps.page';
import { RegisterPage } from './pages/register/register.page';


import { ScatterService } from './services/scatter.service';
import { BGBoxService } from './services/bgbox.service';
import { VapaeeService } from './services/vapaee.service';



@NgModule({
    declarations: [
        AppComponent,
        HomePage,
        VideoPage,
        BGBoxPage,
        NotFoundPage,
        AppsPage,
        RegisterPage,
        RootPage,
        AccountPage,
        IdenticonComponent,
        EosioAccountComponent,
        LoadingOverall
    ],
    entryComponents: [
        LoadingOverall
    ],
    imports: [
        BrowserModule,
        AppRoutingModule,
        CommonServicesModule
    ],
    providers: [
        ScatterService,
        BGBoxService,
        VapaeeService,
        EosioTokenMathService
    ],
    bootstrap: [AppComponent]
})
export class AppModule { }
