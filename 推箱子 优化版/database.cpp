#include "database.h"
#include <iostream>
#include <mysql.h>


#define DB_NAME  "box_man"
#define DB_HOST   "127.0.0.1"
#define DB_PORT    3306
#define DB_USER    "root"
#define DB_USER_PASSWORD  "liu1599zxc"

static int debug = 1;

static bool connect_db(MYSQL& mysql);

/***********************
*功能: 验证用户名和密码获取信息
*输入:
*        user - 用户信息 结构体
*返回值:
*          获取成功返回true, 失败 false
****************************/
bool fetch_users_info(userinfo& user) {
    MYSQL mysql;
    MYSQL_RES* res;    //查询结果集
    MYSQL_ROW row;  //记录结构体
    char sql[256];
    bool ret = false;

    //1.连接数据库
    if (connect_db(mysql) == false) {
        return false;
    }
    //2.根据用户名和密码获取用户信息(id, level_id)
    snprintf(sql, 256, "select id, level_id from users where usertname = '%s' and password = md5('%s'); ", user.username.c_str(), user.password.c_str());
    ret = mysql_query(&mysql, sql);   //成功返回0

    if (ret) {
        //cout << "数据库连接失败, 错误原因: \n" << mysql_error(&mysql);
        printf("数据库连接失败, 错误原因: %s\n", mysql_error(&mysql));
        mysql_close(&mysql);//关闭数据库
        return false;
    }

    //3.获取结果
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);

    if (row == NULL) {//没有查找到记录
        mysql_free_result(res);     //释放数据库
        mysql_close(&mysql);
        return false;
    }

    user.id = atoi(row[0]);
    user.level_id = atoi(row[1]);
    if(debug)cout << "userid: " << user.id << " level_id: " << user.level_id << endl;

   
    mysql_free_result(res);     //释放结果集
    mysql_close(&mysql);
    //4.返回结果
    return true;
}

bool connect_db(MYSQL& mysql) {
    //1.初始化数据库句柄
    mysql_init(&mysql);

    //2.设置字符编码
    mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");

    //3.连接数据库
    if (mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_USER_PASSWORD, DB_NAME, DB_PORT, NULL, 0) == NULL) {
        //cout << "数据库连接失败, 错误原因: \n" << mysql_error(&mysql);
        printf("数据库连接失败, 错误原因: %s\n", mysql_error(&mysql));
        return false;
    }
    return true;
}

/********************************
*功能: 根据关卡id获取完整的关卡信息(如: 地图, 下一关卡
*输入:
*         level - 保存关卡信息的结构体变量
*         leve_id - 要获取详细关卡信息的id
*返回值:
*          获取成功返回true, 失败 false
********************************/

bool fetch_level_info(levelinfo& level, int level_id) {
    MYSQL mysql;
    MYSQL_RES* res;    //查询结果集
    MYSQL_ROW row;  //记录结构体
    char sql[256];
    bool ret = false;

    //1.连接数据库
    if (connect_db(mysql) == false) {
        return false;
    }
    
    //2.根据关卡id查询数据库获取关卡地图信息
    snprintf(sql, 256, "select name, map_row, map_column, map_data, next_level_id from levels where id = %d;", level_id);
    ret = mysql_query(&mysql, sql);   //成功返回0

    if (ret) {
        //cout << "数据库连接失败, 错误原因: \n" << mysql_error(&mysql);
        printf("数据库连接失败, 错误原因: %s\n", mysql_error(&mysql));
        mysql_close(&mysql);//关闭数据库
        return false;
    }

    //3.获取结果
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);

    if (row == NULL) {//没有查找到记录
        mysql_free_result(res);     //释放数据库
        mysql_close(&mysql);
        return false;
    }

    level.id = level_id;
    level.name = row[0];
    level.map_row = atoi(row[1]);
    level.map_column = atoi(row[2]);
    level.map_data = row[3];
    level.next_level = atoi(row[4]);

    if(debug)printf("level id: %d   name:  %s  map row:  %d    map column:  %d   map data:  %s   next level:  %d\n", level.id, level.name.c_str(), level.map_row, level.map_column, level.map_data.c_str(), level.next_level);
    //释放结果集
    mysql_free_result(res);   
    //关闭数据库
    mysql_close(&mysql);

    return true;
}


bool transfrom_map_db2array(levelinfo& level, int map[LINE][COLUMN]) {
    if (level.map_row > LINE || level.map_column > COLUMN) {
        cout << "地图过大, 请重新设置!" << endl;
        return false;
    }
    if (level.map_data.length() < 1) {
        cout << "地图数据错误, 请重新设置!" << endl;
        return false;
    }

    int start = 0, end = 0;
    int row = 0, column = 0;

    do {
        end = level.map_data.find('|', start);
        if (end < 0) {
            end = level.map_data.length();
        }
        if (start >= end)break;

        string line = level.map_data.substr(start, end - start);
        printf("get line: %s\n", line.c_str());

        //对行数据进行解析
        char* next_token = NULL;
        char* item = strtok_s((char*)line.c_str(), ",", &next_token);

        column = 0;

        while (item && column < level.map_column) {
            printf("%s ", item);
            map[row][column] = atoi(item);
            column++;
            item = strtok_s(NULL, ",", &next_token);
        }

        if (column < level.map_column) {
            cout << "地图数据解析出错, 终止!" << endl;
            return false;
        }

        cout << endl;
        row++;
        if (row >= level.map_row) {
            break;
        }


        start = end + 1;
    } while (1 == 1);

    if (row < level.map_row) {
        cout << "地图行数少于设定, " << row << "(need:" << level.map_row << "), 终止!" << endl;
        return false;
    }

    return true;
}

// 0,0,0,0,0,0,0,0,0,0,0,0|0,1,0,1,1,1,1,1,1,1,0,0|0,1,4,1,0,2,1,0,2,1,1,0|0,1,0,1,0,1,0,0,1,1,1,0|0,1,0,2,0,1,1,4,1,1,1,0|0,1,1,1,1,3,1,1,1,4,1,0|0,1,2,1,1,4,1,1,1,1,1,0|0,1,0,0,1,0,1,1,0,0,1,0|0,0,0,0,0,0,0,0,0,0,0,0

/********************************
*功能: 更新用户下一关的地图数据, 跳转
*输入:
*          
*       
*        
*返回值:
*          获取成功返回true, 失败 false
********************************/
bool update_user_level(userinfo& user, int next_level_id) {
    MYSQL mysql;
    MYSQL_RES* res;    //查询结果集
    MYSQL_ROW row;  //记录结构体
    char sql[256];
    bool ret = false;

    //1.连接数据库
    if (connect_db(mysql) == false) {
        return false;
    }

    //2.根据用户id更新下一卡level_id
    snprintf(sql, 256, "update users set level_id = %d where id = %d;", next_level_id, user.id);

    ret = mysql_query(&mysql, sql);
    if (ret) {
        printf("数据库更新出错, %s  错误原因: %s\n", sql, mysql_errno(&mysql));
        mysql_close(&mysql); //关闭数据库
        return false;
    }

    //关闭数据库
    mysql_close(&mysql);

    return true;

}