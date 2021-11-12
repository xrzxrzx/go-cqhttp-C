#include"gocqhttp_API.h"
#include"URLcode.h"
#include<process.h>
#include<winsock2.h>
#include<windows.h>
#include<WS2tcpip.h>
#include<stdio.h>
#include<string.h>
#pragma comment(lib, "ws2_32.lib")

/* socket */
static SOCKADDR_IN	server_addr;//服务端
static SOCKET		server;		//新建套接字


/*send_private_msg 发送私密消息*/
send_private_msg_data New_send_private_msg(unsigned long user_id, unsigned long group_id, char message[1024], int auto_escape)
{
	char* temp = GBKtoUTF8(message);
	send_private_msg_data data;
	data.send_msg.user_id = user_id;
	data.send_msg.group_id = group_id;
	strcpy(data.send_msg.message, temp);
	data.send_msg.auto_escape = auto_escape;
	free(temp);
	return data;
}

cqhttp_err send_private_msg(send_private_msg_data* data)
{
	char func[70] = "send_private_msg";

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return set_cqhttp_err(SocketInitError, func, 0, NULL);
	if (connect(server, (SOCKADDR*)&server_addr, sizeof(server_addr)) < 0)
		return set_cqhttp_err(ConnectionError, func, 0, NULL);

	char rmsg[1024] = { '\0' };							//收包
	char smsg[1024] = { '\0' };							//发包

	//构建发包（字符串）
	sprintf(smsg, API_SEND_PRIVATE_MSG_FORM,
			data->send_msg.user_id,
			data->send_msg.group_id,
			data->send_msg.message,
			data->send_msg.auto_escape);
	int isend = send(server, smsg, strlen(smsg), 0);	//发送
	if (isend < 0)
	{
		closesocket(server);
		return set_cqhttp_err(NetworkIOError, func, 1, "发包发送失败");
	}

	memset(data, 0, sizeof(send_private_msg_data));
	while (recv(server, rmsg, sizeof(rmsg), 0) < 0)
	{
		int count = 0;		//接收次数
		while (recv(server, rmsg, sizeof(rmsg), 0) < 0)		//接收
		{
			count++;
			if (count > RECV_MAX)
			{
				closesocket(server);
				return set_cqhttp_err(ConnectionError, func, 1, "当前连接已异常中断");
			}
		}
	}
	if(sscanf(rmsg, API_SEND_PRIVATE_MSG_RECV,
				&data->recv_msg.data.message_id,
				&data->recv_msg.retcode,
				data->recv_msg.status) == -1)
		cqhttp_err_out(set_cqhttp_err(StringError, func, 1, "sscanf"));

	closesocket(server);
	return set_cqhttp_err(None, func, 0, NULL);
}

/*send_group_msg 发送群消息*/
send_group_msg_data New_send_group_msg(unsigned long group_id, char message[1024], int auto_escape)
{
	char* temp = GBKtoUTF8(message);
	send_group_msg_data data;
	data.send_msg.group_id = group_id;
	strcpy(data.send_msg.message, temp);
	data.send_msg.auto_escape = auto_escape;
	free(temp);
	return data;
}

cqhttp_err send_group_msg(send_group_msg_data* data)
{
	char func[70] = "send_group_msg";

	memset(data, 0, sizeof(data));

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return set_cqhttp_err(SocketInitError, func, 0, NULL);
	if (connect(server, (SOCKADDR*)&server_addr, sizeof(server_addr)) < 0)
		return set_cqhttp_err(ConnectionError, func, 0, NULL);

	char rmsg[1024] = { '\0' };			//收包
	char smsg[1024] = { '\0' };			//发包

	//构建发包
	sprintf(smsg, API_SEND_GROUP_MSG_FORM,
			data->send_msg.group_id,
			data->send_msg.message,
			data->send_msg.auto_escape);
	int isend = send(server, smsg, strlen(smsg), 0);	//发送
	if (isend < 0)
	{
		closesocket(server);
		return set_cqhttp_err(NetworkIOError, func, 1, "发包发送失败");
	}

	memset(data, 0, sizeof(send_group_msg_data));
	int count = 0;		//接收次数
	while (recv(server, rmsg, sizeof(rmsg), 0) < 0)		//接收
	{
		count++;
		if (count > RECV_MAX)
		{
			closesocket(server);
			return set_cqhttp_err(ConnectionError, func, 1, "当前连接已异常中断");
		}
	}
	if (sscanf(rmsg, API_SEND_GROUP_MSG_RECV,
		&data->recv_msg.data.message_id,
		&data->recv_msg.retcode,
		data->recv_msg.status) == -1)
		cqhttp_err_out(set_cqhttp_err(StringError, func, 1, "sscanf"));

	closesocket(server);
	return set_cqhttp_err(None, func, 0, NULL);
}

/*get_msg 获取消息*/
get_msg_data New_get_msg(int message_id)
{
	get_msg_data data;
	data.send_msg.message_id = message_id;
	return data;
}

