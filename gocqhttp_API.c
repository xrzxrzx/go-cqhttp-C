#include"gocqhttp_API.h"
#include"URLcode.h"
#include"Log.h"
#include"AnaJSON.h"
#include"Tool.h"
#include"AnaJSON.h"
#include<process.h>
#include<winsock2.h>
#include<windows.h>
#include<WS2tcpip.h>
#include<stdio.h>
#include<string.h>
#pragma comment(lib, "ws2_32.lib")

#define MESSAGE_DEFAULT_SIZE 1024

/* socket */
static SOCKADDR_IN	server_addr;//服务端

/*API连接*/
static SOCKET APIconection();

/*send_private_msg 发送私密消息*/
send_private_msg_data New_send_private_msg(unsigned int user_id, unsigned int group_id, char message[1024], int auto_escape)
{
	char* temp = GBKtoUTF8(message);
	send_private_msg_data data;
	memset(&data, 0, sizeof(data));
	data.send_msg.user_id = user_id;
	data.send_msg.group_id = group_id;
	strcpy_s(data.send_msg.message, MESSAGE_DEFAULT_SIZE, temp);
	data.send_msg.auto_escape = auto_escape;
	free(temp);
	return data;
}

void send_private_msg(send_private_msg_data* data)
{
	SOCKET server;
	if ((server = APIconection()) == 0)	//对接API
	{
		logErrAll("API连接错误");
		return;
	}

	char rmsg[1024] = { '\0' };							//收包
	char smsg[1024] = { '\0' };							//发包
	char* temp;

	//构建URL
	temp = urlencode(data->send_msg.message);

	//构建发包（字符串）
	if (
		sprintf_s(smsg, MESSAGE_DEFAULT_SIZE,
		API_SEND_PRIVATE_MSG_FORM,
		data->send_msg.user_id,
		data->send_msg.group_id,
		temp,
		data->send_msg.auto_escape)
		< 0)
		logErrAll("构建发包失败\n待发送消息：%s", data->send_msg.message);
	free(temp);
	int isend = send(server, smsg, strlen(smsg), 0);	//发送
	if (isend < 0)
	{
		closesocket(server);
		logErrAll("发送 %s 失败", smsg);
		return;
	}
	//logInfo("发送 %s 成功", data->send_msg.message);

	memset(data, 0, sizeof(send_private_msg_data));
	int count = 0;
	while (recv(server, rmsg, sizeof(rmsg), 0) < 0)		//接收
	{
		count++;
		if (count > RECV_MAX)
		{
			closesocket(server);
			logWarnAll("发送 %s 后连接中断", smsg);
			return;
		}
	}
	JSON* recvJSON;
	char* jsonStr;
	jsonStr = removeHeaders(rmsg);
	recvJSON = StringToJSON(jsonStr);
	
	getIntValue(&data->recv_msg.retcode, recvJSON, "retconde");
	getStringValue(data->recv_msg.status, recvJSON, "status");

	free(jsonStr);
	FreeJSON(recvJSON);
	closesocket(server);
}

/*send_group_msg 发送群消息*/
send_group_msg_data New_send_group_msg(unsigned int group_id, char message[1024], int auto_escape)
{
	char* temp = GBKtoUTF8(message);
	send_group_msg_data data;
	memset(&data, 0, sizeof(data));
	data.send_msg.group_id = group_id;
	strcpy_s(data.send_msg.message, sizeof(data.send_msg.message), temp);
	data.send_msg.auto_escape = auto_escape;
	free(temp);
	return data;
}

void send_group_msg(send_group_msg_data* data)
{
	SOCKET server;
	if ((server = APIconection()) == 0)	//对接API
	{
		logErrAll("API连接错误");
		return;
	}

	char rmsg[1024] = { '\0' };			//收包
	char smsg[1024] = { '\0' };			//发包
	char* temp;

	//构建URL
	temp = urlencode(data->send_msg.message);

	//构建发包
	if (
		sprintf_s(smsg, MESSAGE_DEFAULT_SIZE,
			API_SEND_GROUP_MSG_FORM,
			data->send_msg.group_id,
			temp,
			data->send_msg.auto_escape)
		< 0)
		logErrAll("构建发包失败：%s", data->send_msg.message);
	free(temp);
	int isend = send(server, smsg, strlen(smsg), 0);	//发送
	if (isend < 0)
	{
		closesocket(server);
		logErrAll("发送 %s 失败", smsg);
		return;
	}
	//logInfo("发送 %s 成功", data->send_msg.message);

	memset(data, 0, sizeof(send_group_msg_data));
	int count = 0;		//接收次数
	while (recv(server, rmsg, sizeof(rmsg), 0) < 0)		//接收
	{
		count++;
		if (count > RECV_MAX)
		{
			closesocket(server);
			logWarnAll("发送 %s 后连接中断", smsg);
			return;
		}
	}
	JSON* recvJSON;
	char* jsonStr;
	jsonStr = removeHeaders(rmsg);
	recvJSON = StringToJSON(jsonStr);

	getIntValue(&data->recv_msg.retcode, recvJSON, "retconde");
	getStringValue(data->recv_msg.status, recvJSON, "status");

	free(jsonStr);
	FreeJSON(recvJSON);
	closesocket(server);
}

