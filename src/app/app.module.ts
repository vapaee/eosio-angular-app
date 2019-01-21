import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { AppRoutingModule } from './app-routing.module';
import { CommonServicesModule } from './services/common/common.module';
import { HttpClient, HttpClientModule } from '@angular/common/http';

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
import { CntService } from './services/cnt.service';

import { CntRootPage } from './pages-cnt/root/cntroot.page';
import { ProfilePage } from './pages-cnt/profile/profile.page';
import { InventoryPage } from './pages-cnt/inventory/inventory.page';
import { CardsPage } from './pages-cnt/cards/cards.page';
import { AlbumsPage } from './pages-cnt/albums/albums.page';

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
        LoadingOverall,
        CntRootPage,
        ProfilePage,
        InventoryPage,
        CardsPage,
        AlbumsPage
    ],
    entryComponents: [
        LoadingOverall
    ],
    imports: [
        BrowserModule,
        AppRoutingModule,
        HttpClientModule,
        CommonServicesModule
    ],
    providers: [
        ScatterService,
        BGBoxService,
        CntService,
        HttpClient,
        EosioTokenMathService
    ],
    bootstrap: [AppComponent]
})
export class AppModule { }
