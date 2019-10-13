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
  temperature: number = 104;
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
  }

  togglePower() {
    this.temperatureOff = !this.temperatureOff;
    if (this.temperatureOff) {
      this.coproService.setThermostat(255).subscribe(() => {});
    } else {
      this.updateThermostat()
    }
  }

  updateThermostat() {
    this.coproService.setThermostat((this.temperature - 32) / 9 * 5).subscribe(() => {});
  }

  ngOnDestroy() {
    this.alive = false;
  }
}
