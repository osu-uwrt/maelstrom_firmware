import { Component, OnDestroy } from "@angular/core";
import { NbThemeService } from "@nebular/theme";
import { takeWhile } from "rxjs/operators";
import { SolarData } from "../../@core/data/solar";
import { CoproService } from "../../service/copro-service";

interface CardSettings {
  title: string;
  iconClass: string;
  type: string;
  onChange: Function;
}

@Component({
  selector: "ngx-dashboard",
  styleUrls: ["./dashboard.component.scss"],
  templateUrl: "./dashboard.component.html"
})
export class DashboardComponent implements OnDestroy {
  private alive = true;

  stbdVoltage = 10;
  portVoltage = 19.45;
  stbdCurrent = 0;
  portCurrent = 0.45;
  totalCurrent = 0.45;
  temp = 82;

  moboCard: CardSettings = {
    title: "Motherboard",
    iconClass: "nb-power",
    type: "success",
    onChange: state => {
      this.coproService.setMoboPower(state).subscribe(() => {});
    }
  };
  killCard: CardSettings = {
    title: "Thrusters",
    iconClass: "nb-close",
    type: "danger",
    onChange: state => {
      this.coproService.setThrusterPower(state).subscribe(() => {});
    }
  };
  jetsonCard: CardSettings = {
    title: "Jetson",
    iconClass: "nb-power",
    type: "info",
    onChange: state => {
      this.coproService.setJetsonPower(state).subscribe(() => {});
    }
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

  constructor(
    private themeService: NbThemeService,
    private solarService: SolarData,
    private coproService: CoproService
  ) {
    this.themeService
      .getJsTheme()
      .pipe(takeWhile(() => this.alive))
      .subscribe(theme => {
        this.statusCards = this.statusCardsByThemes[theme.name];
      });

    this.getBattery();
    this.getTemperature();
    this.coproService.coproDisconnected.subscribe(() =>
      alert("Copro disconnected")
    );
  }

  getBattery() {
    this.coproService.getBatVoltages().subscribe(v => {
      this.portVoltage = v[0];
      this.stbdVoltage = v[1];
    });
    this.coproService.getBatCurrents().subscribe(i => {
      this.portCurrent = i[0];
      this.stbdCurrent = i[1];
      this.totalCurrent = this.stbdCurrent + this.portCurrent;
    });
    setTimeout(() => this.getBattery(), 1000);
  }

  getTemperature() {
    this.coproService
      .getTemperature()
      .subscribe(t => (this.temp = (t * 9) / 5 + 32));
    setTimeout(() => this.getTemperature(), 1000);
  }

  ngOnDestroy() {
    this.alive = false;
  }
}
