#include"Log.h"
#include"Tool.h"
#include<windows.h>
#include<string.h>
#include<malloc.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <io.h>
#include <direct.h>
#include<stdarg.h>

struct _logTypeInfoBase
{
	LogType baseType;	//预设类型的基本类型（信息，警告，错误）
	char* info;
};
typedef struct _logTypeInfoBase LogTypeInfoBase;
static LogTypeInfoBase setBaseType(LogType type, char* info);

/*获取日志类型基本信息*/
static LogTypeInfoBase getLogTypeInfo(LogType type);

//生成当前格式化时间
static void getNowFormatTime(char* formatTime);

//判断是否初始化
void isLogInit();

//日志输出（根据预设）
void logPreset(LogType type)
{
	LogTypeInfoBase baseType;
	baseType = getLogTypeInfo(type);
	switch (baseType.baseType)
	{
	case Information:
		logInfo(baseType.info);
		break;
	case Warning:
		logWarn(baseType.info);
		break;
	case Error:
		logErr(baseType.info);
		break;
	}
}
void logPresetDetailed(char* functionName, char* filePath, int line, LogType type)
{
	LogTypeInfoBase baseType;
	baseType = getLogTypeInfo(type);
	switch (baseType.baseType)
	{
	case Information:
		logInfoDetailed(functionName, filePath, line, baseType.info);
		break;
	case Warning:
		logWarnDetailed(functionName, filePath, line, baseType.info);
		break;
	case Error:
		logErrDetailed(functionName, filePath, line, baseType.info);
		break;
	}
}

//信息输出
void logInfo(char* message, ...)
{
	va_list(va);
	va_start(va, message);
	FILE* tempFile;
	char tempFilePath[40] = { '\0' };
	char fileName[11] = { '\0' };
	char* completeMessage;
	int messageSize;
	
	isLogInit();

	char nowFormatTime[30];
	getNowFormatTime(nowFormatTime);

	//把格式化字符串暂存到一个文件
	strncpy_s(fileName, sizeof(fileName), nowFormatTime, 10);//只截取 YYYY-MM-DD 的部分
	sprintf_s(tempFilePath, 40, "temp/%s", fileName);
	//fopen_s(&tempFile, tempFilePath, "wb+");
	tmpfile_s(&tempFile);
	vfprintf(tempFile, message, va);

	messageSize = getFileSize(tempFile) + 1;//+1 补空字符的位置
	completeMessage = (char*)malloc_s(messageSize);
	fseek(tempFile, 0L, SEEK_SET);
	fread_s(completeMessage, messageSize, messageSize, 1, tempFile);
	completeMessage[messageSize - 1] = '\0';//在最后补空字符

	color(INFO_COLOR);
	printf("%s <INFO> %s\n", nowFormatTime, completeMessage);
	color(DEFAULT_COLOR);
	fprintf(logFile, "%s <INFO> %s\n", nowFormatTime, completeMessage);

	fclose(tempFile);
	free(completeMessage);
	//remove(tempFilePath);
}
void logInfoDetailed(char* functionName, char*filePath, int line,  char* message, ...)
{
	va_list(va);
	va_start(va, message);
	FILE* tempFile;
	char tempFilePath[40] = { '\0' };
	char fileName[11] = { '\0' };
	char* completeMessage;
	int messageSize;

	isLogInit();

	char nowFormatTime[30];
	getNowFormatTime(nowFormatTime);

	//把格式化字符串暂存到一个文件
	strncpy_s(fileName, sizeof(fileName), nowFormatTime, 10);//只截取 YYYY-MM-DD 的部分
	sprintf_s(tempFilePath, 40, "temp/%s", fileName);
	//fopen_s(&tempFile, tempFilePath, "wb+");
	tmpfile_s(&tempFile);
	vfprintf(tempFile, message, va);

	messageSize = getFileSize(tempFile) + 1;//+1 补空字符的位置
	completeMessage = (char*)malloc_s(messageSize);
	fseek(tempFile, 0L, SEEK_SET);
	fread_s(completeMessage, messageSize, messageSize, 1, tempFile);
	completeMessage[messageSize - 1] = '\0';//在最后补空字符

	color(INFO_COLOR);
	printf("%s <INFO> %s\nfunction: %s\npath: %s\nline: %d\n\n", nowFormatTime, completeMessage, functionName, filePath, line);
	color(DEFAULT_COLOR);
	fprintf(logFile, "%s <INFO> %s\nfunction: %s\npath: %s\nline: %d\n\n", nowFormatTime, completeMessage, functionName, filePath, line);

	fclose(tempFile);
	free(completeMessage);
	//remove(tempFilePath);
}

