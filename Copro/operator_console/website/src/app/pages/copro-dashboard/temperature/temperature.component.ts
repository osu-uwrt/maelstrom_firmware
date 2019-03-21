import { Component, OnDestroy, Input } from '@angular/core';
import { NbThemeService } from '@nebular/theme';
import { Temperature, TemperatureHumidityData } from '../../../@core/data/temperature-humidity';
import { takeWhile } from 'rxjs/operators';
import { forkJoin } from 'rxjs';
import { CoproService } from '../../../service/copro-service';

@Component({
  selector: 'ngx-temperature',
  styleUrls: ['./temperature.component.scss'],
  templateUrl: './temperature.component.html',
})
export class TemperatureComponent implements OnDestroy {

  private alive = true;

  @Input() currentTemp: number;
  temperatureData: Temperature;
  temperature: number;
  temperatureOff = false;
  temperatureMode = 'cool';

  colors: any;
  themeSubscription: any;

  constructor(private theme: NbThemeService,
              private temperatureHumidityService: TemperatureHumidityData,
              private coproService: CoproService) {
    this.theme.getJsTheme()
      .pipe(takeWhile(() => this.alive))
      .subscribe(config => {
      this.colors = config.variables;
    });

    forkJoin(
      this.temperatureHumidityService.getTemperatureData(),
      this.temperatureHumidityService.getHumidityData(),
    )
      .subscribe(([temperatureData, humidityData]: [Temperature, Temperature]) => {
        this.temperatureData = temperatureData;
        this.temperature = this.temperatureData.value;
      });
  }

  togglePower() {
    this.coproService.setPeltierPower(this.temperatureOff).subscribe(() => {});
    this.temperatureOff = !this.temperatureOff;
  }

  ngOnDestroy() {
    this.alive = false;
  }
}
