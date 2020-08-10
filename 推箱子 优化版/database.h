#pragma once

#include <string>

using namespace std;

#define LINE  48
#define COLUMN 48


//�û���Ϣ
typedef struct _userinfo {
	int id;                       //�û�id
	string username;    //�û���
	string password;     //����
	int level_id;             //�ؿ�id
}userinfo;

typedef struct _levelinfo {
	int id;                  //�ؿ�id
	string name;       //�ؿ�������
	int map_row;         //��ͼ��������
	int map_column;   //��ͼ��������
	string map_data;   //��ά��ͼ����
	int next_level;         //��һ�ؿ�id
}levelinfo;

bool fetch_users_info(userinfo& user);
bool update_user_level(userinfo& user, int next_level_id);
bool fetch_level_info(levelinfo &level, int level_id);
bool transfrom_map_db2array(levelinfo &level, int map[LINE][COLUMN]);