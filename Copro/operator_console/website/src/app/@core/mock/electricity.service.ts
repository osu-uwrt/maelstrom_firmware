import { Injectable } from '@angular/core';
import { of as observableOf, Observable } from 'rxjs';
import { Electricity, ElectricityChart, ElectricityData } from '../data/electricity';

@Injectable()
export class ElectricityService extends ElectricityData {

  private listData: Electricity = 
    {
      components: [
		{ name: '3.3v', amps: '803'},
		{ name: '5v', amps: '806'},
        { name: '12v', amps: '816'}
      ],
    };

  private chartPoints = [
    490, 490, 495, 500,
    505, 510, 520, 530,
    550, 580, 630, 720,
    800, 840, 860, 870,
    870, 860, 840, 800,
    720, 200, 145, 130,
    130, 145, 200, 570,
    635, 660, 670, 670,
    660, 630, 580, 460,
    380, 350, 340, 340,
    340, 340, 340, 340,
    340, 340, 340,
  ];

  chartData: ElectricityChart[];

  constructor() {
    super();
    this.chartData = this.chartPoints.map((p, index) => ({
      label: (index % 5 === 3) ? `${Math.round(index / 5)}` : '',
      value: p,
    }));
  }

  getListData(): Observable<Electricity> {
    return observableOf(this.listData);
  }

  getChartData(): Observable<ElectricityChart[]> {
    return observableOf(this.chartData);
  }
}
