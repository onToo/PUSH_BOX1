#pragma once

#include <string>

using namespace std;

#define LINE  48
#define COLUMN 48


//用户信息
typedef struct _userinfo {
	int id;                       //用户id
	string username;    //用户名
	string password;     //密码
	int level_id;             //关卡id
}userinfo;

typedef struct _levelinfo {
	int id;                  //关卡id
	string name;       //关卡的名字
	int map_row;         //地图的总行数
	int map_column;   //地图的总列数
	string map_data;   //二维地图数据
	int next_level;         //下一关卡id
}levelinfo;

bool fetch_users_info(userinfo& user);
bool update_user_level(userinfo& user, int next_level_id);
bool fetch_level_info(levelinfo &level, int level_id);
bool transfrom_map_db2array(levelinfo &level, int map[LINE][COLUMN]);