/*get_msg 获取消息*/
get_msg_data New_get_msg(int message_id)
{
	get_msg_data data;
	memset(&data, 0, sizeof(data));
	data.send_msg.message_id = message_id;
	return data;
}

void get_msg(get_msg_data* data)
{
	SOCKET server;
	if ((server = APIconection()) == 0)	//对接API
	{
		logErrAll("API连接错误");
		return;
	}

	char rmsg[1024] = { '\0' };							//收包
	char smsg[1024] = { '\0' };							//发包
	char tempStr[1024] = { '\0' };
	char* tempPtr;

	//构建发包
	if (
		sprintf_s(smsg, MESSAGE_DEFAULT_SIZE,
			API_GET_MSG_FORM,
			data->send_msg.message_id)
		< 0)
		logErrAll("构建发包失败，消息ID：%d", data->send_msg.message_id);
	
	int isend = send(server, smsg, strlen(smsg), 0);	//发送
	if (isend < 0)
	{
		closesocket(server);
		logErrAll("发送 %s 失败", smsg);
		return;
	}

	memset(data, 0, sizeof(get_msg_data));
	while (recv(server, rmsg, sizeof(rmsg), 0) < 0);	//接收

	JSON* recvJSON, subJSON, subJSON2;
	char* jsonStr;
	jsonStr = removeHeaders(rmsg);
	recvJSON = StringToJSON(jsonStr);

	getIntValue(&data->recv_msg.retcode, recvJSON, "retconde");
	getStringValue(data->recv_msg.status, recvJSON, "status");
	getJSONValue(&subJSON, recvJSON, "data");
	getBoolValue(&data->recv_msg.data.group, &subJSON, "group");
	getIntValue(&data->recv_msg.data.group_id, &subJSON, "group_id");
	getIntValue(&data->recv_msg.data.message_id, &subJSON, "message_id");
	getStringValue(tempStr, &subJSON, "message");
	tempPtr = UTF8toGBK(tempStr);
	strcpy_s(data->recv_msg.data.message, 50, tempPtr);
	memset(tempStr, 0, sizeof(tempStr));
	free(tempPtr);
	getStringValue(data->recv_msg.data.message_id_v2, &subJSON, "message_id_v2");
	getIntValue(&data->recv_msg.data.message_seq, &subJSON, "message_seq");
	getIntValue(&data->recv_msg.data.time, &subJSON, "time");
	getIntValue(&data->recv_msg.data.real_id, &subJSON, "real_id");
	getStringValue(data->recv_msg.data.message_type, &subJSON, "message_type");
	getJSONValue(&subJSON2, &subJSON, "sender");
	getStringValue(tempStr, &subJSON2, "nickname");
	tempPtr = UTF8toGBK(tempStr);
	strcpy_s(data->recv_msg.data.message, 50, tempPtr);
	memset(tempStr, 0, sizeof(tempStr));
	free(tempPtr);
	getIntValue(&data->recv_msg.data.sender.user_id, &subJSON2, "user_id");

	FreeJSON(recvJSON);
	free(jsonStr);

	closesocket(server);
	logInfo("获取消息信息成功");
}

/*delete_msg 撤回消息*/
delete_msg_data New_delete_msg(int message_id)
{
	delete_msg_data data;
	memset(&data, 0, sizeof(data));
	data.send_msg.message_id = message_id;
	return data;
}

void delete_msg(delete_msg_data* data)
{
	SOCKET server;
	if ((server = APIconection()) == 0)	//对接API
	{
		logErrAll("API连接错误");
		return;
	}

	char rmsg[1024] = { '\0' };							//收包
	char smsg[1024] = { '\0' };							//发包

	//构建发包
	if (
		sprintf_s(smsg, MESSAGE_DEFAULT_SIZE,
			API_DELETE_MSG_FORM,
			data->send_msg.message_id)
		< 0)
		logErrAll("构建发包失败，消息ID：%d", data->send_msg.message_id);

	int isend = send(server, smsg, strlen(smsg), 0);	//发送
	if (isend < 0)
	{
		closesocket(server);
		logErrAll("发送 %s 失败", smsg);
		return;
	}

	memset(data, 0, sizeof(data));
	while (recv(server, rmsg, sizeof(rmsg), 0) < 0);	//接收
	scanf_s(rmsg, API_DELETE_MSG_RECV,
		data->recv_msg.data,
		&data->recv_msg.retcode,
		data->recv_msg.status);

	logInfoAll("%s", rmsg);
	JSON* recvJSON;
	char* jsonStr;
	jsonStr = removeHeaders(rmsg);
	recvJSON = StringToJSON(jsonStr);

	getIntValue(&data->recv_msg.retcode, recvJSON, "retconde");
	getStringValue(data->recv_msg.status, recvJSON, "status");

	free(jsonStr);
	FreeJSON(recvJSON);
	closesocket(server);
	logInfo("已成功撤回消息");
}

