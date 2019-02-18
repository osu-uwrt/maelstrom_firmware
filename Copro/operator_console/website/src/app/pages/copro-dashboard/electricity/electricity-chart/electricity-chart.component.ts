import { delay, takeWhile } from 'rxjs/operators';
import { AfterViewInit, Component, Input, OnDestroy, EventEmitter } from '@angular/core';
import { NbThemeService } from '@nebular/theme';
import { LayoutService } from '../../../../@core/utils';
import { ElectricityChart } from '../../../../@core/data/electricity';

@Component({
  selector: 'ngx-electricity-chart',
  styleUrls: ['./electricity-chart.component.scss'],
  template: `
    <div echarts
         [options]="option"
         class="echart"
         (chartInit)="onChartInit($event)">
    </div>
  `,
})
export class ElectricityChartComponent implements AfterViewInit, OnDestroy {

  private alive = true;

  @Input() data: ElectricityChart[];
  @Input() newData: EventEmitter<any>;

  option: any;
  echartsIntance: any;
  eTheme: any;

  constructor(private theme: NbThemeService,
    private layoutService: LayoutService) {
    this.layoutService.onChangeLayoutSize()
      .pipe(
        takeWhile(() => this.alive),
      )
      .subscribe(() => this.resizeChart());

  }

  updateChart() {
    this.option = {
      grid: {
        left: 0,
        top: 0,
        right: 0,
        bottom: 80,
      },
      tooltip: {
        trigger: 'axis',
        axisPointer: {
          type: 'line',
          lineStyle: {
            color: this.eTheme.tooltipLineColor,
            width: this.eTheme.tooltipLineWidth,
          },
        },
        textStyle: {
          color: this.eTheme.tooltipTextColor,
          fontSize: 20,
          fontWeight: this.eTheme.tooltipFontWeight,
        },
        position: 'top',
        backgroundColor: this.eTheme.tooltipBg,
        borderColor: this.eTheme.tooltipBorderColor,
        borderWidth: 3,
        formatter: '{c0} kWh',
        extraCssText: this.eTheme.tooltipExtraCss,
      },
      xAxis: {
        type: 'category',
        boundaryGap: false,
        offset: 25,
        data: this.data.map(i => i.label),
        axisTick: {
          show: false,
        },
        axisLabel: {
          color: this.eTheme.xAxisTextColor,
          fontSize: 18,
        },
        axisLine: {
          lineStyle: {
            color: this.eTheme.axisLineColor,
            width: '2',
          },
        },
      },
      yAxis: {
        boundaryGap: [0, '5%'],
        axisLine: {
          show: false,
        },
        axisLabel: {
          show: false,
        },
        axisTick: {
          show: false,
        },
        splitLine: {
          show: true,
          lineStyle: {
            color: this.eTheme.yAxisSplitLine,
            width: '1',
          },
        },
      },
      series: [
        {
          type: 'line',
          smooth: true,
          symbolSize: 20,
          itemStyle: {
            normal: {
              opacity: 0,
            },
            emphasis: {
              color: '#ffffff',
              borderColor: this.eTheme.itemBorderColor,
              borderWidth: 2,
              opacity: 1,
            },
          },
          lineStyle: {
            normal: {
              width: this.eTheme.lineWidth,
              type: this.eTheme.lineStyle,
              color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [{
                offset: 0,
                color: this.eTheme.lineGradFrom,
              }, {
                offset: 1,
                color: this.eTheme.lineGradTo,
              }]),
              shadowColor: this.eTheme.lineShadow,
              shadowBlur: 6,
              shadowOffsetY: 12,
            },
          },
          areaStyle: {
            normal: {
              color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [{
                offset: 0,
                color: this.eTheme.areaGradFrom,
              }, {
                offset: 1,
                color: this.eTheme.areaGradTo,
              }]),
            },
          },
          data: this.data.map(i => i.value),
        },

        {
          type: 'line',
          smooth: true,
          symbol: 'none',
          lineStyle: {
            normal: {
              width: this.eTheme.lineWidth,
              type: this.eTheme.lineStyle,
              color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [{
                offset: 0,
                color: this.eTheme.lineGradFrom,
              }, {
                offset: 1,
                color: this.eTheme.lineGradTo,
              }]),
              shadowColor: this.eTheme.shadowLineDarkBg,
              shadowBlur: 14,
              opacity: 1,
            },
          },
          data: this.data.map(i => i.value),
        },
      ],
    };
  }

  ngAfterViewInit(): void {
    this.theme.getJsTheme()
      .pipe(
        takeWhile(() => this.alive),
        delay(1),
      )
      .subscribe(config => {
        this.eTheme = config.variables.electricity;
        this.newData.subscribe(() => this.updateChart());
        this.updateChart();
      });
  }

  onChartInit(echarts) {
    this.echartsIntance = echarts;
  }

  resizeChart() {
    if (this.echartsIntance) {
      this.echartsIntance.resize();
    }
  }

  ngOnDestroy() {
    this.alive = false;
  }
}
