import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { RootPage } from './pages/root/root.page';
import { HomePage } from './pages/home/home.page';
import { NotFoundPage } from './pages/not-found/not-found.page';

const routes: Routes = [
  { path: '',                     data: { state: "root" }, redirectTo: '/home', pathMatch: 'full' },
  { path: '',                     data: { state: "root" }, component: RootPage,
    children: [
      { path: 'home',             data: { state: "home" }, component: HomePage }
    ]
  },
  { path: '**',                   data: { state: "404" }, component: NotFoundPage }
];

@NgModule({
    imports: [RouterModule.forRoot(routes)],
    exports: [RouterModule]
})
export class AppRoutingModule { }