cqhttp_err get_msg(get_msg_data* data)
{
	char func[70] = "get_msg";

	memset(data, 0, sizeof(data));

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return set_cqhttp_err(SocketInitError, func, 0, NULL);
	if (connect(server, (SOCKADDR*)&server_addr, sizeof(server_addr)) < 0)
		return set_cqhttp_err(ConnectionError, func, 0, NULL);

	char rmsg[1024] = { '\0' };							//收包
	char smsg[1024] = { '\0' };							//发包

	//构建发包
	sprintf(smsg, API_GET_MSG_FORM,
		data->send_msg.message_id);
	int isend = send(server, smsg, strlen(smsg), 0);	//发送
	if (isend < 0)
	{
		closesocket(server);
		return set_cqhttp_err(NetworkIOError, func, 1, "发包发送失败");
	}

	memset(data, 0, sizeof(delete_msg_data));
	while (recv(server, rmsg, sizeof(rmsg), 0) < 0);	//接收
	if (sscanf(rmsg, API_GET_MSG_RECV,
		data->recv_msg.data.group,
		&data->recv_msg.data.group_id,
		data->recv_msg.data.message,
		&data->recv_msg.data.message_id,
		&data->recv_msg.data.message_seq,
		data->recv_msg.data.message_type,
		data->recv_msg.data.raw_message,
		&data->recv_msg.data.real_id,
		data->recv_msg.data.sender.nickname,
		&data->recv_msg.data.sender.user_id,
		&data->recv_msg.data.time,
		&data->recv_msg.retcode,
		data->recv_msg.status) == -1)
		cqhttp_err_out(set_cqhttp_err(StringError, func, 1, "sscanf"));

	closesocket(server);
	return set_cqhttp_err(None, func, 0, NULL);
}

/*delete_msg 撤回消息*/
delete_msg_data New_delete_msg(int message_id)
{
	delete_msg_data data;
	data.send_msg.message_id = message_id;
	return data;
}

cqhttp_err delete_msg(delete_msg_data* data)
{
	char func[70] = "delete_msg";

	memset(data, 0, sizeof(data));

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return set_cqhttp_err(SocketInitError, func, 0, NULL);
	if (connect(server, (SOCKADDR*)&server_addr, sizeof(server_addr)) < 0)
		return set_cqhttp_err(ConnectionError, func, 0, NULL);

	char rmsg[1024] = { '\0' };							//收包
	char smsg[1024] = { '\0' };							//发包

	//构建发包
	sprintf(smsg, API_DELETE_MSG_FORM,
		data->send_msg.message_id);
	int isend = send(server, smsg, strlen(smsg), 0);	//发送
	if (isend < 0)
	{
		closesocket(server);
		return set_cqhttp_err(NetworkIOError, func, 1, "发包发送失败");
	}

	memset(data, 0, sizeof(data));
	while (recv(server, rmsg, sizeof(rmsg), 0) < 0);	//接收
	if (sscanf(rmsg, API_DELETE_MSG_RECV,
		data->recv_msg.data,
		&data->recv_msg.retcode,
		data->recv_msg.status) == -1)
		cqhttp_err_out(set_cqhttp_err(StringError, func, 1, "sscanf"));

	closesocket(server);
	return set_cqhttp_err(None, func, 0, NULL);
}

/*get_group_member_info 获取群成员信息*/
get_group_member_info_data New_get_group_member_info(unsigned long group_id, unsigned long user_id, int no_cache)
{
	get_group_member_info_data data;
	data.send_msg.group_id = group_id;
	data.send_msg.user_id = user_id;
	data.send_msg.no_cache = no_cache;
	return data;
}

cqhttp_err get_group_member_info(get_group_member_info_data* data)
{
	char func[70] = "get_group_member_info";

	memset(data, 0, sizeof(data));

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return set_cqhttp_err(SocketInitError, func, 0, NULL);
	if (connect(server, (SOCKADDR*)&server_addr, sizeof(server_addr)) < 0)
		return set_cqhttp_err(ConnectionError, func, 0, NULL);

	char rmsg[1024] = { '\0' };							//收包
	char smsg[1024] = { '\0' };							//发包

	//构建发包
	sprintf(smsg, API_GET_GROUP_MEMBER_INFO_FORM,
		data->send_msg.group_id,
		data->send_msg.user_id,
		data->send_msg.no_cache);
	int isend = send(server, smsg, strlen(smsg), 0);	//发送
	if (isend < 0)
	{
		closesocket(server);
		return set_cqhttp_err(NetworkIOError, func, 1, "发包发送失败");
	}

	memset(data, 0, sizeof(get_group_member_info_data));
	while (recv(server, rmsg, sizeof(rmsg), 0) < 0);	//接收
	if (sscanf(rmsg, API_GET_GROUP_MEMBER_INFO_RECV,
		&data->recv_msg.data.age,
		data->recv_msg.data.area,
		data->recv_msg.data.card,
		data->recv_msg.data.card_changeable,
		&data->recv_msg.data.group_id,
		&data->recv_msg.data.join_time,
		&data->recv_msg.data.last_sent_time,
		data->recv_msg.data.level,
		data->recv_msg.data.nickname,
		data->recv_msg.data.role,
		data->recv_msg.data.sex,
		&data->recv_msg.data.shut_up_timestamp,
		data->recv_msg.data.title,
		&data->recv_msg.data.title_expire_time,
		data->recv_msg.data.unfriendly,
		&data->recv_msg.data.user_id,
		&data->recv_msg.retcode,
		data->recv_msg.status) == -1)
		cqhttp_err_out(set_cqhttp_err(StringError, func, 1, "sscanf"));

	closesocket(server);
	return set_cqhttp_err(None, func, 0, NULL);
}


/*初始化*/
cqhttp_err init_gocqhttpAPI(const char* ip, const int port)
{
	const char func[70] = "init_gocqhttpAPI";

	memset((void*)&server_addr, 0, sizeof(SOCKADDR_IN));
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, (void*)&server_addr.sin_addr);

	WSADATA wsaData;
	int e = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (e)
		return set_cqhttp_err(WSAStartupError, func, 1, "初始化动态库失败");
	
	return set_cqhttp_err(None, func, 0, NULL);
}

/*退出*/
void exit_gocqhttpAPI(void)
{
	closesocket(server);
	memset((void*)&server_addr, 0, sizeof(server_addr));
}