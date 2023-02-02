#include "gocqhttp_Event.h"
#include "Log.h"
#include"Tool.h"
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
SOCKET serverSocket;	//服务端
SOCKET client;			//客户端

//socket数据
SOCKADDR_IN server_addr;//服务端
SOCKADDR_IN client_addr;//客户端


//结构大小
int addr_len = sizeof(SOCKADDR_IN);

void (*event_response)(void* data);				//事件类型响应函数

/*创建Event服务端*/
void init_gocqhttpEvent(const char* ip, const int port, void(*response)(void* data), void(*init)(void))
{
    if (!response || !init)
    {
        logErr("初始化Event模块失败！函数指针 response 为空");
        exit(1);
    }

    init();

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
    {
        logPresetAll(WSAStartupError);
        exit(1);
    }

    //创建套接字
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        logPresetAll(SocketInitError);
        exit(1);
    }

    //绑定端口
    if (bind(serverSocket, (SOCKADDR*)&server_addr, sizeof(server_addr)) < 0)
    {
        logPresetAll(BindError);
        exit(1);
    }

    //监听端口
    if (listen(serverSocket, 100) < 0)
    {
        logPresetAll(ListenError);
        exit(1);
    }

    //开始接受连接
    logInfo("等待连接...");
    if ((client = accept(serverSocket, (SOCKADDR*)&client_addr, (socklen_t*)&addr_len)) < 0)
    {
        logWarn("接受连接失败，五秒后重新接受连接");
        Sleep(5000);
    }

    logInfo("已接受连接！");
}

/*接收Event数据*/
int recv_event(void)
{
    int recv_max = 1000, count;
    char* msg;
    if (!event_response)				//如果未声明消息响应函数
    {
        logErr("event_response 是空指针");
        exit(1);
    }

    while (1)
    {
        count = 0;
        msg = (char*)malloc_s(2048);
        if (!msg)
            logPresetAll(NULLException);
        memset(msg, 0, 2048);

        while (recv(client, msg, 2048, 0) < 0)
        {
            count++;
            if (count > recv_max)	//如果连接中断
            {
                closesocket(client);
                closesocket(serverSocket);
                return -1;
            }
        }
        if (send(client, Event_Response, strlen(Event_Response), 0) < 0)
        {
            return -2;//发送http响应头失败
        }
        _beginthread(event_response, 0, (void*)msg);	//开始检索事件
    }
}


