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

    @Input() data:any[];
    zoom:number;
    component:GoogleChartComponentInterface;
    zomm_min: number = 5;

    constructor(
        public vapaee: VapaeeService,
        public local: LocalStringsService
    ) {
        this.zoom = 24;
    }
    
    public _chartData:GoogleChartInterface;
    
    // counter = 0;
    get chartData(): GoogleChartInterface {
        return this._chartData;
    }    

    ready(event) {
        this.component = event.component;
        console.assert(!!this.component, "ERROR: ", event);
    }

    error(event) {
        //console.log("error", event);
    }

    select(event) {
        //console.log("select", event);
    }

    mouseOver(event) {
        //console.log("mouseOver", event);
    }

    mouseOut(event) {
        //console.log("mouseOut", event);
    }

    private recreateDataTable() {        
        var data = [];
        var zoom = this.zoom
        if (zoom > this.data.length) {
            zoom = this.data.length;
        }
        for (var i=0; i<zoom; i++) {
            data.push(this.data[this.data.length - zoom + i]);    
        }
        // console.log("********************* recreateDataTable()", data);
        return data;
    }

    private prepareChartDataStyles() {
        var current = "green";
        for (var i=0; i<this.data.length; i++) {
            var row = this.data[i];
            // console.log("--->", this.data[i]);
            if (row[2] > row[3]) {
                current = "red";
            } else if (row[2] < row[3]) {
                current = "green";
            }
            // this.data[i].push("color: blue");
        }        
    }

    mouseWheel(event) {
        //console.log("mouseWheel", event);
        this.zoom -= event.delta;
        if (this.zoom < this.zomm_min) this.zoom = this.zomm_min;
        if (this.zoom > this.data.length) this.zoom = this.data.length;
        // this.recreateChartData();
        //console.log(this._chartData);
        this.component.redraw(this.recreateDataTable(), null);
    }

    timer;
    ngOnChanges() {
        //console.log("********************* ngOnChanges()", this.data);
        // if (this.data && this.data.length > 0) {
        if (this.data) {
            this._chartData = null;
            clearTimeout(this.timer);
            this.timer = setTimeout(_ => {
                if (this.data.length > 0 && this.data[0].length == 5 ){
                    this.prepareChartDataStyles();
                }
                var data = this.recreateDataTable();
                var baseline = 0; 
                if (data.length > 0) {
                    baseline = data[data.length-1][3]
                    // console.log("--------------------------------------->", data[data.length-1], baseline);
                }
                
                this._chartData = {        
                    chartType: 'CandlestickChart',
                    dataTable: data,
                    opt_firstRowIsData: true,
    
                    // https://developers.google.com/chart/interactive/docs/gallery/candlestickchart#data-format
                    options: {
                        legend:'none',
                        backgroundColor: "#0e1110",
                        candlestick: {
                            fallingColor: { strokeWidth: 0, fill: '#a52714' }, // red
                            risingColor: { strokeWidth: 0, fill: '#0f9d58' }   // green
                        },
                        colors: ["#444400"],
                        height: 300,
                        bar: {
                            groupWidth: "80%"
                        },
                        hAxis: {
                            textStyle: {
                                color: "#AAA"
                            }
                        },
                        vAxis: {
                            gridlines: {
                                color: '#888',
                                // count: 10
                            },
                            minorGridlines: {
                                color: '#222',
                                // count: 10
                            },
                            textStyle: {
                                color: "white"
                            },
                            baseline: baseline,
                            baselineColor: "#dddd00",
                            // format: "#,### TLOS",
                            title: 'Price',
                            titleTextStyle: {
                                color: 'white'
                            }
                        }
                        
                    }
                }
            }, 20);
        }
    }
}
