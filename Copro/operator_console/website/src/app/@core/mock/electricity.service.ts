import { Injectable } from '@angular/core';
import { of as observableOf, Observable } from 'rxjs';
import { Electricity, ElectricityChart, ElectricityData } from '../data/electricity';

@Injectable()
export class ElectricityService extends ElectricityData {

  private listData: Electricity = 
    {
      components: [
        { name: '12v', amps: '816', wattHr: '97' },
        { name: '5v', amps: '806', wattHr: '95' },
        { name: '3.3v', amps: '803', wattHr: '94' },
        { name: 'HPF', amps: '818', wattHr: '98' },
        { name: 'HSF', amps: '809', wattHr: '96' },
        { name: 'HPA', amps: '808', wattHr: '96' },
        { name: 'HSA', amps: '815', wattHr: '97' },
        { name: 'SRP', amps: '807', wattHr: '95' },
        { name: 'SRS', amps: '792', wattHr: '92' },
        { name: 'SWF', amps: '791', wattHr: '92' },
        { name: 'SWA', amps: '786', wattHr: '89' }
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