/*事件解析*/
//消息事件
MessageEventInfo message_event_analysis(JSON* eventData)
{
    MessageEventInfo info;
    JSON subJSON;
    char tempStr[1024] = { '\0' };
    char* tempPtr;

    //message
    getStringValue(tempStr, eventData, "message");
    tempPtr = UTF8toGBK(tempStr);
    strcpy_s(info.message, MESSAGE_DEFAULT_SIZE, tempPtr);
    free(tempPtr);

    //self_id
    if (getIntValue(&info.self_id, eventData, "self_id") == -1)
    {
        info.self_id = 0;
    }

    //group_id
    if (getIntValue(&info.group_id, eventData, "group_id") == -1)
    {
        info.group_id = 0;
        info.isPrivate = 1;
    }
    else
    {
        info.isPrivate = 0;
    }

    //user_id
    int i = getIntValue(&info.user_id, eventData, "user_id");

    //nickname
    memset(tempStr, '\0', 1024);
    getStringValue(tempStr, eventData, "nickname");
    tempPtr = UTF8toGBK(tempStr);
    strcpy_s(info.nickname, 50, tempPtr);
    if(!tempPtr)
        free(tempPtr);

    //message_id
    getIntValue(&info.message_id, eventData, "message_id");

    //time
    getIntValue(&info.time, eventData, "time");

    return info;
}
//请求事件
RequestEventInfo request_event_analysis(JSON* eventData)
{
    RequestEventInfo info;
    char tempStr[1024] = {'\0'};
    char* tempPtr;
    char type[20] = { '\0' };

    //group_id
    if (getIntValue(&info.group_id, eventData, "group_id") == -1)
    {
        info.isFriend = 1;
        info.group_id = 0;
    }
    else
    {
        info.isFriend = 0;
    }

    //self_id
    if (getIntValue(&info.self_id, eventData, "self_id") == -1)
    {
        info.self_id = 0;
    }

    //user_id
    getIntValue(&info.user_id, eventData, "user_id");

    //time
    getIntValue(&info.time, eventData, "time");

    //comment
    getStringValue(tempStr, eventData, "comment");
    tempPtr = UTF8toGBK(tempStr);
    strcpy_s(info.comment, MESSAGE_DEFAULT_SIZE, tempPtr);
    free(tempPtr);

    //type
    getStringValue(type, eventData, "request_type");
    if (!strcmp(type, "friend"))
    {
        info.type = friend;
    }
    else if (!strcmp(type, "group"))
    {
        info.type = group;
    }

    return info;
}
//上报事件类型检索
static NoticeType notice_event_type_switch(JSON* json);
//上报事件
NoticeEventInfo notice_event_analysis(JSON* eventData)
{
    NoticeEventInfo info;
    char tempStr[1024] = { '\0' };
    char* tempPtr;
    char subType[20] = { '\0' };

    //group_id
    if (getIntValue(&info.group_id, eventData, "group_id") == -1)
    {
        info.isPrivate = 1;
        info.group_id = 0;
    }
    else
    {
        info.isPrivate = 0;
    }

    //self_id
    if (getIntValue(&info.self_id, eventData, "self_id") == -1)
    {
        info.self_id = 0;
    }

    //user_id
    getIntValue(&info.user_id, eventData, "user_id");

    //nickname
    getStringValue(tempStr, eventData, "nickname");
    tempPtr = UTF8toGBK(tempStr);
    strcpy_s(info.nickname, 50, tempPtr);
    free(tempPtr);

    //operator_id
    getIntValue(&info.operator_id, eventData, "operator_id");

    //target_id
    getIntValue(&info.target_id, eventData, "target_id");
    
    //time
    getIntValue(&info.time, eventData, "time");

    //message_id
    if (getIntValue(&info.message_id, eventData, "message_id") == -1)
    {
        info.message_id = 0;
    }
    
    //type,subType
    getStringValue(subType, eventData, "sub_type");
    switch (notice_event_type_switch(eventData))
    {
    case friend_recall:
        info.type = friend_recall;
        break;
    case group_recall:
        info.type = group_recall;
        break;
    case group_increase:
        info.type = group_increase;
        if (!strcmp(subType, "approve"))
            info.subType.group_increase = approve;
        else if (!strcmp(subType, "invite"))
            info.subType.group_increase = invite;
        break;
    case group_decrease:
        info.type = group_decrease;
        if (!strcmp(subType, "leave"))
            info.subType.group_decrease = leave;
        else if (!strcmp(subType, "kick"))
            info.subType.group_decrease = kick;
        else if (!strcmp(subType, "kick_me"))
            info.subType.group_decrease = kick_me;
        break;
    case group_admin:
        info.type = group_admin;
        if (!strcmp(subType, "set"))
            info.subType.group_admin = set;
        else if (!strcmp(subType, "unset"))
            info.subType.group_admin = unset;
        break;
    case group_upload:
        info.type = group_upload;
        break;
    case group_ban:
        info.type = group_ban;
        if (!strcmp(subType, "ban"))
            info.subType.group_ban = ban;
        else if (!strcmp(subType, "lift_ban"))
            info.subType.group_ban = lift_ban;
        break;
    case friend_add:
        info.type = friend_add;
        break;
    case notify:
        info.type = notify;
        if (!strcmp(subType, "poke"))
            info.subType.notify = poke;
        else if (!strcmp(subType, "honor"))
            info.subType.notify = honor;
        else if (!strcmp(subType, "title"))
            info.subType.notify = title;
        break;
    case group_card:
        info.type = group_card;
        break;
    case offline_file:
        info.type = offline_file;
        break;
    case client_status:
        info.type = client_status;
        break;
    case essence:
        info.type = essence;
        if (!strcmp(subType, "add"))
            info.subType.essence = add;
        else if (!strcmp(subType, "delete"))
            info.subType.essence = delete;
        break;
    }

    return info;
}

