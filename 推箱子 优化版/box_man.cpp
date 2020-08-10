#include <graphics.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "database.h"
#include "box_man.h"

using namespace std;

struct _POS man;			//С�����ڶ�ά�����е�λ��

/*��Ϸ��ͼ*/
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
*�ж���Ϸ�Ƿ����������������κ�һ������Ŀ�ĵأ��ʹ�����Ϸ���� 
*���룺 �� 
*����ֵ�� 
*           true - ��Ϸ���� false - ��Ϸ���� 
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
 *��Ϸ���������������ͨ�غ���ʾ 
 *���룺 
 * bg - ����ͼƬ������ָ�� 
*����ֵ��
	   �� 
**********************************************/ 
void gameOverScene(IMAGE* bg) {
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	settextstyle(20, 0, _T("����"));
	drawtext(_T("��ϲ��~ \n ͨ����! û����, �ټ�!"), &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void gameNextScene(IMAGE* bg) {
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	settextstyle(20, 0, _T("����"));
	drawtext(_T("��ϲ��~ \n ͨ���ˣ��������ת��һ��"), &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	system("pause");
	cleardevice();
	
}


/***************************************************
*�ı���Ϸ��ͼ��һ���Ӧ���߲�������ʾ
*����:
*		line    - �����ڵ�ͼ��������±�
*		column- - �����ڵ�ͼ��������±�
*		prop    - ���ߵ�����
*	����ֵ:��
***************************************************/
void changeMap(POS* pos, PROPS prop) {
	map[pos->x][pos->y] = prop;
	putimage(START_X + pos->y * RATIO, START_Y + pos->x * RATIO, &images[prop]);
}

/**************************************************
*ʵ����Ϸ�ĸ�����(�ϡ��¡�����)�Ŀ���
*���룺
* direct - ��ǰ������
* ���:��
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
	//��չ�� next_pos.x>=0 && next_pos.x<LINE && next_pos.y>=0 && next_pos.y <COLUMN 
	if (isValid(next_pos) && map[next_pos.x][next_pos.y] == FLOOR) {// �˵�ǰ���ǵذ� 
		changeMap(&next_pos, MAN); //С��ǰ��һ�� 
		changeMap(&man, FLOOR);
		man = next_pos;
	}
	else if (isValid(next_next_pos) && map[next_pos.x][next_pos.y] == BOX) {//�˵�ǰ�������� 
   //�������������ǰ���ǵذ����������Ŀ�ĵ� 
		if (map[next_next_pos.x][next_next_pos.y] == FLOOR) {
			changeMap(&next_next_pos, BOX);
			changeMap(&next_pos, MAN); //С��ǰ��һ��  
			changeMap(&man, FLOOR); 
			man = next_pos;
		}
		else if (map[next_next_pos.x][next_next_pos.y] == BOX_DES) {
			changeMap(&next_next_pos, HIT);
			changeMap(&next_pos, MAN); //С��ǰ��һ��
			changeMap(&man, FLOOR);
			man = next_pos;
		}
	}
}

bool login(userinfo& user) {
	int times = 0;
	bool ret = false;
	
	do {
		cout << "�������û���: ";
		cin >> user.username;
		cout << "����������: ";
		cin >> user.password;
		ret = fetch_users_info(user);
		times++;
		if (times >= MAX_RETRY_TIMES) {
			break;
		}
		if (ret == false) {
			cout << "��½ʧ��, ����������!" << endl;
		}
	} while (!ret);

	return ret;
}

void init_game_graph(IMAGE &bg_img) {
	
	//��̨��Ϸ
	initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
	loadimage(&bg_img, _T("blackground.bmp"), SCREEN_WIDTH, SCREEN_HEIGHT, true);
	putimage(0, 0, &bg_img);

	//���ص���ͼ��
	loadimage(&images[WALL], _T("wall.bmp"), RATIO, RATIO, true);
	loadimage(&images[FLOOR], _T("floor.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX_DES], _T("des.bmp"), RATIO, RATIO, true);
	loadimage(&images[MAN], _T("man.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX], _T("box.bmp"), RATIO, RATIO, true);
	loadimage(&images[HIT], _T("box.bmp"), RATIO, RATIO, true);
}


int main(void) {
	//�û������֤
	userinfo user;
	levelinfo level;
	bool ret = false;
	IMAGE bg_img;

	if (!login(user)) {
		cout << "��½ʧ��, �����µ�½!" << endl;
		system("pause");
		exit(-1);
	}
	else {
		cout << "��½�ɹ�! �û�id: " << user.id << ", �����ڵĹؿ�: " << user.level_id << ", �뿪ʼ��Ϸ!" << endl;
		system("pause");
	}

	//��ʼ����Ϸ��ͼ
	init_game_graph(bg_img);

    //��Ϸ����
	bool quit = false;

	do {
		//�����û����ڵĹؿ�id��ȡ�ؿ�����
		ret = fetch_level_info(level, user.level_id);

		if (!ret) {
			cout << "��ȡ�ؿ���Ϣʧ��, ������!" << endl;
			system("pause");
			exit(-1);
		}

		//�����ݿ��еĵ�ͼ����ת����map��
		ret = transfrom_map_db2array(level, map);

		if (!ret) {
			cout << "ת����ͼʧ��, ������!" << endl;
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
			if (_kbhit()) {//��Ұ���
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
					//�����û���һ�ؿ���Ϣ
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
	//��Ϸ����, �ͷ���Դ
	closegraph();
	return 0;
}