#pragma once

#ifndef _LOG_H_
#define _LOG_H_

/*设置控制台文字颜色
* color code
* 控制台：11（蓝白）
* 出错：4（红）
* 信息：9（蓝）
* 警告：14（黄）
*/
int color(int a);

#define DEFAULT_COLOR	11
#define ERROR_COLOR		4
#define INFO_COLOR		9
#define WARNING_COLOR	14

typedef enum _log_type
{
	Information,			//一般信息
	Warning,				//一般警告
	Error,					//一般错误
	WSAStartupError,		//WSAStartup函数错误
	SocketInitError,		//socket初始化异常
	BindError,				//绑定端口失败
	ListenError,			//监听端口失败
	AcceptFailed,			//接受连接失败
	ConnectionError,		//连接错误
	NetworkIOException,		//网络IO错误
	NULLException,			//空指针异常
	NotFound,				//文件未找到
	StringException,		//字符串操作异常
	Win32Exception,			//调用win api异常
	AllocNULLEXception		//内存申请函数调用异常
}LogType;	//错误类型

//日志输出（根据预设）
void logPreset(LogType type);
void logPresetDetailed(char* functionName, char* filePath, int line, LogType type);
#define	logPresetAll(type) logPresetDetailed(__func__, __FILE__, __LINE__, type)

//信息输出
void logInfo(char* message, ...);
void logInfoDetailed(char* functionName, char* filePath, int line, char* message, ...);
#define logInfoAll(message,...) logInfoDetailed(__func__, __FILE__, __LINE__, message, __VA_ARGS__)

//警告输出
void logWarn(char* message, ...);
void logWarnDetailed(char* functionName, char* filePath, int line, char* message, ...);
#define logWarnAll(message,...) logWarnDetailed(__func__, __FILE__, __LINE__, message, __VA_ARGS__)

//错误输出
void logErr(char* message, ...);
void logErrDetailed(char* functionName, char* filePath, int line, char* message, ...);
#define logErrAll(message,...) logErrDetailed(__func__, __FILE__, __LINE__, message, __VA_ARGS__)

#include<time.h>

static time_t now;
static struct tm timeinfo;

#include<stdio.h>

static FILE* logFile;

/*初始化日志模块*/
void init_Log();

#endif