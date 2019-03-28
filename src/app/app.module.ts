import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { AppRoutingModule } from './app-routing.module';
import { CommonServicesModule } from './services/common/common.module';
import { HttpClient, HttpClientModule } from '@angular/common/http';

// perfect scrollbar
import { PerfectScrollbarModule } from 'ngx-perfect-scrollbar';
import { PERFECT_SCROLLBAR_CONFIG } from 'ngx-perfect-scrollbar';
import { PerfectScrollbarConfigInterface } from 'ngx-perfect-scrollbar';
const DEFAULT_PERFECT_SCROLLBAR_CONFIG: PerfectScrollbarConfigInterface = {
    suppressScrollX: true
};
 
import { AppComponent } from './app.component';

import { HomePage } from './pages/home/home.page';
import { NotFoundPage } from './pages/not-found/not-found.page';
import { RootPage } from './pages/root/root.page';
import { AccountPage } from './pages/account/account.page';
import { EosioTokenMathService } from './services/eosio.token-math.service';
import { LoadingOverall } from './services/common/app.service';
import { VideoPage } from './pages/video/video.page';
import { BGBoxPage } from './pages/bgbox/bgbox.page';
import { AppsPage } from './pages/apps/apps.page';
import { RegisterPage } from './pages/register/register.page';


import { ScatterService } from './services/scatter.service';
import { BGBoxService } from './services/bgbox.service';
import { CntService } from './services/cnt.service';
import { VapaeeService } from './services/vapaee.service';

import { CntRootPage } from './pages-cnt/root/cntroot.page';
import { VpeRootPage } from './pages-vpe/root/vpe-root.page';
import { VpeProfilePage } from './pages-vpe/profile/profile.page';
import { TradePage } from './pages-vpe/trade/trade.page';
import { TokensPage } from './pages-vpe/tokens/tokens.page';
import { VpeHomePage } from './pages-vpe/home/home.page';
import { ProfilePage } from './pages-cnt/profile/profile.page';
import { InventoryPage } from './pages-cnt/inventory/inventory.page';
import { CardsPage } from './pages-cnt/cards/cards.page';
import { AlbumsPage } from './pages-cnt/albums/albums.page';

import { IdenticonComponent } from './components/identicon/identicon.component';
import { EosioAccountComponent } from './components/eosio-account/eosio-account.component';
import { VpeOrdersComponent } from './components/vpe-orders/vpe-orders.component';
import { VpeHistoryComponent } from './components/vpe-history/vpe-history.component';
import { VpeWalletComponent } from './components/vpe-wallet/vpe-wallet.component';
import { VpeChartComponent } from './components/vpe-chart/vpe-chart.component';
import { VpeTokensComponent } from './components/vpe-tokens/vpe-tokens.component';
import { VpeOrderEditorComponent } from './components/vpe-order-editor/vpe-order-editor.component';

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
        VpeOrdersComponent,
        VpeHistoryComponent,
        VpeWalletComponent,
        VpeChartComponent,
        VpeTokensComponent,
        VpeOrderEditorComponent,
        LoadingOverall,
        CntRootPage,
        VpeRootPage,
        ProfilePage,
        VpeProfilePage,
        TradePage,
        TokensPage,
        VpeHomePage,
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
        CommonServicesModule,
        PerfectScrollbarModule
    ],
    providers: [
        ScatterService,
        BGBoxService,
        VapaeeService,
        CntService,
        HttpClient,
        EosioTokenMathService,
        { provide: PERFECT_SCROLLBAR_CONFIG, useValue: DEFAULT_PERFECT_SCROLLBAR_CONFIG }
    ],
    bootstrap: [AppComponent]
})
export class AppModule { }
