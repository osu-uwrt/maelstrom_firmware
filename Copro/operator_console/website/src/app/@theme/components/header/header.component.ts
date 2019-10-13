import { Component, Input, OnInit } from '@angular/core';

import { NbMenuService, NbSidebarService } from '@nebular/theme';
import { UserData } from '../../../@core/data/users';
import { AnalyticsService } from '../../../@core/utils';
import { LayoutService } from '../../../@core/utils';
import { CoproService } from '../../../service/copro-service';

@Component({
  selector: 'ngx-header',
  styleUrls: ['./header.component.scss'],
  templateUrl: './header.component.html',
})
export class HeaderComponent implements OnInit {

  @Input() position = 'normal';

  user: any;
  memory: number = .4;
  latency: number = 5;

  userMenu = [{ title: 'Profile' }, { title: 'Log out' }];

  constructor(private sidebarService: NbSidebarService,
              private menuService: NbMenuService,
              private userService: UserData,
              private analyticsService: AnalyticsService,
              private layoutService: LayoutService,
              private coproService: CoproService) {
    this.getMemory();
    this.getLatency();
  }

  ngOnInit() {
    this.userService.getUsers()
      .subscribe((users: any) => this.user = users.nick);
  }

  toggleSidebar(): boolean {
    this.sidebarService.toggle(true, 'menu-sidebar');
    this.layoutService.changeLayoutSize();

    return false;
  }

  goToHome() {
    this.menuService.navigateHome();
  }

  getMemory() {
		this.coproService
			.getMemory()
			.subscribe(t => (this.memory = t));
		setTimeout(() => this.getMemory(), 1000);
  }

  getLatency() {
		this.coproService
			.getLatency()
			.subscribe(t => (this.latency = t / 1000000));
		setTimeout(() => this.getLatency(), 1000);
	}

  reset() {
    this.coproService.reset().subscribe()
  }
}
