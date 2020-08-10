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
*����: ��֤�û����������ȡ��Ϣ
*����:
*        user - �û���Ϣ �ṹ��
*����ֵ:
*          ��ȡ�ɹ�����true, ʧ�� false
****************************/
bool fetch_users_info(userinfo& user) {
    MYSQL mysql;
    MYSQL_RES* res;    //��ѯ�����
    MYSQL_ROW row;  //��¼�ṹ��
    char sql[256];
    bool ret = false;

    //1.�������ݿ�
    if (connect_db(mysql) == false) {
        return false;
    }
    //2.�����û����������ȡ�û���Ϣ(id, level_id)
    snprintf(sql, 256, "select id, level_id from users where usertname = '%s' and password = md5('%s'); ", user.username.c_str(), user.password.c_str());
    ret = mysql_query(&mysql, sql);   //�ɹ�����0

    if (ret) {
        //cout << "���ݿ�����ʧ��, ����ԭ��: \n" << mysql_error(&mysql);
        printf("���ݿ�����ʧ��, ����ԭ��: %s\n", mysql_error(&mysql));
        mysql_close(&mysql);//�ر����ݿ�
        return false;
    }

    //3.��ȡ���
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);

    if (row == NULL) {//û�в��ҵ���¼
        mysql_free_result(res);     //�ͷ����ݿ�
        mysql_close(&mysql);
        return false;
    }

    user.id = atoi(row[0]);
    user.level_id = atoi(row[1]);
    if(debug)cout << "userid: " << user.id << " level_id: " << user.level_id << endl;

   
    mysql_free_result(res);     //�ͷŽ����
    mysql_close(&mysql);
    //4.���ؽ��
    return true;
}

bool connect_db(MYSQL& mysql) {
    //1.��ʼ�����ݿ���
    mysql_init(&mysql);

    //2.�����ַ�����
    mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");

    //3.�������ݿ�
    if (mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_USER_PASSWORD, DB_NAME, DB_PORT, NULL, 0) == NULL) {
        //cout << "���ݿ�����ʧ��, ����ԭ��: \n" << mysql_error(&mysql);
        printf("���ݿ�����ʧ��, ����ԭ��: %s\n", mysql_error(&mysql));
        return false;
    }
    return true;
}

/********************************
*����: ���ݹؿ�id��ȡ�����Ĺؿ���Ϣ(��: ��ͼ, ��һ�ؿ�
*����:
*         level - ����ؿ���Ϣ�Ľṹ�����
*         leve_id - Ҫ��ȡ��ϸ�ؿ���Ϣ��id
*����ֵ:
*          ��ȡ�ɹ�����true, ʧ�� false
********************************/

bool fetch_level_info(levelinfo& level, int level_id) {
    MYSQL mysql;
    MYSQL_RES* res;    //��ѯ�����
    MYSQL_ROW row;  //��¼�ṹ��
    char sql[256];
    bool ret = false;

    //1.�������ݿ�
    if (connect_db(mysql) == false) {
        return false;
    }
    
    //2.���ݹؿ�id��ѯ���ݿ��ȡ�ؿ���ͼ��Ϣ
    snprintf(sql, 256, "select name, map_row, map_column, map_data, next_level_id from levels where id = %d;", level_id);
    ret = mysql_query(&mysql, sql);   //�ɹ�����0

    if (ret) {
        //cout << "���ݿ�����ʧ��, ����ԭ��: \n" << mysql_error(&mysql);
        printf("���ݿ�����ʧ��, ����ԭ��: %s\n", mysql_error(&mysql));
        mysql_close(&mysql);//�ر����ݿ�
        return false;
    }

    //3.��ȡ���
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);

    if (row == NULL) {//û�в��ҵ���¼
        mysql_free_result(res);     //�ͷ����ݿ�
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
    //�ͷŽ����
    mysql_free_result(res);   
    //�ر����ݿ�
    mysql_close(&mysql);

    return true;
}


bool transfrom_map_db2array(levelinfo& level, int map[LINE][COLUMN]) {
    if (level.map_row > LINE || level.map_column > COLUMN) {
        cout << "��ͼ����, ����������!" << endl;
        return false;
    }
    if (level.map_data.length() < 1) {
        cout << "��ͼ���ݴ���, ����������!" << endl;
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

        //�������ݽ��н���
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
            cout << "��ͼ���ݽ�������, ��ֹ!" << endl;
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
        cout << "��ͼ���������趨, " << row << "(need:" << level.map_row << "), ��ֹ!" << endl;
        return false;
    }

    return true;
}

// 0,0,0,0,0,0,0,0,0,0,0,0|0,1,0,1,1,1,1,1,1,1,0,0|0,1,4,1,0,2,1,0,2,1,1,0|0,1,0,1,0,1,0,0,1,1,1,0|0,1,0,2,0,1,1,4,1,1,1,0|0,1,1,1,1,3,1,1,1,4,1,0|0,1,2,1,1,4,1,1,1,1,1,0|0,1,0,0,1,0,1,1,0,0,1,0|0,0,0,0,0,0,0,0,0,0,0,0

/********************************
*����: �����û���һ�صĵ�ͼ����, ��ת
*����:
*          
*       
*        
*����ֵ:
*          ��ȡ�ɹ�����true, ʧ�� false
********************************/
bool update_user_level(userinfo& user, int next_level_id) {
    MYSQL mysql;
    MYSQL_RES* res;    //��ѯ�����
    MYSQL_ROW row;  //��¼�ṹ��
    char sql[256];
    bool ret = false;

    //1.�������ݿ�
    if (connect_db(mysql) == false) {
        return false;
    }

    //2.�����û�id������һ��level_id
    snprintf(sql, 256, "update users set level_id = %d where id = %d;", next_level_id, user.id);

    ret = mysql_query(&mysql, sql);
    if (ret) {
        printf("���ݿ���³���, %s  ����ԭ��: %s\n", sql, mysql_errno(&mysql));
        mysql_close(&mysql); //�ر����ݿ�
        return false;
    }

    //�ر����ݿ�
    mysql_close(&mysql);

    return true;

}