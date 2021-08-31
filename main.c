#include"URLcode.h"
#include"gocqhttp_API.h"
#include"gocqhttp_Event.h"
#include"gocqhttp_err.h"
#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<windows.h>
#include<WS2tcpip.h>
#include<string.h>
#include<errno.h>
#pragma comment(lib, "ws2_32.lib")

const char ip[] = "127.0.0.1";
const int port_API = 5700;
const int port_Event = 5701;

void event_switch(void* data);		//事件检索

int main(void)
{
	cqhttp_err err;

	err = init_gocqhttpAPI(ip, port_API);
	cqhttp_err_out(err);
	err = init_gocqhttpEvent(ip, port_Event, event_switch);
	cqhttp_err_out(err);
	err = recv_event();
	cqhttp_err_out(err);

	return 0;
}

/*事件检索*/
void event_switch(void* data)
{
	char func[70] = "event_switch";

	char* event_msg;	//待处理的事件
	event_data msg;		//已解析事件
	cqhttp_err_out(set_cqhttp_err(None, func, 0, NULL));
	while (1)
	{
		while (!(event_msg = event_get())) /*Sleep(100)*/;//获取事件（将前面注释去掉可以设置事件处理间隔，建议设置成100）
		switch (event_type_switch(event_msg))
		{
			case message_event:		//消息事件
				cqhttp_err_out(set_cqhttp_err(None, func, 1, "recv message_event"));
				switch (message_type_switch(event_msg))
				{
					case private_message:	//私聊消息
						cqhttp_err_out(set_cqhttp_err(None, func, 1, "recv parivate_message"));
						msg.private_message = private_message_event_analysis(event_msg);
						//code
						break;
					case group_message:		//群消息
						cqhttp_err_out(set_cqhttp_err(None, func, 1, "recv group_message"));
						msg.group_message = group_message_event_analysis(event_msg);
						//code
						break;
					case unknow_message:	//未知
						cqhttp_err_out(set_cqhttp_err(None, func, 1, "recv unknow_message"));
						//code
						break;
				}
				break;
			case notice_event:		//通知事件
				cqhttp_err_out(set_cqhttp_err(None, func, 1, "recv notice_event"));
				//code（未完成，请谨慎编写）
				break;
			case meta_event:		//心跳事件
				cqhttp_err_out(set_cqhttp_err(None, func, 1, "recv unknow_message"));
				//code
				break;
			case unknow_event:		//未知事件
				cqhttp_err_out(set_cqhttp_err(None, func, 1, "recv unknow_event"));
				//code
				break;
		}
		free(event_msg);
	}
}