static NoticeType notice_event_type_switch(JSON* json)
{
    char type[20] = { '\0' };

    getStringValue(type, json, "notice_type");

    if (!strcmp(type, "friend_recall"))
        return friend_recall;
    else if (!strcmp(type, "group_recall"))
        return group_recall;
    else if (!strcmp(type, "group_increase"))
        return group_increase;
    else if (!strcmp(type, "group_decrease"))
        return group_decrease;
    else if (!strcmp(type, "group_admin"))
        return group_admin;
    else if (!strcmp(type, "group_upload"))
        return group_upload;
    else if (!strcmp(type, "group_ban"))
        return group_ban;
    else if (!strcmp(type, "friend_add"))
        return friend_add;
    else if (!strcmp(type, "notify"))
        return notify;
    else if (!strcmp(type, "group_card"))
        return group_card;
    else if (!strcmp(type, "offline_file"))
        return offline_file;
    else if (!strcmp(type, "client_status"))
        return client_status;
    else if (!strcmp(type, "essence"))
        return essence;
    return friend_recall;
}

/*事件响应*/
//消息事件响应
void message_event_responded(MessageEventInfo info)
{
    logInfo("接收群 %d 成员 %s 的消息：%s", info.group_id, info.nickname, info.message);
    messageRespondedFunction function = findMessageFunction(info.message);
    if (function == NULL)
        return;

    function(info);
}
//请求事件响应
void request_event_responded(RequestEventInfo info)
{
    logInfo("收到请求 %s 来自 %d", requestTypetoString(info.type), info.user_id);
    requestRespondedFunction function = findRequestFunction(info.type);
    if (function == NULL)
        return;

    function(info);
}
//上报事件响应
void notice_event_responded(NoticeEventInfo info)
{
    logInfo("收到群 %d 成员 %s 的上报：%s", info.group_id, info.nickname, noticeTypetoString(info.type));
    noticeRespondedFunction function = findNoticeFunction(info.type);
    if (function == NULL)
        return;

    function(info);
}

//事件检索
event_type event_type_switch(JSON* data)
{
    char post_type[20] = { '\0' };
    getStringValue(post_type, data, "post_type");

    if (!strcmp(post_type, "message"))
        return message_event;
    else if (!strcmp(post_type, "notice"))
        return notice_event;
    else if (!strcmp(post_type, "meta_event"))
        return meta_event;
    else if (!strcmp(post_type, "request"))
        return request_event;
    else
        return unknow_event;
}

int registerMessageFunction(messageRespondedFunction func, char* callName)
{
    MessageRespondedNode* con = messageNodeHead;
    MessageRespondedNode* node = (MessageRespondedNode*)malloc_s(sizeof(MessageRespondedNode));

    node->callName = callName;
    node->function = func;
    node->next = NULL;

    if (con == NULL)
    {
        messageNodeHead = node;
        return 0;
    }
    else
    {
        if (!strcmp(con->callName, callName))
        {
            logWarn("无法注册消息事件：%s，该名称已注册", callName);
            free(node);
            return -1;
        }
    }

    while (con->next != NULL)
    {
        if (!strcmp(con->next->callName, callName))
        {
            logWarn("无法注册消息事件：%s，该名称已注册", callName);
            free(node);
            return -1;
        }
        con = con->next;
    }
    con->next = node;
    return 0;
}