//警告输出
void logWarn(char* message, ...)
{
	va_list(va);
	va_start(va, message);
	FILE* tempFile;
	char tempFilePath[40] = { '\0' };
	char fileName[11] = { '\0' };
	char* completeMessage;
	int messageSize;

	isLogInit();

	char nowFormatTime[30];
	getNowFormatTime(nowFormatTime);

	//把格式化字符串暂存到一个文件
	strncpy_s(fileName, sizeof(fileName), nowFormatTime, 10);//只截取 YYYY-MM-DD 的部分
	sprintf_s(tempFilePath, 40, "temp/%s", fileName);
	//fopen_s(&tempFile, tempFilePath, "wb+");
	tmpfile_s(&tempFile);
	vfprintf(tempFile, message, va);

	messageSize = getFileSize(tempFile) + 1;//+1 补空字符的位置
	completeMessage = (char*)malloc_s(messageSize);
	fseek(tempFile, 0L, SEEK_SET);
	fread_s(completeMessage, messageSize, messageSize, 1, tempFile);
	completeMessage[messageSize - 1] = '\0';//在最后补空字符

	color(WARNING_COLOR);
	printf("%s <WARNING> %s\n", nowFormatTime, completeMessage);
	color(DEFAULT_COLOR);
	fprintf(logFile, "%s <WARNING> %s\n", nowFormatTime, completeMessage);

	fclose(tempFile);
	free(completeMessage);
	//remove(tempFilePath);
}
void logWarnDetailed(char* functionName, char* filePath, int line, char* message, ...)
{
	va_list(va);
	va_start(va, message);
	FILE* tempFile;
	char tempFilePath[40] = { '\0' };
	char fileName[11] = { '\0' };
	char* completeMessage;
	int messageSize;

	isLogInit();

	char nowFormatTime[30];
	getNowFormatTime(nowFormatTime);

	//把格式化字符串暂存到一个文件
	strncpy_s(fileName, sizeof(fileName), nowFormatTime, 10);//只截取 YYYY-MM-DD 的部分
	sprintf_s(tempFilePath, 40, "temp/%s", fileName);
	//fopen_s(&tempFile, tempFilePath, "wb+");
	tmpfile_s(&tempFile);
	vfprintf(tempFile, message, va);

	messageSize = getFileSize(tempFile) + 1;//+1 补空字符的位置
	completeMessage = (char*)malloc_s(messageSize);
	fseek(tempFile, 0L, SEEK_SET);
	fread_s(completeMessage, messageSize, messageSize, 1, tempFile);
	completeMessage[messageSize - 1] = '\0';//在最后补空字符

	color(WARNING_COLOR);
	printf("%s <WARNING> %s\nfunction: %s\npath: %s\nline: %d\n\n", nowFormatTime, completeMessage, functionName, filePath, line);
	color(DEFAULT_COLOR);
	fprintf(logFile, "%s <WARNING> %s\nfunction: %s\npath: %s\nline: %d\n\n", nowFormatTime, completeMessage, functionName, filePath, line);

	fclose(tempFile);
	free(completeMessage);
	//remove(tempFilePath);
}

//错误输出
void logErr(char* message, ...)
{
	va_list(va);
	va_start(va, message);
	FILE* tempFile;
	char tempFilePath[40] = { '\0' };
	char fileName[11] = { '\0' };
	char* completeMessage;
	int messageSize;

	isLogInit();

	char nowFormatTime[30];
	getNowFormatTime(nowFormatTime);

	//把格式化字符串暂存到一个文件
	strncpy_s(fileName, sizeof(fileName), nowFormatTime, 10);//只截取 YYYY-MM-DD 的部分
	sprintf_s(tempFilePath, 40, "temp/%s", fileName);
	//fopen_s(&tempFile, tempFilePath, "wb+");
	tmpfile_s(&tempFile);
	vfprintf(tempFile, message, va);

	messageSize = getFileSize(tempFile) + 1;//+1 补空字符的位置
	completeMessage = (char*)malloc_s(messageSize);
	fseek(tempFile, 0L, SEEK_SET);
	fread_s(completeMessage, messageSize, messageSize, 1, tempFile);
	completeMessage[messageSize - 1] = '\0';//在最后补空字符

	color(4);
	printf("%s <ERROR> %s\n", nowFormatTime, completeMessage);
	color(7);
	fprintf(logFile, "%s <ERROR> %s\n", nowFormatTime, completeMessage);

	fclose(tempFile);
	free(completeMessage);
	//remove(tempFilePath);
}
void logErrDetailed(char* functionName, char* filePath, int line, char* message, ...)
{
	va_list(va);
	va_start(va, message);
	FILE* tempFile;
	char tempFilePath[40] = { '\0' };
	char fileName[11] = { '\0' };
	char* completeMessage;
	int messageSize;

	isLogInit();

	char nowFormatTime[30];
	getNowFormatTime(nowFormatTime);

	//把格式化字符串暂存到一个文件
	strncpy_s(fileName, sizeof(fileName), nowFormatTime, 10);//只截取 YYYY-MM-DD 的部分
	sprintf_s(tempFilePath, 40, "temp/%s", fileName);
	fopen_s(&tempFile, tempFilePath, "wb+");
	vfprintf(tempFile, message, va);

	messageSize = getFileSize(tempFile) + 1;//+1 补空字符的位置
	completeMessage = (char*)malloc_s(messageSize);
	fseek(tempFile, 0L, SEEK_SET);
	fread_s(completeMessage, messageSize, messageSize, 1, tempFile);
	completeMessage[messageSize - 1] = '\0';//在最后补空字符

	color(ERROR_COLOR);
	printf("%s <ERROR> %s\nfunction: %s\npath: %s\nline: %d\n\n", nowFormatTime, completeMessage, functionName, filePath, line);
	color(DEFAULT_COLOR);
	fprintf(logFile, "%s <ERROR> %s\nfunction: %s\npath: %s\nline: %d\n\n", nowFormatTime, completeMessage, functionName, filePath, line);

	fclose(tempFile);
	free(completeMessage);
	remove(tempFilePath);
}

