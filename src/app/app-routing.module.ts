import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { RootPage } from './pages/root/root.page';
import { HomePage } from './pages/home/home.page';
import { NotFoundPage } from './pages/not-found/not-found.page';
import { AccountPage } from './pages/account/account.page';
import { VideoPage } from './pages/video/video.page';
import { BGBoxPage } from './pages/bgbox/bgbox.page';
import { AppsPage } from './pages/apps/apps.page';
import { NewAppPage } from './pages/newapp/newapp.page';

const routes: Routes = [
  { path: '',                     data: { state: "root" }, redirectTo: '/eos/home', pathMatch: 'full' },
  { path: ':network',             data: { state: "root" }, component: RootPage,
    children: [
      { path: 'home',             data: { state: "home" }, component: HomePage },
      { path: 'video',            data: { state: "video" }, component: VideoPage },
      { path: 'bgbox',            data: { state: "bgbox" }, component: BGBoxPage, children: [
          { path: 'newapp',         data: { state: "newapp" }, component: NewAppPage },
      ] },
      { path: 'account',          data: { state: "account" }, component: AccountPage},
      { path: 'account/:name',    data: { state: "account_name" }, component: AccountPage }      
    ]
  },
  { path: '**',                   data: { state: "404" }, component: NotFoundPage }
];

@NgModule({
    imports: [RouterModule.forRoot(routes)],
    exports: [RouterModule]
})
export class AppRoutingModule { }
