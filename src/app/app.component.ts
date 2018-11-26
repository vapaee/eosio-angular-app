import { Component, HostListener } from '@angular/core';
import { AppService } from './services/common/app.service';

@Component({
    selector: 'app-root',
    template: '<router-outlet></router-outlet>',
    styles: ['']
})
export class AppComponent {
  
    constructor(
        private app: AppService
    ) {
        this.app.init();
    }
    ngOnInit() {}
    
}
