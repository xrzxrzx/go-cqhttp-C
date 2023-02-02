#include"URLcode.h"
#include"gocqhttp_API.h"
#include"gocqhttp_Event.h"
#include"robot.h"
#include "Log.h"
#include"AnaJSON.h"
#include"function.h"
#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<windows.h>
#include<WS2tcpip.h>
#include<string.h>
#include<time.h>
#pragma comment(lib, "ws2_32.lib")

const char ip[] = "127.0.0.1";
const int port_API = 5700;
const int port_Event = 5701;

void event_switch(void* data);		//事件检索

int main(void)
{
    init_Log();

RESTART:

    init_gocqhttpAPI(ip, port_API);

    init_gocqhttpEvent(ip, port_Event, event_switch, initialize);

    int recvErrorCode = recv_event();
    if (recvErrorCode != 0)
    {
        logWarn("当前连接已异常中断，错误码：%d，5秒后开始重连", recvErrorCode);
        Sleep(5000);
        goto RESTART;
    }

    return 0;
}

/*事件检索*/
void event_switch(void* data)
{
    char* event_msg = (char*)data;	//待处理的事件
    char* jsonString;
    JSON* eventInfo;
    MessageEventInfo messageEvent;
    RequestEventInfo requestEvent;
    NoticeEventInfo noticeEvent;

    jsonString = removeHeaders(event_msg);
    eventInfo = StringToJSON(jsonString);

    switch (event_type_switch(eventInfo))
    {
        case message_event:		//消息事件
            logInfo("接收 消息事件");
            messageEvent = message_event_analysis(eventInfo);
            message_event_responded(messageEvent);
            break;
        case request_event:		//请求事件
            logInfo("接收 请求事件");
            requestEvent = request_event_analysis(eventInfo);
            request_event_responded(requestEvent);
            break;
        case notice_event:		//上报事件
            logInfo("接收 上报事件");
            noticeEvent = notice_event_analysis(eventInfo);
            notice_event_responded(noticeEvent);
            break;
        case meta_event:		//元事件
            logInfo("接收 元事件");
            //code
            break;
        case unknow_event:		//未知事件
            logInfo("接收 未知事件");
            //code
            break;
    }
    FreeJSON(eventInfo);
    free(jsonString);
    free(event_msg);
}
