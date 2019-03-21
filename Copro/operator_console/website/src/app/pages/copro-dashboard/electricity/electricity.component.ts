import { Temperature } from './../../../@core/data/temperature-humidity';
import { Component, OnDestroy, EventEmitter, Input, OnInit } from '@angular/core';
import { NbThemeService } from '@nebular/theme';

import { Electricity, ElectricityChart, ElectricityData } from '../../../@core/data/electricity';
import { takeWhile } from 'rxjs/operators';
import { forkJoin } from 'rxjs';
import { ElectricityChartComponent } from './electricity-chart/electricity-chart.component';

@Component({
  selector: 'ngx-electricity',
  styleUrls: ['./electricity.component.scss'],
  templateUrl: './electricity.component.html',
})
export class ElectricityComponent implements OnDestroy, OnInit {

  private alive = true;

  listData: Electricity;
  chartData: ElectricityChart[] = [];
  amperage: number;
  @Input() amperageEmitter: EventEmitter<number>;

  newData = new EventEmitter<any>();

  type = 'week';
  types = ['week', 'month', 'year'];

  currentTheme: string;
  themeSubscription: any;



  constructor(private electricityService: ElectricityData,
    private themeService: NbThemeService) {
    this.themeService.getJsTheme()
      .pipe(takeWhile(() => this.alive))
      .subscribe(theme => {
        this.currentTheme = theme.name;
      });

    forkJoin(
      this.electricityService.getListData(),
      this.electricityService.getChartData(),
    )
      .pipe(takeWhile(() => this.alive))
      .subscribe(([listData, chartData]: [Electricity, ElectricityChart[]]) => {
        this.listData = listData;
        //this.chartData = chartData;
      });
  }

  ngOnInit() {
    this.amperageEmitter.subscribe(a => {
      this.amperage = a;
      this.chartData.push({ label: '', value: this.amperage });
      this.newData.emit();
    });
  }

  ngOnDestroy() {
    this.alive = false;
  }
}
