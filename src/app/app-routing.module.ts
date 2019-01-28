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

const routes: Routes = [
  { path: '',                            data: { state: "root" }, redirectTo: '/eos/home', pathMatch: 'full' },
  { path: 'cnt',                         data: { state: "root" }, component: CntRootPage,
    children: [
      { path: 'cards',                   data: { state: "cards" }, component: CardsPage },
      { path: 'inventory/:publisher',    data: { state: "inventory" }, component: InventoryPage },
      { path: 'albums',                  data: { state: "albums" }, component: AlbumsPage },
      { path: 'profile/:publisher',      data: { state: "profile" }, component: ProfilePage }
    ]
  },
  { path: ':network',                    data: { state: "root" }, component: RootPage,
    children: [
      { path: 'home',                    data: { state: "home" }, component: HomePage },
      { path: 'video',                   data: { state: "video" }, component: VideoPage },
      { path: 'bgbox',                   data: { state: "bgbox" }, component: BGBoxPage, children: [
        { path: 'register',              data: { state: "register" }, component: RegisterPage }
    ] },
    { path: 'account',                   data: { state: "account" }, component: AccountPage},
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
