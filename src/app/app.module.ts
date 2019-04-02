import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { AppRoutingModule } from './app-routing.module';
import { CommonServicesModule } from './services/common/common.module';
import { HttpClient, HttpClientModule } from '@angular/common/http';
import { FormsModule } from '@angular/forms';

import { Ng2SearchPipeModule } from 'ng2-search-filter';

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
import { VpePanelComponent } from './components/vpe-panel/vpe-panel.component';
import { VpePanelOrdersComponent } from './components/vpe-panel-orders/vpe-panel-orders.component';
import { VpePanelHistoryComponent } from './components/vpe-panel-history/vpe-panel-history.component';
import { VpePanelWalletComponent } from './components/vpe-panel-wallet/vpe-panel-wallet.component';
import { VpePanelChartComponent } from './components/vpe-panel-chart/vpe-panel-chart.component';
import { VpePanelTokensComponent } from './components/vpe-panel-tokens/vpe-panel-tokens.component';
import { VpePanelOrderEditorComponent } from './components/vpe-panel-order-editor/vpe-panel-order-editor.component';
import { VpeTokenInputComponent } from './components/vpe-token-input/vpe-token-input.component';

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
        VpePanelComponent,
        VpePanelOrdersComponent,
        VpePanelHistoryComponent,
        VpePanelWalletComponent,
        VpePanelChartComponent,
        VpePanelTokensComponent,
        VpePanelOrderEditorComponent,
        VpeTokenInputComponent,
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
        PerfectScrollbarModule,
        Ng2SearchPipeModule,
        FormsModule
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
