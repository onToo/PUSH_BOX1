#include <graphics.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "database.h"
#include "box_man.h"

using namespace std;

struct _POS man;			//小人所在二维数组中的位置

/*游戏地图*/
int map[LINE][COLUMN] = { 0 };
/*int map[LINE][COLUMN] = {
	 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
     { 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
     { 0, 1, 4, 1, 0, 2, 1, 0, 2, 1, 1, 0 },
     { 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0 },
     { 0, 1, 0, 2, 0, 1, 1, 4, 1, 1, 1, 0 },
     { 0, 1, 1, 1, 1, 3, 1, 1, 1, 4, 1, 0 },
     { 0, 1, 2, 1, 1, 4, 1, 1, 1, 1, 1, 0 },
     { 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0 },
     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};*/

/********************************************** 
*判断游戏是否结束，如果不存在任何一个箱子目的地，就代表游戏结束 
*输入： 无 
*返回值： 
*           true - 游戏结束 false - 游戏继续 
**********************************************/ 
bool isGameOver() {
	for (int i = 0; i < LINE; i++) { 
		for (int j = 0; j < COLUMN; j++) { 
			if (map[i][j] == BOX_DES) return false; 
		} 
	}
	return true;
}

/**********************************************
 *游戏结束场景，在玩家通关后显示 
 *输入： 
 * bg - 背景图片变量的指针 
*返回值：
	   无 
**********************************************/ 
void gameOverScene(IMAGE* bg) {
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	settextstyle(20, 0, _T("宋体"));
	drawtext(_T("恭喜您~ \n 通关了! 没有了, 再见!"), &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void gameNextScene(IMAGE* bg) {
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	settextstyle(20, 0, _T("宋体"));
	drawtext(_T("恭喜您~ \n 通关了！任意键跳转下一关"), &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	system("pause");
	cleardevice();
	
}


/***************************************************
*改变游戏视图中一格对应道具并重新显示
*输入:
*		line    - 道具在地图数组的行下标
*		column- - 道具在地图数组的列下标
*		prop    - 道具的类型
*	返回值:无
***************************************************/
void changeMap(POS* pos, PROPS prop) {
	map[pos->x][pos->y] = prop;
	putimage(START_X + pos->y * RATIO, START_Y + pos->x * RATIO, &images[prop]);
}

/**************************************************
*实现游戏四个方向(上、下、左、右)的控制
*输入：
* direct - 人前进方向
* 输出:无
***************************************************/
void gameControl(DIRECTION direct) {
	POS next_pos = man;
	POS next_next_pos = man;

	switch (direct) {
	case UP:
		next_pos.x--;
		next_next_pos.x -= 2;
		break;
	case DOWN:

		next_pos.x++;
		next_next_pos.x += 2;
		break;
	case LEFT:
		next_pos.y--;
		next_next_pos.y -= 2;
		break;
	case RIGHT:
		next_pos.y++;
		next_next_pos.y += 2;
		break;
	}
	//宏展开 next_pos.x>=0 && next_pos.x<LINE && next_pos.y>=0 && next_pos.y <COLUMN 
	if (isValid(next_pos) && map[next_pos.x][next_pos.y] == FLOOR) {// 人的前方是地板 
		changeMap(&next_pos, MAN); //小人前进一格 
		changeMap(&man, FLOOR);
		man = next_pos;
	}
	else if (isValid(next_next_pos) && map[next_pos.x][next_pos.y] == BOX) {//人的前方是箱子 
   //两种情况，箱子前面是地板或者是箱子目的地 
		if (map[next_next_pos.x][next_next_pos.y] == FLOOR) {
			changeMap(&next_next_pos, BOX);
			changeMap(&next_pos, MAN); //小人前进一格  
			changeMap(&man, FLOOR); 
			man = next_pos;
		}
		else if (map[next_next_pos.x][next_next_pos.y] == BOX_DES) {
			changeMap(&next_next_pos, HIT);
			changeMap(&next_pos, MAN); //小人前进一格
			changeMap(&man, FLOOR);
			man = next_pos;
		}
	}
}

bool login(userinfo& user) {
	int times = 0;
	bool ret = false;
	
	do {
		cout << "请输入用户名: ";
		cin >> user.username;
		cout << "请输入密码: ";
		cin >> user.password;
		ret = fetch_users_info(user);
		times++;
		if (times >= MAX_RETRY_TIMES) {
			break;
		}
		if (ret == false) {
			cout << "登陆失败, 请重新输入!" << endl;
		}
	} while (!ret);

	return ret;
}

void init_game_graph(IMAGE &bg_img) {
	
	//搭台唱戏
	initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
	loadimage(&bg_img, _T("blackground.bmp"), SCREEN_WIDTH, SCREEN_HEIGHT, true);
	putimage(0, 0, &bg_img);

	//加载道具图标
	loadimage(&images[WALL], _T("wall.bmp"), RATIO, RATIO, true);
	loadimage(&images[FLOOR], _T("floor.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX_DES], _T("des.bmp"), RATIO, RATIO, true);
	loadimage(&images[MAN], _T("man.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX], _T("box.bmp"), RATIO, RATIO, true);
	loadimage(&images[HIT], _T("box.bmp"), RATIO, RATIO, true);
}


int main(void) {
	//用户身份验证
	userinfo user;
	levelinfo level;
	bool ret = false;
	IMAGE bg_img;

	if (!login(user)) {
		cout << "登陆失败, 请重新登陆!" << endl;
		system("pause");
		exit(-1);
	}
	else {
		cout << "登陆成功! 用户id: " << user.id << ", 你所在的关卡: " << user.level_id << ", 请开始游戏!" << endl;
		system("pause");
	}

	//初始化游戏地图
	init_game_graph(bg_img);

    //游戏环节
	bool quit = false;

	do {
		//根据用户所在的关卡id获取关卡数据
		ret = fetch_level_info(level, user.level_id);

		if (!ret) {
			cout << "获取关卡信息失败, 请重试!" << endl;
			system("pause");
			exit(-1);
		}

		//把数据库中的地图数据转换到map中
		ret = transfrom_map_db2array(level, map);

		if (!ret) {
			cout << "转换地图失败, 请重试!" << endl;
			system("pause");
			exit(-1);
		}
		
		for (int i = 0; i < level.map_row; i++) {
			for (int j = 0; j < level.map_column; j++) {
				if (map[i][j] == MAN) {
					man.x = i;
					man.y = j;
				}
				putimage(START_X + j * RATIO, START_Y + i * RATIO, &images[map[i][j]]);
			}
		}
		

		do {
			if (_kbhit()) {//玩家按键
				char ch = _getch();

				if (ch == KEY_UP) {
					gameControl(UP);
				}
				else if (ch == KEY_DOWN) {
					gameControl(DOWN);
				}
				else if (ch == KEY_LEFT) {
					gameControl(LEFT);
				}
				else if (ch == KEY_RIGHT) {
					gameControl(RIGHT);
				}
				else if (ch == KEY_QUIT) {
					quit = true;
				}
				if (isGameOver()) {
					if (level.next_level < 1) {
						gameOverScene(&bg_img);
						//update_user_level(user, level.next_level);
						quit = true;
						break;
					}
					gameNextScene(&bg_img);
					//更新用户下一关卡信息
					if (update_user_level(user, level.next_level)) {
						user.level_id = level.next_level;
					}
					
					//quit = true;
				}
			}
			Sleep(100);
		} while (quit == false); //!quit
	} while (quit == false);

	system("pause");
	//游戏结束, 释放资源
	closegraph();
	return 0;
}