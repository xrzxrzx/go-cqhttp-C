#include"Tool.h"
#include<stdlib.h>

int getFileSize(FILE* fp)
{
	fseek(fp, 0, SEEK_END);
	return ftell(fp);
}

void* malloc_s(size_t size)
{
	void* memory;
	memory = malloc(size);
	if (!memory)
	{
		puts("内存申请失败！");
		exit(1);
	}
	return memory;
}

//去除http消息头
char* removeHeaders(char* httpMessage)
{
	int headerSize, contentSize, i;
	char* jsonString;

	for (headerSize = 0; httpMessage[headerSize] != '{'; headerSize++)
		;
	contentSize = strlen(httpMessage) - headerSize + 1;
	jsonString = (char*)malloc_s(contentSize);
	for (i = 0; i < contentSize; i++, headerSize++)
	{
		jsonString[i] = httpMessage[headerSize];
	}

	return jsonString;
}