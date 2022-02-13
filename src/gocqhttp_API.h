#pragma once
#include"gocqhttp_err.h"

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef ZERO(X)
#define ZERO(X)		memset(X,0,sizeof(X))
#endif // !ZERO(X)


#define RECV_MAX		100			//最大接收次数

/*初始化*/
cqhttp_err init_gocqhttpAPI(const char* ip, const int port);

/*退出*/
void exit_gocqhttpAPI(void);

/////////////////////////////////
/*send_private_msg 发送私密消息*/
/////////////////////////////////

#define API_SEND_PRIVATE_MSG_FORM	"GET /send_private_msg?user_id=%u&group_id=%u&message=%s&auto_escape=%d HTTP/1.1\r\nHost: 127.0.0.1:5700\r\nConnection: keep-alive\r\n\r\n"
#define API_SEND_PRIVATE_MSG_RECV	"%*[^{]{\"data\":{\"message_id\":%d},\"retcode\":%d,\"status\":\"%[^\"]\""

typedef struct
{
	unsigned long user_id;	//对方 QQ 号
	unsigned long group_id;	//主动发起临时会话群号(机器人本身必须是管理员/群主)
	char message[1024];		//要发送的内容
	int auto_escape;		//消息内容是否作为纯文本发送，只在 message 字段是字符串时有效(默认值 false）
}send_private_msg_s;	//发送消息包

typedef struct
{
	struct
	{
		int message_id;		//消息ID
	}data;	//返回数据
	int retcode;			//返回码
	char status[10];		//状态
}send_private_msg_r;	//接收消息包

typedef union
{
	send_private_msg_s send_msg;	//发包
	send_private_msg_r recv_msg;	//收包
}send_private_msg_data;//组合包

//API
cqhttp_err send_private_msg(
	send_private_msg_data* data				//发包
);

//获取发包
send_private_msg_data New_send_private_msg(
	unsigned long user_id,					//用户ID
	unsigned long group_id,					//群号
	char message[1024],						//消息
	int auto_escape							//是否纯文本
);

/////////////////////////////
/*send_group_msg 发送群消息*/
/////////////////////////////

#define API_SEND_GROUP_MSG_FORM		"GET /send_group_msg?group_id=%ld&message=%s&auto_escape=%d HTTP/1.1\r\nHost: 127.0.0.1:5700\r\nConnection: keep-alive\r\n\r\n"
#define API_SEND_GROUP_MSG_RECV		"%*[^{]{\"data\":{\"message_id\":%d},\"retcode\":%d,\"status\":\"%[^\"]\"}"

typedef struct
{
	unsigned long group_id;	//群号
	char message[1024];		//要发送的内容
	int auto_escape;		//消息内容是否作为纯文本发送，只在 message 字段是字符串时有效(默认值 false）
}send_group_msg_s;  //发送消息包

typedef struct
{
	struct
	{
		int message_id;		//消息ID
	}data;	//返回数据
	int retcode;			//返回码
	char status[10];		//状态
}send_group_msg_r;  //接收消息包

typedef union
{
	send_group_msg_s send_msg;		//发包
	send_group_msg_r recv_msg;		//收包
}send_group_msg_data;//组合包

//API
cqhttp_err send_group_msg(
	send_group_msg_data* data				//发包
);

//获取发包
send_group_msg_data New_send_group_msg(
	unsigned long group_id,					//群号
	char message[1024],						//消息
	int auto_escape							//是否纯文本
);

////////////
/*获取消息*/
////////////

#define API_GET_MSG_FORM			"GET /get_msg?message_id=%d HTTP/1.1\r\nHost: 127.0.0.1:5700\r\nConnection: keep-alive\r\n\r\n"
#define API_GET_MSG_RECV			"%*[^{]{\"data\":{\"group\":%[^,],\"group_id\":%lu,\"message\":\"%[^\"]\",\"message_id\":%d,\"message_id_v2\":\"%[^\"]\",\"message_seq\":%d,\"message_type\":\"%[^\"]\",\"real_id\":%d,\"sender\":{\"nickname\":\"%[^\"]\",\"user_id\":%lu},\"time\":%d},\"retcode\":%d,\"status\":\"%[^\"]\"}"

typedef struct
{
	int message_id;	//消息 ID
}get_msg_s;			//发送消息包

