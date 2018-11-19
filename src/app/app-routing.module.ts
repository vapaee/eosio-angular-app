import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { RootPage } from './pages/root/root.page';
import { HomePage } from './pages/home/home.page';
import { NotFoundPage } from './pages/not-found/not-found.page';
import { AccountPage } from './pages/account/account.page';

const routes: Routes = [
  { path: '',                     data: { state: "root" }, redirectTo: '/home', pathMatch: 'full' },
  { path: '',                     data: { state: "root" }, component: RootPage,
    children: [
      { path: 'home',             data: { state: "home" }, component: HomePage },
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
