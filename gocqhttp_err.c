#include"gocqhttp_err.h"
#include<string.h>

/*…Ë÷√cqhttp_err*/
cqhttp_err set_cqhttp_err(cqhttp_err_list list, char function[20])
{
	cqhttp_err err;

	err.error = list;
	strcpy_s(err.function, sizeof(err.function), function);
	
	return err;
}

/* ‰≥ˆ¥ÌŒÛ*/
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
	}
	printf("| function: \"%s\"\n", err.function);
	return err.error;
}