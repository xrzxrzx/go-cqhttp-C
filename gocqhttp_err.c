#include"gocqhttp_err.h"
#include<string.h>
#include<stdio.h>

/*设置cqhttp_err*/
cqhttp_err set_cqhttp_err(cqhttp_err_list list, const char function[70], int flag, char instructions[500])
{
	cqhttp_err err;
	memset(&err, 0, sizeof(err));
	err.error = list;
	strcpy(err.function, function);
	err.flag = flag;
	if (err.flag)
		strcpy(err.instructions, instructions);
	return err;
}

/*输出错误*/
cqhttp_err_list cqhttp_err_out(cqhttp_err err)
{
	switch (err.error)
	{
		case None:
			printf("[INFO]No error(s) ");
			break;
		case WSAStartupError:
			printf("[ERROR]Failed to open socket service ");
			break;
		case SocketInitError:
			printf("[ERROR]Initialization scoket error ");
			break;
		case BindError:
			printf("[ERROR]Binding port failed ");
			break;
		case ListenError:
			printf("[ERROR]Listening port failed ");
			break;
		case AcceptFailed:
			printf("[ERROR]Failed to accept connection ");
			break;
		case ConnectionError:
			printf("[ERROR]Connection server error ");
			break;
		case NetworkIOError:
			printf("[WARNING]Failed to send or recv ");
			break;
		case NULLError:
			printf("[WARNING]Memory request failed ");
			break;
		case StringError:
			printf("[ERROR]String operation error ");
			break;
		case NotFound:
			printf("[ERROR]Not found the file ");
			break;
	}
	printf("| function: \"%s\" ", err.function);
	if (err.flag)
		printf("instructions: %s", err.instructions);
	printf("\n");
	return err.error;
}