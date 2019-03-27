import { Component, Input, OnDestroy, OnInit } from '@angular/core';
import { NbThemeService } from '@nebular/theme';
import { interval, Subscription } from 'rxjs';
import { switchMap, takeWhile } from 'rxjs/operators';
import { LiveUpdateChart, EarningData } from '../../../@core/data/earning';
import { pipe } from '@angular/core/src/render3';

@Component({
	selector: 'ngx-depth-card',
	styleUrls: ['./depth-card.component.scss'],
	templateUrl: './depth-card.component.html',
})
export class DepthCardComponent {

	private alive = true;
	private date = new Date;

	@Input() depthValue: number;

	intervalSubscription: Subscription;
	currentTheme: string;
	liveUpdateChartData: { value: [string, number] }[] = [];

	constructor(private themeService: NbThemeService) {
		this.themeService.getJsTheme()
			.pipe(takeWhile(() => this.alive))
			.subscribe(theme => {
				this.currentTheme = theme.name;
			});
	}

	ngOnInit() {
		this.addDepthValue();
	}

	private addDepthValue() {
		this.liveUpdateChartData.push({
			value: [
				[
					this.date.getDay(),
					this.date.getHours(),
					this.date.getMinutes(),
					this.date.getSeconds()
				].join('/'),
				this.depthValue
			]
		});
		this.startReceivingDepthValues();
	}

	startReceivingDepthValues() {
		if (this.intervalSubscription){
			this.intervalSubscription.unsubscribe();
		}
		console.log(this.liveUpdateChartData);
		this.intervalSubscription = interval(200)
			.subscribe(() => {
				this.liveUpdateChartData.push({
					value: [
						[
							this.date.getDay(),
							this.date.getHours(),
							this.date.getMinutes(),
							this.date.getSeconds()
						].join('/'),
						this.depthValue
					]
				});
			});
	}

	ngOnDestroy() {
		this.alive = false;
	}
}
