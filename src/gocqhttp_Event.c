#include "gocqhttp_Event.h"
#include "gocqhttp_err.h"
#include"AnaJSON.h"
#include"URLcode.h"
#include<process.h>
#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<windows.h>
#include<WS2tcpip.h>
#include<string.h>
#include<time.h>
#pragma comment(lib, "ws2_32.lib")

/*socket*/

//socket返回的文件描述符
SOCKET serverSocket;
SOCKET client;			//客户端

//socket数据
SOCKADDR_IN server_addr;//服务端
SOCKADDR_IN client_addr;//客户端


//结构大小
int addr_len = sizeof(SOCKADDR_IN);

void (*event_response)(void* data);				//事件类型响应函数

/*创建Event服务端*/
cqhttp_err init_gocqhttpEvent(const char* ip, const int port, void(*response)(void* data))
{
	if (!response)
		return set_cqhttp_err(NULLError, func, 1, "\"response\"是空指针");

	//初始化
	memset(&server_addr, 0, sizeof(SOCKADDR_IN));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, (void*)&server_addr.sin_addr);

	event_response = response;//设置事件判断函数

	//动态库初始化
	WSADATA wsaData;
	int e = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (e)
		return set_cqhttp_err(WSAStartupError, func, 0, NULL);

	//创建套接字
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return set_cqhttp_err(SocketInitError, func, 0, NULL);

	//绑定端口
	if (bind(serverSocket, (SOCKADDR*)&server_addr, sizeof(server_addr)) < 0)
		return set_cqhttp_err(BindError, func, 0, NULL);

	//监听端口
	if (listen(serverSocket, 1) < 0)
		return set_cqhttp_err(ListenError, func, 0, NULL);

	//开始
	
	puts("尝试接受连接...");
	if((client = accept(serverSocket, (SOCKADDR*)&client_addr, (socklen_t*)&addr_len)) < 0)
		return set_cqhttp_err(AcceptFailed, func, 0, NULL);

	puts("已接受连接！");
	return set_cqhttp_err(None, func, 0, NULL);
}

/*接收Event数据*/
cqhttp_err recv_event(void)
{
	int recv_max = 1000, count;
	char* msg;
	if (!event_response)				//如果未声明消息响应函数
		return set_cqhttp_err(NULLError, func, 1, "event_response");
	while (1)
	{
		count = 0;
		msg = (char*)malloc(2048);
		if (!msg)
			return set_cqhttp_err(NULLError, func, 0, NULL);
		memset(msg, 0, 2048);
		while (recv(client, msg, 2048, 0) < 0)
		{
			count++;
			if (count > recv_max)
			{
				closesocket(client);
				closesocket(serverSocket);
				return set_cqhttp_err(ConnectionError, func, 1, "当前连接已异常中断");
			}
		}
		if (send(client, Event_Response, strlen(Event_Response), 0) < 0)
			return set_cqhttp_err(NetworkIOError, func, 1, "发送http响应头失败");
		_beginthread(event_response, 0, (void*)msg);	//开始检索事件
	}
}

////////////
/*事件解析*/
////////////

//群消息事件
group_message_event_data group_message_event_analysis(char* data)
{
	char rmsg_json[1024] = { '\0' };
	char tn[20] = { '\0' };
	int index = 0, i = 0;
	char tstr[1024], tname[50], tna[50];
	char* t1, *t2, *t3;
	JSONData* json, anonymous, sender;
	group_message_event_data ge_data;
	memset(&ge_data, 0, sizeof(ge_data));

	while (data[index++] != '{');
	index--;
	while (data[index] != '\0')	rmsg_json[i++] = data[index++];
	
	json = StrtoJSON(rmsg_json);
	//anonymous--
	ZERO(tn);
	getJSONVal(json, "anonymous", tn, &anonymous);
	//如果拥有anonymous字段
	if (strcmp(tn, "null"))
	{
		//flag
		getJSONVal(&anonymous, "flag", ge_data.anonymous.flag, NULL);
		//id
		ZERO(tn);
		getJSONVal(&anonymous, "id", tn, NULL);
		sscanf(tn, "%lu", &ge_data.anonymous.id);
		//name
		getJSONVal(&anonymous, "name", tna, NULL);
		t3 = UTF8toGBK(tna);
		strcpy(ge_data.anonymous.name, t3);
		free(t3);
	}
	else
		ge_data.anonymous.id = 0;
	//front
	ZERO(tn);
	getJSONVal(&anonymous, "font", tn, NULL);
	sscanf(tn, "%d", &ge_data.font);
	//group_id
	ZERO(tn);
	getJSONVal(json, "group_id", tn, NULL);
	sscanf(tn, "%lu", &ge_data.group_id);
	//message
	getJSONVal(json, "message", tstr, NULL);
	t1 = UTF8toGBK(tstr);
	strcpy(ge_data.message, t1);
	free(t1);
	//message_id
	ZERO(tn);
	getJSONVal(json, "message_id", tn, NULL);
	sscanf(tn, "%d", &ge_data.message_id);
	//message_seq
	ZERO(tn);
	getJSONVal(json, "message_seq", tn, NULL);
	sscanf(tn, "%d", &ge_data.message_seq);
	//message_type
	getJSONVal(json, "message_type", ge_data.message_type, NULL);
	//post_type
	getJSONVal(json, "post_type", ge_data.post_type, NULL);
	//raw_message
	getJSONVal(json, "raw_message", ge_data.raw_message, NULL);
	//self_id
	ZERO(tn);
	getJSONVal(json, "self_id", tn, NULL);
	sscanf(tn, "%lu", &ge_data.self_id);
	//sender--
	getJSONVal(json, "sender", NULL, &sender);
	//age
	ZERO(tn);
	getJSONVal(&sender, "age", tn, NULL);
	sscanf(tn, "%d", &ge_data.sender.age);
	//area
	getJSONVal(&sender, "area", ge_data.sender.area, NULL);
	//card
	getJSONVal(&sender, "card", ge_data.sender.card, NULL);
	//level
	getJSONVal(&sender, "level", ge_data.sender.level, NULL);
	//nickname
	getJSONVal(&sender, "nickname", tname, NULL);
	t2 = UTF8toGBK(tname);
	strcpy(ge_data.sender.nickname, t2);
	free(t2);
	//role
	getJSONVal(&sender, "role", ge_data.sender.role, NULL);
	//sex
	getJSONVal(&sender, "sex", ge_data.sender.sex, NULL);
	//title
	getJSONVal(&sender, "title", ge_data.sender.title, NULL);
	//user_id
	ZERO(tn);
	getJSONVal(&sender, "user_id", tn, NULL);
	sscanf(tn, "%lu", &ge_data.sender.user_id);
	//sub_type
	getJSONVal(json, "sub_type", ge_data.sub_type, NULL);
	//time
	ZERO(tn);
	getJSONVal(json, "time", tn, NULL);
	sscanf(tn, "%lu", &ge_data.time);
	//user_id
	ZERO(tn);
	getJSONVal(json, "user_id", tn, NULL);
	sscanf(tn, "%lu", &ge_data.user_id);

	FreeJSON(json);

	return ge_data;
}

