#pragma once
#include<malloc.h>
#include<stdio.h>

//获取文件大小
int getFileSize(FILE* fp);

//安全申请内存（使用malloc申请，如果返回NULL，则退出程序）
void* malloc_s(size_t size);

//去除http消息头
char* removeHeaders(char* httpMessage);