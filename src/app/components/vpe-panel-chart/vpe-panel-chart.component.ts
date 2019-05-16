import { Component, Input, OnChanges, Output } from '@angular/core';
import { EventEmitter } from '@angular/core';
import { VapaeeService } from 'src/app/services/vapaee.service';
import { LocalStringsService } from 'src/app/services/common/common.services';
import { GoogleChartInterface, GoogleChartComponentInterface } from 'src/app/services/google-chart-service/google-charts-interfaces';
import { GoogleChartComponent } from 'src/app/services/google-chart-service';


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
    component:GoogleChartComponentInterface;

    constructor(
        public vapaee: VapaeeService,
        public local: LocalStringsService
    ) {
        this.zoom = 5;
        setTimeout(_ => {
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
            ];
            this._chartData = {        
                chartType: 'CandlestickChart',
                dataTable: this.recreateDataTable(),
                opt_firstRowIsData: true,

                // https://developers.google.com/chart/interactive/docs/gallery/candlestickchart#data-format
                options: {
                    legend:'none',
                    // backgroundColor: "#1f211f",
                    // height: 600,
                    bar: {
                        groupWidth: "80%"
                    }
                }
            }
        }, 0);
    }    
    
    public _chartData:GoogleChartInterface;
    
    counter = 0;
    get chartData(): GoogleChartInterface {
        this.counter++
        if (this.counter>10000) {
            console.error("chartData() canceled", this.counter);
            return null;
        }
        return this._chartData;
    }    

    ready(event) {
        this.component = this.chartData.component;
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

    private recreateDataTable() {
        var data = [];
        for (var i=0; i<this.zoom; i++) {
            data.push(this.data[this.data.length - this.zoom + i]);    
        }
        return data;
    }

    mouseWheel(event) {
        console.log("mouseWheel", event);
        this.zoom -= event.delta;
        if (this.zoom < 5) this.zoom = 5;
        if (this.zoom > this.data.length) this.zoom = this.data.length;
        // this.recreateChartData();
        console.log(this._chartData);
        this.component.redraw(this.recreateDataTable(), null);
    }


    ngOnChanges() {

    }
}