//私聊事件
private_message_event_data private_message_event_analysis(char* data)
{
	char tstr[1024], tname[50];
	char* t1, *t2;
	private_message_event_data pe_data;
	memset(&pe_data, 0, sizeof(pe_data));

	if (strstr(data, "\"sub_type\":\"friend\""))
	{
		if (sscanf(data, PRIVATE_MESSAGE_EVENT_FORM,
					&pe_data.font,
					tstr,
					&pe_data.message_id,
					pe_data.message_type,
					pe_data.post_type,
					pe_data.raw_message,
					&pe_data.self_id,
					&pe_data.sender.age,
					tname,
					pe_data.sender.sex,
					&pe_data.sender.user_id,
					pe_data.sub_type,
					&pe_data.target_id,
					&pe_data.time,
					&pe_data.user_id) == -1)
			cqhttp_err_out(set_cqhttp_err(StringError, func, 1, "sscanf_1"));
	}
	/*else if (strstr(data, "\"sub_type\":\"group\""))
	{
		if (sscanf(data, PRIVATE_MESSAGE_EVENT_FORM_2,
					&pe_data.font,
					pe_data.message,
					&pe_data.message_id,
					pe_data.message_type,
					pe_data.post_type,
					pe_data.raw_message,
					&pe_data.self_id,
					&pe_data.sender.age,
					&pe_data.sender.group_id,
					pe_data.sender.nickname,
					pe_data.sender.sex,
					&pe_data.sender.user_id,
					pe_data.sub_type,
					&pe_data.temp_source,
					&pe_data.time,
					&pe_data.user_id) == -1)
			cqhttp_err_out(set_cqhttp_err(StringError, func, 1, "sscanf_2"));
	}*/
	else
		cqhttp_err_out(set_cqhttp_err(StringError, func, 1, "字符串匹配失败"));
	t1 = UTF8toGBK(tstr);
	t2 = UTF8toGBK(tname);
	strcpy(pe_data.message, t1);
	strcpy(pe_data.sender.nickname, t2);
	free(t1);
	free(t2);

	return pe_data;
}

//群消息撤回事件
group_recall_notice_data group_recall_notice_analysis(char* data)
{
	group_recall_notice_data gr_data;
	memset(&gr_data, 0, sizeof(gr_data));

	if(sscanf(data, GROUP_RECALL_NOTICE_EVENT_FORM,
		&gr_data.group_id,
		&gr_data.message_id,
		gr_data.notice_type,
		&gr_data.operator_id,
		gr_data.post_type,
		&gr_data.self_id,
		&gr_data.time,
		&gr_data.user_id) == -1)
		cqhttp_err_out(set_cqhttp_err(StringError, func, 1, "sscanf"));
	return gr_data;
}

/*事件消息检索*/
//上报类型检索
event_type event_type_switch(const char* data)
{
	if (strstr(data, "\"post_type\":\"message\""))
		return message_event;
	else if (strstr(data, "\"post_type\":\"notice\""))
		return notice_event;
	else if (strstr(data, "\"post_type\":\"meta_event\""))
		return meta_event;
	else if (strstr(data, "\"post_type\":\"request\""))
		return request_event;
	else if (strstr(data, "\"post_type\":\"essence\""))
		return essence_event;
	else
		return unknow_event;
}

//消息类型检索
message_type message_type_switch(const char* data)
{
	if (strstr(data, "\"message_type\":\"private\""))
		return private_message;
	else if (strstr(data, "\"message_type\":\"group\""))
		return group_message;
	else
		return unknow_message;
}

//通知类型检索
notice_type notice_type_switch(char* data)
{
	if (strstr(data, "\"notice_type\":\"group_recall\""))
		return group_recall_notice;
	else if (strstr(data, "\"notice_type\":\"friend_recall\""))
		return friend_recall_notice;
	else return unknow_notice;
}