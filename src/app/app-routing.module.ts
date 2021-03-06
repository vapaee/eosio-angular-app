import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { RootPage } from './pages/root/root.page';
import { HomePage } from './pages/home/home.page';
import { NotFoundPage } from './pages/not-found/not-found.page';
import { AccountPage } from './pages/account/account.page';
import { VideoPage } from './pages/video/video.page';
import { BGBoxPage } from './pages/bgbox/bgbox.page';
import { RegisterPage } from './pages/register/register.page';
import { CntRootPage } from './pages-cnt/root/cntroot.page';
import { CardsPage } from './pages-cnt/cards/cards.page';
import { InventoryPage } from './pages-cnt/inventory/inventory.page';
import { AlbumsPage } from './pages-cnt/albums/albums.page';
import { ProfilePage } from './pages-cnt/profile/profile.page';
import { VpeRootPage } from './pages-vpe/root/vpe-root.page';
import { TradePage } from './pages-vpe/trade/trade.page';
import { TokensPage } from './pages-vpe/tokens/tokens.page';
import { VpeHomePage } from './pages-vpe/home/home.page';
import { VpeAccountPage } from './pages-vpe/account/account.page';
import { VpeWPPage } from './pages-vpe/wp/wp.page';
import { VpeComingSoonPage } from './pages-vpe/coming-soon/coming-soon.page';

const routes: Routes = [
  { path: '',                            data: { state: "root" }, redirectTo: '/exchange/home', pathMatch: 'full' },
  // { path: 'wp',                          data: { state: "wp" }, component: VpeWPPage },
  { path: 'cnt',                         data: { state: "root" }, component: CntRootPage,
    children: [
      { path: 'cards',                   data: { state: "cards" }, component: CardsPage },
      { path: 'inventory/:profile',      data: { state: "inventory" }, component: InventoryPage },
      { path: 'albums',                  data: { state: "albums" }, component: AlbumsPage },
      { path: 'profile/:profile',        data: { state: "profile" }, component: ProfilePage }
    ]
  },
  { path: 'exchange',                    data: { state: "root" }, component: VpeRootPage,
    children: [
      { path: '',                        data: { state: "root" }, redirectTo: '/exchange/home', pathMatch: 'full' },
      /*/
      { path: 'wp',                      data: { state: "wp" }, component: VpeComingSoonPage },
      { path: 'home',                    data: { state: "home" }, component: VpeComingSoonPage },
      { path: 'trade/:scope',            data: { state: "trade" }, component: VpeComingSoonPage },
      { path: 'tokens',                  data: { state: "tokens" }, component: VpeComingSoonPage, children: [
          { path: ':symbol',             data: { state: "edit" }, component: VpeComingSoonPage }
      ] },
      { path: 'account',                 data: { state: "guest" }, component: VpeComingSoonPage},
      { path: 'account/:name',           data: { state: "account" }, component: VpeComingSoonPage }
      /*/
      { path: 'wp',                      data: { state: "wp" }, component: VpeWPPage },
      { path: 'home',                    data: { state: "home" }, component: VpeHomePage },
      { path: 'trade/:scope',            data: { state: "trade" }, component: TradePage },
      { path: 'tokens',                  data: { state: "tokens" }, component: TokensPage, children: [
          { path: ':symbol',             data: { state: "edit" }, component: TokensPage }
      ] },
      { path: 'account',                 data: { state: "guest" }, component: VpeAccountPage},
      { path: 'account/:name',           data: { state: "account" }, component: VpeAccountPage }
      //*/
    ]
  },
  { path: ':network',                    data: { state: "root" }, component: RootPage,
    children: [
      { path: 'home',                    data: { state: "home" }, component: HomePage },
      { path: 'video',                   data: { state: "video" }, component: VideoPage },
      { path: 'bgbox',                   data: { state: "bgbox" }, component: BGBoxPage, children: [
          { path: 'register',            data: { state: "register" }, component: RegisterPage }
      ] },
      { path: 'account',                 data: { state: "account" }, component: AccountPage},
      { path: 'account/:name',           data: { state: "account_name" }, component: AccountPage }      
    ]
  },
  { path: '**',                          data: { state: "404" }, component: NotFoundPage }
];

@NgModule({
    imports: [RouterModule.forRoot(routes)],
    exports: [RouterModule]
})
export class AppRoutingModule { }
