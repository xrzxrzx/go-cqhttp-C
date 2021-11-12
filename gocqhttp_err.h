#pragma once

typedef enum
{
	None,					//没有错误
	WSAStartupError,		//WSAStartup函数异常
	SocketInitError,		//socket初始化异常
	BindError,				//绑定端口失败
	ListenError,			//监听端口失败
	AcceptFailed,			//接受连接失败
	ConnectionError,		//连接错误
	NetworkIOError,			//网络IO错误
	NULLError,				//空指针异常
	NotFound,				//文件未找到
	StringError				//字符串操作失败
}cqhttp_err_list;	//错误类型

typedef struct
{
	cqhttp_err_list error;	//错误类型
	char function[70];		//错误函数
	int flag;				//是否拥有附加说明
	char instructions[500];	//附加说明
} cqhttp_err;		//错误封装

//设置cqhttp_err
cqhttp_err set_cqhttp_err(
	cqhttp_err_list list,			//错误类型
	const char function[70],		//错误函数
	int flag,						//是否拥有附加说明
	char instructions[500]			//附加说明
);

//输出错误
cqhttp_err_list cqhttp_err_out(
	cqhttp_err err		//错误信息
);