/*get_group_member_info 获取群成员信息*/
get_group_member_info_data New_get_group_member_info(unsigned long group_id, unsigned long user_id, int no_cache)
{
	get_group_member_info_data data;
	memset(&data, 0, sizeof(data));
	data.send_msg.group_id = group_id;
	data.send_msg.user_id = user_id;
	data.send_msg.no_cache = no_cache;
	return data;
}

void get_group_member_info(get_group_member_info_data* data)
{
	SOCKET server;
	if ((server = APIconection()) == 0)	//对接API
	{
		logErrAll("API连接错误");
		return;
	}

	char rmsg[1024] = { '\0' };							//收包
	char smsg[1024] = { '\0' };							//发包
	char tempStr[1024] = { '\0' };
	char* tempPtr;

	//构建发包
	if (
		sprintf_s(smsg, MESSAGE_DEFAULT_SIZE,
			API_GET_GROUP_MEMBER_INFO_FORM,
			data->send_msg.group_id,
			data->send_msg.user_id,
			data->send_msg.no_cache)
		< 0)
		logErrAll("构建发包失败，群ID：%d", data->send_msg.group_id);

	int isend = send(server, smsg, strlen(smsg), 0);	//发送
	if (isend < 0)
	{
		closesocket(server);
		logErrAll("发送 %s 失败", smsg);
		return;
	}

	memset(data, 0, sizeof(get_group_member_info_data));
	while (recv(server, rmsg, sizeof(rmsg), 0) < 0);	//接收
	logInfoAll("%s", rmsg);
	JSON* recvJSON, subJSON;
	char* jsonStr;
	jsonStr = removeHeaders(rmsg);
	recvJSON = StringToJSON(jsonStr);

	getIntValue(&data->recv_msg.retcode, recvJSON, "retconde");
	getStringValue(data->recv_msg.status, recvJSON, "status");
	getJSONValue(&subJSON, recvJSON, "data");
	getIntValue(&data->recv_msg.data.age, &subJSON, "age");
	getIntValue(&data->recv_msg.data.group_id, &subJSON, "group_id");
	getIntValue(&data->recv_msg.data.user_id, &subJSON, "user_id");
	getStringValue(data->recv_msg.data.area, &subJSON, "area");
	getIntValue(&data->recv_msg.data.join_time, &subJSON, "json_time");
	getStringValue(data->recv_msg.data.card, &subJSON, "card");
	getBoolValue(&data->recv_msg.data.card_changeable, &subJSON, "card_changeable");
	getIntValue(&data->recv_msg.data.last_sent_time, &subJSON, "last_sent_time");
	getStringValue(data->recv_msg.data.level, &subJSON, "level");
	getStringValue(tempStr, &subJSON, "nickname");
	tempPtr = UTF8toGBK(tempStr);
	strcpy_s(data->recv_msg.data.nickname, 50, tempPtr);
	memset(tempStr, 0, sizeof(tempStr));
	free(tempPtr);
	getStringValue(data->recv_msg.data.role, &subJSON, "role");
	getStringValue(data->recv_msg.data.sex, &subJSON, "sex");
	getIntValue(&data->recv_msg.data.shut_up_timestamp, &subJSON, "shut_up_timestamp");
	getStringValue(data->recv_msg.data.title, &subJSON, "title");
	getIntValue(&data->recv_msg.data.title_expire_time, &subJSON, "title_expire_time");
	getBoolValue(&data->recv_msg.data.unfriendly, &subJSON, "unfriendly");

	free(jsonStr);
	FreeJSON(recvJSON);
	closesocket(server);
	logInfo("成功获取群成员信息");
}

/*初始化*/
void init_gocqhttpAPI(const char* ip, const int port)
{
	memset((void*)&server_addr, 0, sizeof(SOCKADDR_IN));
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, (void*)&server_addr.sin_addr);

	WSADATA wsaData;
	int e = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (e)
	{
		logErr("初始化动态库失败");
		exit(1);
	}
	
	logInfo("API模块初始化完成");
}

///*退出*/
//VOID EXIT_GOCQHTTPAPI(VOID)
//{
//	CLOSESOCKET(SERVER);
//	MEMSET((VOID*)&SERVER_ADDR, 0, SIZEOF(SERVER_ADDR));
//}

//API连接
static SOCKET APIconection()
{
	SOCKET server;
	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		logPresetAll(SocketInitError);
		return 0;
	}
	if (connect(server, (SOCKADDR*)&server_addr, sizeof(server_addr)) < 0)
	{
		logPresetAll(ConnectionError);
		return 0;
	}
	return server;
}
