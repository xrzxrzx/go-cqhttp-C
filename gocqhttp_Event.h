#pragma once
#include"AnaJSON.h"

#define MESSAGE_DEFAULT_SIZE 1024

/*消息事件*/
typedef struct _message_event_info
{
    int isPrivate;			//是否是私聊消息
    unsigned int group_id;	//群号，当为私聊消息时，此成员为0
    unsigned int self_id;   //机器人QQ
    unsigned int user_id;   //发送人QQ
    char nickname[50];      //发送人昵称
    int message_id;         //消息ID
    char message[1024];     //消息
    int time;               //时间戳
}MessageEventInfo;	//消息事件通用数据结构

/*请求事件*/
typedef enum _request_event_type
{
    friend,
    group
}RequestType;

typedef struct _request_event_info
{
    int isFriend;		    //是否为好友请求
    unsigned int group_id;  //群号
    unsigned int self_id;   //机器人QQ
    unsigned int user_id;   //请求人QQ
    char comment[1024];		//验证消息
    int time;               //时间戳
    RequestType type;       //请求事件类型
}RequestEventInfo;	//请求事件通用数据结构

/*上报事件*/
typedef enum _notice_event_type
{
    friend_recall,		//好友消息撤回
    group_recall,		//群消息撤回
    group_increase,		//群成员增加
    group_decrease,		//群成员减少
    group_admin,		//群管理员变动
    group_upload,		//群文件上传
    group_ban,			//群禁言
    friend_add,			//好友添加
    notify,				//一些通知（戳一戳、红包运气王、群荣耀变更、群成员头衔变更）
    group_card,			//群成员名片更新
    offline_file,		//接收到离线文件
    client_status,		//其他客户端在线状态变更
    essence				//精华消息变更
}NoticeType;

typedef union _notice_event_sub_type
{
    enum
    {
        approve,//管理员同意
        invite	//管理员邀请
    }group_increase;
    enum
    {
        leave,	//主动退群
        kick,	//成员被踢
        kick_me	//登录号被踢
    }group_decrease;
    enum
    {
        set,	//设置管理员
        unset	//取消管理员
    }group_admin;
    enum
    {
        ban,	//被禁言
        lift_ban//解除禁言
    }group_ban;
    enum
    {
        poke,	//戳一戳
        honor,	//群荣耀变更
        title	//群成语头衔变更
    }notify;
    enum
    {
        add,	//添加
        delete	//删除
    }essence;
}NoticeSubType;

typedef struct _notice_event_info
{
    int isPrivate;          //是否是私人
    unsigned int group_id;  //群号
    unsigned int self_id;   //机器人QQ
    unsigned int user_id;   //触发上报事件的QQ
    unsigned int target_id; //目标QQ
    char nickname[50];      //触发事件用户的昵称
    unsigned int operator_id;//操作者QQ
    unsigned int time;      //时间戳
    unsigned int message_id;//此字段不一定会有，没有的话为0
    char comment[1024];		//杂项（一切上报事件所刚需的字符串数据都为这个成员）
    NoticeType type;        //上报事件各类型
    NoticeSubType subType;  //上报事件各类型的子类型
}NoticeEventInfo;		//上报事件通用数据结构

//去除HTTP消息头
char* removeHeaders(char* httpMessage);

#define ACCEPT_COUNT_MAX	20
#define Event_Response		"HTTP/1.1 204 OK\r\n\r\n"

/*事件类型*/
typedef enum _event_type
{
    message_event,			//消息
    notice_event,			//通知
    meta_event,				//元
    request_event,			//请求
    unknow_event			//未知
}event_type;

/*事件解析*/
//消息事件
MessageEventInfo message_event_analysis(JSON* eventData);
//请求事件
RequestEventInfo request_event_analysis(JSON* eventData);
//上报事件
NoticeEventInfo notice_event_analysis(JSON* eventData);

/*事件响应*/
//消息事件响应
void message_event_responded(MessageEventInfo info);
//请求事件响应
void request_event_responded(RequestEventInfo info);
//上报事件响应
void notice_event_responded(NoticeEventInfo info);

/*创建Event服务端*/
void init_gocqhttpEvent(
    const char* ip,
    const int port,
    void(*response)(void* data),		//消息事件响应函数
    void(*init)(void)                   //初始化
);

/*接收Event*/
int recv_event(void);

/*事件检索*/
event_type event_type_switch(JSON* data);

/*响应函数列表*/
typedef void (*messageRespondedFunction)(MessageEventInfo); //消息事件响应函数
typedef void (*requestRespondedFunction)(RequestEventInfo); //请求事件响应函数
typedef void (*noticeRespondedFunction)(NoticeEventInfo);   //上报事件响应函数

//消息事件
typedef struct _message_responded_node
{
    messageRespondedFunction function;
    char* callName;
    struct _message_responded_node* next;
}MessageRespondedNode;

//请求事件
typedef struct _request_responded_node
{
    requestRespondedFunction function;
    char* callName;
    RequestType type;
    struct _request_responded_node* next;
}RequestRespondedNode;

//上报事件
typedef struct _notice_responded_node
{
    noticeRespondedFunction function;
    char* callName;
    NoticeType type;
    struct _notice_responded_node* next;
}NoticeRespondedNode;

MessageRespondedNode* messageNodeHead;      //消息事件响应函数注册列表
RequestRespondedNode* requestNodeHead;      //请求事件响应函数注册列表
NoticeRespondedNode* noticeNodeHead;        //上报事件响应函数注册列表

/*注册事件响应函数*/
//注册消息事件
int registerMessageFunction(messageRespondedFunction func, char* callName);
//注册请求事件
int registerRequestFunction(requestRespondedFunction func, RequestType type, char* callName);
//注册上报事件
int registerNoticeFunction(noticeRespondedFunction func, NoticeType type, char* callName);

/*查找已注册函数*/
//查找消息事件函数
messageRespondedFunction findMessageFunction(char* message);
//查找请求事件函数
requestRespondedFunction findRequestFunction(RequestType type);
//查找上报事件函数
noticeRespondedFunction findNoticeFunction(NoticeType type);

//从消息开头比较名称（若匹配则返回1，否则返回0）
static int cmpMsgwithName(const char* message, const char* name);

//把RequestType转换成字符串
static char* requestTypetoString(RequestType type);

//把NoticeType转换成字符串
static char* noticeTypetoString(NoticeType type);