//生成当前格式化时间
static void getNowFormatTime(char* formatTime)
{
	if (!formatTime)
		return;
	time(&now);
	localtime_s(&timeinfo, &now);
	sprintf_s(formatTime, 30, "%d-%d-%d %d:%d:%d",
		timeinfo.tm_year + 1900,
		timeinfo.tm_mon,
		timeinfo.tm_mday,
		timeinfo.tm_hour,
		timeinfo.tm_min,
		timeinfo.tm_sec);
}

static LogTypeInfoBase getLogTypeInfo(LogType type)
{
	switch (type)
	{
	case Information:
		return setBaseType(Information, "Information");
	case Warning:
		return setBaseType(Warning, "Warning");
	case Error:
		return setBaseType(Error, "Error");
	case WSAStartupError:
		return setBaseType(Error, "Failed to open socket service");
	case SocketInitError:
		return setBaseType(Error, "Initialization scoket error");
	case BindError:
		return setBaseType(Error, "Binding port failed");
	case ListenError:
		return setBaseType(Error, "Listening port failed");
	case AcceptFailed:
		return setBaseType(Warning, "Failed to accept connection");
	case ConnectionError:
		return setBaseType(Error, "Connection server error");
	case NetworkIOException:
		return setBaseType(Warning, "Failed to send or recv");
	case NULLException:
		return setBaseType(Warning, "The pointer is null");
	case NotFound:
		return setBaseType(Warning, "Not found the file");
	case StringException:
		return setBaseType(Warning, "String operation error");
	case AllocNULLEXception:
		return setBaseType(Error, "Memory request failed");
	default:
		return setBaseType(Warning, "No such error type");
	}
}

static LogTypeInfoBase setBaseType(LogType type, char* info)
{
	LogTypeInfoBase base;
	base.baseType = type;
	base.info = info;
	return base;
}

//初始化日志模块
void init_Log()
{
	char logFileName[25] = { '\0' };

	if (_access("log", 0) == -1)//如果文件夹不存在
	{
		int temp = _mkdir("log");
		if (temp != temp)//对返回值进行处理，这个地方不处理可能过不了编译，但一般情况下都没问题，除非这个exe没权限，所以我就不写了
		{
			printf("不可能的运行我");
		}
	}

	if (_access("temp", 0) == -1)//如果文件夹不存在
	{
		int temp = _mkdir("temp");
		if (temp != temp)//对返回值进行处理，这个地方不处理可能过不了编译，但一般情况下都没问题，除非这个exe没权限，所以我就不写了
		{
			printf("不可能的运行我");
		}
	}

	time(&now);
	localtime_s(&timeinfo, &now);
	sprintf_s(logFileName, 25, "log\\%d-%d-%d.log",
		timeinfo.tm_year + 1900,
		timeinfo.tm_mon,
		timeinfo.tm_mday);

	 fopen_s(&logFile, logFileName, "a");
}

void isLogInit()
{
	if (!logFile)//是否初始化日志模块
	{
		color(DEFAULT_COLOR);
		puts("未初始化日志模块！");
		exit(1);
	}
}

int color(int a)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a);    //更改文字颜色 
	return 0;
}