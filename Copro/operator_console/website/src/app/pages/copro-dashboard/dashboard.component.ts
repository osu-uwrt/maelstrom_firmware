import { Component, OnDestroy, EventEmitter } from "@angular/core";
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
	logicCurrents = [0, 0, 0];
	depthValue = 0;
	totalCurrentEmitter = new EventEmitter<number>();
	temp = 82;

	twelveVoltCard: CardSettings = {
		title: "12 V Power",
		iconClass: "nb-power",
		type: "success",
		onChange: state => {
			this.coproService.setTwelvePower(state).subscribe(() => { });
		}
	};
	killCard: CardSettings = {
		title: "Thrusters",
		iconClass: "nb-close",
		type: "danger",
		onChange: state => {
			this.coproService.setThrusterPower(state).subscribe(() => { });
		}
	};
	fiveVoltCard: CardSettings = {
		title: "5 V Power",
		iconClass: "nb-power",
		type: "info",
		onChange: state => {
			this.coproService.setFivePower(state).subscribe(() => { });
		}
	};

	statusCards: string;

	commonStatusCardsSet: CardSettings[] = [
		this.twelveVoltCard,
		this.killCard,
		this.fiveVoltCard
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
		this.getLogicCurrents();
		this.getDepthValue();
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
			this.totalCurrentEmitter.emit(i[0] + i[1]);
		});
		setTimeout(() => this.getBattery(), 500);
	}

	getTemperature() {
		this.coproService
			.getTemperature()
			.subscribe(t => (this.temp = Number(((t * 9) / 5 + 32).toFixed(2))));
		setTimeout(() => this.getTemperature(), 500);
	}

	getLogicCurrents() {
		this.coproService
			.getLogicCurrents()
			.subscribe(i => {
				//in order of 3.3, 5, 12
				this.logicCurrents = i;
			});
		setTimeout(() => this.getLogicCurrents(), 500);
	}

	getDepthValue() {
		this.coproService.getDepth().subscribe(d => (this.depthValue = d));
		setTimeout(() => this.getDepthValue(), 500);
	}

	ngOnDestroy() {
		this.alive = false;
	}
}
