import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { VapaeeService } from 'src/app/services/vapaee.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { GoogleChartInterface } from 'src/app/services/google-chart-service/google-charts-interfaces';


// https://www.devrandom.it/software/ng2-google-charts/
// import { GoogleChartInterface } from 'ng2-google-charts/google-charts-interfaces';




@Component({
    selector: 'vpe-panel-chart',
    templateUrl: './vpe-panel-chart.component.html',
    styleUrls: ['./vpe-panel-chart.component.scss']
})
export class VpePanelChartComponent implements OnChanges {

    data:any[];
    zoom:number;

    constructor(
        public vapaee: VapaeeService,
        public local: LocalStringsService
    ) {
        this.zoom = 5;
        this.data = [
            ['Sat', 19, 23, 34, 45],
            ['Sun', 24, 35, 57, 78],
            ['Mon', 20, 28, 38, 60],
            ['Tue', 31, 38, 55, 66],
            ['Wed', 50, 55, 77, 80],
            ['Thu', 77, 70, 66, 50],
            ['Fri', 68, 66, 22, 15],
            ['Sat', 19, 23, 34, 45],
            ['Sun', 24, 35, 57, 78],
            ['Mon', 20, 28, 38, 60],
            ['Tue', 31, 38, 55, 66],
            ['Wed', 50, 55, 77, 80],
            ['Thu', 77, 70, 66, 50],
            ['Fri', 68, 66, 22, 15]
            // Treat first row as data as well.
        ]
    }
    
    //*/
    chartData:GoogleChartInterface = {        
        chartType: 'CandlestickChart',
        dataTable: [
            ['Sat', 19, 23, 34, 45],
            ['Sun', 24, 35, 57, 78],
            ['Mon', 20, 28, 38, 60],
            ['Tue', 31, 38, 55, 66],
            ['Wed', 50, 55, 77, 80],
            ['Thu', 77, 70, 66, 50],
            ['Fri', 68, 66, 22, 15],
            ['Sat', 19, 23, 34, 45],
            ['Sun', 24, 35, 57, 78],
            ['Mon', 20, 28, 38, 60],
            ['Tue', 31, 38, 55, 66],
            ['Wed', 50, 55, 77, 80],
            ['Thu', 77, 70, 66, 50],
            ['Fri', 68, 66, 22, 15]
            // Treat first row as data as well.
        ],
        opt_firstRowIsData: true,
        options: {
            legend:'none'
        }
    }
    /*/
    get chartData(): GoogleChartInterface {
        var data:GoogleChartInterface = {        
            chartType: 'CandlestickChart',
            dataTable: [
                ['Sat', 19, 23, 34, 45],
                ['Sun', 24, 35, 57, 78],
                ['Mon', 20, 28, 38, 60],
                ['Tue', 31, 38, 55, 66],
                ['Wed', 50, 55, 77, 80],
                ['Thu', 77, 70, 66, 50],
                ['Fri', 68, 66, 22, 15],
                ['Sat', 19, 23, 34, 45],
                ['Sun', 24, 35, 57, 78],
                ['Mon', 20, 28, 38, 60],
                ['Tue', 31, 38, 55, 66],
                ['Wed', 50, 55, 77, 80],
                ['Thu', 77, 70, 66, 50],
                ['Fri', 68, 66, 22, 15]
                // Treat first row as data as well.
            ],
            opt_firstRowIsData: true,
            options: {
                legend:'none'
            }
        };
        return data;
    }
    //*/

    ready(event) {
        console.log("ready", event);
    }

    error(event) {
        console.log("error", event);
    }

    select(event) {
        console.log("select", event);
    }

    mouseOver(event) {
        console.log("mouseOver", event);
    }

    mouseOut(event) {
        console.log("mouseOut", event);
    }

    mouseWheel(event) {
        console.log("mouseWheel", event);
        
    }


    ngOnChanges() {

    }
}
