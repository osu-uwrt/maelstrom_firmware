import { Temperature } from './../../../@core/data/temperature-humidity';
import { Component, OnDestroy, EventEmitter, Input } from '@angular/core';
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
export class ElectricityComponent implements OnDestroy {

  private alive = true;

  listData: Electricity;
  chartData: ElectricityChart[] = [];
  amperage: number;
  @Input('amperage')
  set chartValue(amperage: number) {
    this.amperage = amperage;
    this.chartData.push({ label: '', value: this.amperage });
    this.newData.emit();
  }

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


  ngOnDestroy() {
    this.alive = false;
  }
}
