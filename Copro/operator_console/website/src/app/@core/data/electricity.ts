import { Observable } from 'rxjs';

export interface Components {
  name: string;
  amps: string;
}

export interface Electricity {
  components: Components[];
}

export interface ElectricityChart {
  label: string;
  value: number;
}

export abstract class ElectricityData {
  abstract getListData(): Observable<Electricity>;
  abstract getChartData(): Observable<ElectricityChart[]>;
}
