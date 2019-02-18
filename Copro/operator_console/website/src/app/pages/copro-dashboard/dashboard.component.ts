import {Component, OnDestroy} from '@angular/core';
import { NbThemeService } from '@nebular/theme';
import { takeWhile } from 'rxjs/operators' ;
import { SolarData } from '../../@core/data/solar';
import { CoproService } from '../../service/copro-service';

interface CardSettings {
  title: string;
  iconClass: string;
  type: string;
  onChange: Function;
}

@Component({
  selector: 'ngx-dashboard',
  styleUrls: ['./dashboard.component.scss'],
  templateUrl: './dashboard.component.html',
})
export class DashboardComponent implements OnDestroy {

  private alive = true;

  solarValue: number;
  moboCard: CardSettings = {
    title: 'Motherboard',
    iconClass: 'nb-power',
    type: 'success',
    onChange: (state) => {this.coproService.setMoboPower(state).subscribe(() => {})}
  };
  killCard: CardSettings = {
    title: 'Thrusters',
    iconClass: 'nb-close',
    type: 'danger',
    onChange: (state) => {this.coproService.setThrusterPower(state).subscribe(() => {})}
  };
  jetsonCard: CardSettings = {
    title: 'Jetson',
    iconClass: 'nb-power',
    type: 'info',
    onChange: (state) => {this.coproService.setJetsonPower(state).subscribe(() => {})}
  };

  statusCards: string;

  commonStatusCardsSet: CardSettings[] = [
    this.moboCard,
    this.killCard,
    this.jetsonCard
  ];

  statusCardsByThemes: {
    default: CardSettings[];
    cosmic: CardSettings[];
    corporate: CardSettings[];
  } = {
    default: this.commonStatusCardsSet,
    cosmic: this.commonStatusCardsSet,
    corporate: this.commonStatusCardsSet
  };

  constructor(private themeService: NbThemeService,
              private solarService: SolarData,
              private coproService: CoproService) {
    this.themeService.getJsTheme()
      .pipe(takeWhile(() => this.alive))
      .subscribe(theme => {
        this.statusCards = this.statusCardsByThemes[theme.name];
    });

    this.solarService.getSolarData()
      .pipe(takeWhile(() => this.alive))
      .subscribe((data) => {
        this.solarValue = data;
      });
  }

  ngOnDestroy() {
    this.alive = false;
  }
}
