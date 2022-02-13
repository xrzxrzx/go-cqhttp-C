#include"AnaJSON.h"
#include<malloc.h>
#include<string.h>

/**
计算到下个符号的长度
@str 	需操作的字符串
@front  字符串起始索引
@ch		需检测的字符
*/
static int getSymlen(const char* str, int front, char ch);

/**
从指定位置复制字符串
@str1		需操作的字符串
@str2		复制后的字符串
@start		起始位置
@size		复制的大小
*/
static int strCpy(const char* str1, char* str2, int start, int size);

/**
解析JSON
@str		JSON字符串
@index		当前索引
@JSONData	JSONData对象
@n			当前JSONData子对象的数量
*/
static int AnaJSON(const char* str, int index, JSONData* data, int n);

static void RecFreeJSON(JSONData* data);

//解析字符串到JSON
JSONData* StrtoJSON(const char* string)
{
	int n = 0;
	int index = 1;
	if(!string || string[0] != '{')		//判断是否是正常JSON开头
		return NULL;

	JSONData* data = (JSONData*)malloc(sizeof(JSONData));
	data->domain = (JSONContent*)malloc(sizeof(JSONContent));
	data->domain->object = NULL;

	//设置根节点对象名
	data->name = (char*)malloc(5);
	strCpy("root", data->name, 0, 4);

	AnaJSON(string, index, data, n);
	return data;
}

//解析JSON
static int AnaJSON(const char* str, int index, JSONData* data, int n)
{
	int len;
	int ns = 0;
	//初始化
	if(n == 0)
	{
		data->domain->object = (JSONData*)malloc(sizeof(JSONData));
		data->num = 0;
		n = 1;
	}
	else
	{
		data->domain->object = (JSONData*)realloc(data->domain->object, sizeof(JSONData)*n);
		data->num = n;
	}
	data->domain->object[n-1].num = 0;
	data->domain->object[n-1].domain = NULL;
	data->domain->object[n-1].name = NULL;

	len = getSymlen(str, index+1, ':') - 1;	//"占一位
	data->domain->object[n-1].name = (char*)malloc(len + 1);
	strCpy(str, data->domain->object[n-1].name, index+1, len);
	index = index+1+len+2;		//移到“:”后一个字符
	switch(str[index])
	{
		case '\"':			//如果为字符串类型
			len = getSymlen(str, index+1, '\"');
			data->domain->object[n-1].domain = (JSONContent*)malloc(sizeof(JSONContent));
			data->domain->object[n-1].domain->value = (char*)malloc(len + 1);
			strCpy(str, data->domain->object[n-1].domain->value, index+1, len);
			index = index + 1 + len + 1;
			break;
		case '{':			//遇到对象 
			index++;
			data->domain->object[n-1].domain = (JSONContent*)malloc(sizeof(JSONContent));
			index = AnaJSON(str, index, &data->domain->object[n-1], ns);
			data->domain->object[n-1].num++;
			break;
		case '\0':			//遇到字符串结尾 
			free(data->domain->object[n-1].name);
			return -1;
//		case '[':
//			break;
		default:			//数字类型或布尔类型 
			len = getSymlen(str, index, ',');
			if(str[index+len-1] == '}')
				len = getSymlen(str, index, '}');
			if(len == -1)
			{
				len = getSymlen(str, index, '}');
				data->domain->object[n-1].domain = (JSONContent*)malloc(sizeof(JSONContent));
				data->domain->object[n-1].domain->value = (char*)malloc(len+1);
				strCpy(str, data->domain->object[n-1].domain->value, index, len);
				index = index + len;
				break;
			}
			data->domain->object[n-1].domain = (JSONContent*)malloc(sizeof(JSONContent));
			data->domain->object[n-1].domain->value = (char*)malloc(len+1);
			strCpy(str, data->domain->object[n-1].domain->value, index, len);
			index = index + len;
			break;
	}
//	index++;
	switch(str[index])
	{
		case '}':
		case '\0':
			index++;
			break;
		case ',':
			index = AnaJSON(str, ++index, data, ++n);
			break;
		default:
			return -1;
	}
	
	return index;
}

//计算到下个符号的长度
static int getSymlen(const char* str, int front, char ch)
{
	int len = 0;
	while(1)
	{
		if(str[front] == '\\')		//如果遇到转义符
		{
			front+=2;
			len+=2;
		}
		else if(str[front++] == ch)	//遇到匹配字符w
			break;
		else						//一般情况
			len++;
		if (str[len] == '\0')		//如果到字符串末尾任未检测到字符
			return -1; 
	}
	return len;
}

//从指定位置复制字符串
static int strCpy(const char* str1, char* str2, int start, int size)
{
	int i = 0;
	if(size <= 0)
	{
		str2[0] = '\0';
		return 1;
	}

	while(size > 0)
	{
		if(str1[start] == '\\')
		{
			size -= 2;
			start += 2;
			str2[i++] = str1[start - 1];
		}
		else if(str1[start] == '\0')
		{
			str2[i] = '\0';
			if(size > 0)
				return -1;
			else
				return 0;
		}
		else
		{
			str2[i++] = str1[start++];
			size--;
		}
	}
	str2[i] = '\0';
	return 0;
}

//释放JSONData
void FreeJSON(JSONData* data)
{
	RecFreeJSON(data);
	free(data);
}

static void RecFreeJSON(JSONData* con)
{
	int i;

	free(con->name);
	if (con->num == 0)
		free(con->domain->value);
	else
	{
		for(i = 0; i < con->num-1; i++)
			RecFreeJSON(&con->domain->object[i]);
		free(con->domain->object);
	}
	free(con->domain);
}

//获取对象的值
int getJSONVal(JSONData* data, const char* name, char* val1, JSONData* val2)
{
	if (!data)
		return -1;
	int i;
	if(data->num == 0)			//如果该对象的值为字符串 
	{
		strcpy(val1, data->domain->value);
		return 1;
	}
	else						//如果该对象的值为对象 
	{
		for(i = 0; i < data->num; i++)		//比较子对象 
		{
			if(!strcmp(name, data->domain->object[i].name))
			{
				if(data->domain->object[i].num != 0)	//如果该对象的值为对象 
				{
					if (val2 == NULL)
						return -1;
					*val2 = data->domain->object[i];
					return 2;
				}
				else									//如果该对象的值为字符串 
				{
					strcpy(val1, data->domain->object[i].domain->value);
					return 1;
				}
			}
		}
	}
	return 0;
}