typedef struct
{
	struct
	{
		char group[10];				//我也不清楚，大概也用不到
		unsigned long group_id;		//群号
		int message_id;				//消息 ID
		char message_id_v2[50];		//消息 ID 2.0版
		int real_id;				//真实 ID
		int message_seq;			//我也不知道是干啥的
		char message_type[20];		//消息类型
		struct
		{
			char nickname[100];		//昵称
			unsigned long user_id;	//用户QQ号
		}sender;		//发送者
		int time;					//发送时间
		char message[1024];			//消息类容
	}data;			//返回数据
	int retcode;					//返回码
	char status[10];				//状态
}get_msg_r;			//接收消息包

typedef union
{
	get_msg_s send_msg;
	get_msg_r recv_msg;
}get_msg_data;			//组合包

//API
cqhttp_err get_msg(
	get_msg_data* data						//发包
);

//获取发包
get_msg_data New_get_msg(
	int message_id							//消息ID
);

///////////////////////
/*delete_msg 撤回消息*/
///////////////////////

#define API_DELETE_MSG_FORM			"GET /delete_msg?message_id=%d HTTP/1.1\r\nHost: 127.0.0.1:5700\r\nConnection: keep-alive\r\n\r\n"
#define API_DELETE_MSG_RECV			"%*[^{]{\"data\":%[^,],\"retcode\":%d,\"status\":\"%[^\"]\"}"

typedef struct
{
	int message_id;	//消息 ID
}delete_msg_s;		//发送消息包

typedef struct
{
	char data[10];			//返回数据
	int retcode;			//返回码
	char status[10];		//状态
}delete_msg_r;		//接收消息包

typedef union
{
	delete_msg_s send_msg;			//发包
	delete_msg_r recv_msg;			//收包
}delete_msg_data;		//组合包

//API
cqhttp_err delete_msg(
	delete_msg_data* data					//发包
);

//获取发包
delete_msg_data New_delete_msg(
	int message_id							//消息ID
);

//////////////////
/*获取群成员信息*/
//////////////////

#define API_GET_GROUP_MEMBER_INFO_FORM	"GET /get_group_member_info?group_id=%lu&user_id=%lu&no_cache=%d HTTP/1.1\r\nHost: 127.0.0.1:5700\r\nConnection: keep-alive\r\n\r\n"
#define API_GET_GROUP_MEMBER_INFO_RECV	"%*[^{]{\"data\":{\"age\":%d,\"area\":%[^,],\"card\":\"%[^\"]\",\"card_changeable\":%[^,],\"group_id\":%lu,\"join_time\":%d,\"last_sent_time\":%d,\"level\":\"%[^\"]\",\"nickname\":\"%[^\"]\",\"role\":\"%[^\"]\",\"sex\":\"%[^\"]\",\"shut_up_timestamp\":%d,\"title\":%[^,],\"title_expire_time\":%lu,\"unfriendly\":%[^,],\"user_id\":%lu},\"retcode\":%d,\"status\":\"%[^\"]\"}"

typedef struct
{
	unsigned long group_id;	//群号
	unsigned long user_id;	//QQ号
	int no_cache;			//是否不使用缓存（使用缓存可能更新不及时, 但响应更快）
}get_group_member_info_s;		//发送消息包

typedef struct
{
	struct
	{
		unsigned long group_id;	//群号
		unsigned long user_id;	//QQ号
		char nickname[50];		//昵称
		char card[50];			//群名片/备注
		char sex[10];			//性别
		int age;				//年龄
		char area[200];			//地区
		int join_time;			//加群时间戳
		int last_sent_time;		//最后发言时间戳
		char level[50];			//成员等级
		char role[10];			//角色
		char unfriendly[10];	//是否不良记录成员
		char title[50];			//专属头衔
		long title_expire_time;	//专属头衔过期时间戳
		char card_changeable[10];//是否允许修改群名片
		int shut_up_timestamp;
	}data;			//返回数据
	int retcode;					//返回码
	char status[10];				//状态
}get_group_member_info_r;		//接收消息包

typedef union
{
	get_group_member_info_s send_msg;
	get_group_member_info_r recv_msg;
}get_group_member_info_data;	 //组合包

//API
cqhttp_err get_group_member_info(
	get_group_member_info_data* data
);

//获取发包
get_group_member_info_data New_get_group_member_info(
	unsigned long group_id,					//群号
	unsigned long user_id,					//QQ号
	int no_cache							//是否不使用缓存（使用缓存可能更新不及时, 但响应更快）
);