int registerRequestFunction(requestRespondedFunction func, RequestType type, char* callName)
{
    RequestRespondedNode* con = requestNodeHead;
    RequestRespondedNode* node = (RequestRespondedNode*)malloc_s(sizeof(RequestRespondedNode));

    node->callName = callName;
    node->type = type;
    node->function = func;
    node->next = NULL;

    if (con == NULL)
    {
        requestNodeHead = node;
        return 0;
    }
    else
    {
        if (!strcmp(con->callName, callName))
        {
            logWarn("无法注册请求事件：%s，该名称已注册", callName);
            free(node);
            return -1;
        }
    }

    while (con->next != NULL)
    {
        if (!strcmp(con->next->callName, callName))
        {
            logWarn("无法注册请求事件：%s，该名称已注册", callName);
            free(node);
            return -1;
        }
        con = con->next;
    }
    con->next = node;
    return 0;
}

int registerNoticeFunction(noticeRespondedFunction func, NoticeType type, char* callName)
{
    NoticeRespondedNode* con = noticeNodeHead;
    NoticeRespondedNode* node = (NoticeRespondedNode*)malloc_s(sizeof(NoticeRespondedNode));

    node->callName = callName;
    node->type = type;
    node->function = func;
    node->next = NULL;

    if (con == NULL)
    {
        noticeNodeHead = node;
        return 0;
    }
    else
    {
        if (!strcmp(con->callName, callName))
        {
            logWarn("无法注册上报事件：%s，该名称已注册", callName);
            free(node);
            return -1;
        }
    }

    while (con->next != NULL)
    {
        if (!strcmp(con->next->callName, callName))
        {
            logWarn("无法注册上报事件：%s，该名称已注册", callName);
            free(node);
            return -1;
        }
        con = con->next;
    }
    con->next = node;
    return 0;
}

messageRespondedFunction findMessageFunction(const char* message)
{
    MessageRespondedNode* con = messageNodeHead;

    if (!con)
    {
        logWarn("未注册消息事件响应函数");
        return NULL;
    }

    while (con != NULL)
    {
        if (cmpMsgwithName(message, con->callName))
        {
            logInfo("找到消息事件响应： %s", con->callName);
            return con->function;
        }
        con = con->next;
    }
    return NULL;
}

requestRespondedFunction findRequestFunction(RequestType type)
{
    RequestRespondedNode* con = requestNodeHead;

    if (!con)
    {
        logWarn("未注册请求事件响应函数");
        return NULL;
    }

    while (con != NULL)
    {
        if (con->type == type)
        {
            logInfo("找到请求事件响应： %s", con->callName);
            return con->function;
        }
        con = con->next;
    }
    return NULL;
}

noticeRespondedFunction findNoticeFunction(NoticeType type)
{
    NoticeRespondedNode* con = noticeNodeHead;

    if (!con)
    {
        logWarn("未注册上报事件响应函数");
        return NULL;
    }

    while (con != NULL)
    {
        if (con->type == type)
        {
            logInfo("找到上报事件响应： %s", con->callName);
            return con->function;
        }
        con = con->next;
    }
    return NULL;
}

//从消息开头比较名称（若匹配则返回1，否则返回0）
static int cmpMsgwithName(const char* message, const char* name)
{
    int len = strlen(name);
    int i;
    for (i = 0; i < len; i++)
    {
        if (message[i] == '\0' || message[i] != name[i])
        {
            return 0;
        }
    }
    return 1;
}

//把RequestType转换成字符串
static char* requestTypetoString(RequestType type)
{
    switch (type)
    {
    case friend:
        return "friend";
    case group:
        return "group";
    }

    logErrAll("未知类型");
    return "null";
}

//把NoticeType转换成字符串
static char* noticeTypetoString(NoticeType type)
{
    switch (type)
    {
    case friend_recall:
        return "friend_recall";
    case group_recall:
        return "group_recall";
    case group_increase:
        return "group_increase";
    case group_decrease:
        return "group_decrease";
    case group_admin:
        return "group_admin";
    case group_upload:
        return "group_upload";
    case group_ban:
        return "group_ban";
    case  friend_add:
        return "friend_add";
    case notify:
        return "notify";
    case group_card:
        return "group_card";
    case offline_file:
        return "offline_file";
    case client_status:
        return "client_status";
    case essence:
        return "essence";
    }

    logErrAll("未知类型");
    return "null";
}
