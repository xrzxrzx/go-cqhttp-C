/**
* 此JSON解析库暂不支持 浮点类型、数组型、布尔型解析
*/

#include"AnaJSON.h"
#include"Tool.h"
#include"Log.h"
#include<string.h>
#include<stdlib.h>

/*计算到下个符号的长度*/
static int getLengthOfNextChar(const char* str, int startIndex, char ch);

/*从指定位置复制字符串*/
static char* interceptString(const char* str, int startIndex, int size);

/*解析JSON*/
static int AnalysisJSON(JSON** node, int index, char* JSONString);

/*获取字符串副本*/
static char* GetStringDuplicate(const char* string);

static char* getNULLString();

static int isNormalJSONStart(const char* string, int index);

static int AnalysisJSONString(JSON** node, int index, char* JSONString);
static int AnalysisJSONInt(JSON** node, int index, char* JSONString);
static int AnalysisJSONBool(JSON** node, int index, char* JSONString);
//static int AnalysisJSONArray(JSON** node, int index, char* JSONString);

//解析字符串到JSON
JSON* StringToJSON(const char* string)
{
	JSON* json;

	if (!isNormalJSONStart(string, 0))
	{
		logWarnAll("不正常的JSON数据开头\nstring: %s", string);
		return NULL;
	}

	AnalysisJSON(&json, 0, string);

	return json;
}

//解析JSON
static int AnalysisJSON(JSON** node, int index, char* JSONString)
{
	int length, preLength;
	char* name;

	preLength = index;
	index++;//自增操作是因为此时索引的字符是 " 或者 {，直接带入下一行的函数会返回0
	if (JSONString[index] == '\"')//如果此时为 " 则此 " 为字符串开头 " 直接跳过
		index++;
	
	length = getLengthOfNextChar(JSONString, index, '\"');//获取到对象名结束的双引号之前的字符的长度（不包括双引号）
	name = interceptString(JSONString, index, length);

	index = index + length + 2;//当前索引 + 对象名长度 + 对象名后 ": 两个字符的长度
	switch (JSONString[index])//检索冒号后的第一个字符（对象的值），以判断数据类型
	{
	case '\"'://字符串类型
		*node = CreateJSON(name, StringType);
		index += AnalysisJSONString(node, index, JSONString);
		break;
	case '1'://整数类型
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '0':
	case'-':
		*node = CreateJSON(name, IntType);
		index += AnalysisJSONInt(node, index, JSONString);
		break;
	case 't'://布尔类型
	case 'f':
        *node = CreateJSON(name, BoolType);
        index += AnalysisJSONBool(node, index, JSONString);
		break;
	case '['://数组类型
		//AnalysisJSONArray(node, recursionData->string, &CurrentIndex);
		break;
	case '{'://对象类型
		*node = CreateJSON(name, ObjectType);
		index += AnalysisJSON(&(*node)->data.jsonData, index, JSONString);
		break;
	case 'n'://NULL空值
		*node = CreateJSON(name, StringType);
		(*node)->data.stringData = getNULLString();
		index += 4;
		break;
	}

	//此时索引位置上的字符应该为 , 或者 }
	if (JSONString[index] == ',')
	{
		index++;
		index += AnalysisJSON(&(*node)->next, index, JSONString);
	}
	else if (JSONString[index] == '}' && JSONString[index + 1] != '\0')
	{
		index += 2;
		index += AnalysisJSON(&(*node)->next, index, JSONString);
	}

	return index - preLength;//返回索引增加的值
}

static int isNormalJSONStart(const char* string, int index)
{
	return string[0] == '{';
}

static int AnalysisJSONString(JSON** node, int index, char* JSONString)
{
	char* value;
	int length = getLengthOfNextChar(JSONString, index + 1, '\"');//+1操作是因为此时索引的字符是"，直接带入此函数会返回0
	value = interceptString(JSONString, index + 1, length);
	
	(*node)->data.stringData = value;

	return 1 + length + 1;//双引号长度 + 字符串长度 + 双引号长度
}

static int AnalysisJSONInt(JSON** node, int index, char* JSONString)
{
	int length, length1, length2;

	length1 = getLengthOfNextChar(JSONString, index, ',');
	length2 = getLengthOfNextChar(JSONString, index, '}');

	//判断当前对象结尾（可能为 } 或 ，）
	if (length1 == -1)
		length = length2;
	else if (length2 == -1)
		length = length1;
	else//对比到两个结尾符的距离，取最小的那个
		length = length1 > length2 ? length2 : length1;

	char* temp = interceptString(JSONString, index, length);
	//long Int = atol(temp);
    int Int = StringToInt(temp);
	(*node)->data.intData = Int;

	free(temp);
	return length;
}

static int AnalysisJSONBool(JSON** node, int index, char* JSONString)
{
	char* value;
	int length = getLengthOfNextChar(JSONString, index, ',');
	value = interceptString(JSONString, index, length);

	if (!strcmp(value, "true"))
	{
		(*node)->data.boolData = 1;
	}
	else if (!strcmp(value, "false"))
	{
		(*node)->data.boolData = 0;
	}
	else
	{
		(*node)->data.boolData = -1;
	}

	free(value);
	return length;
}

//static int AnalysisJSONArray(JSON** node, int index, char* JSONString)
//{
//	//JSONArray* ArrayNode;
//
//	//node->value.type = ArrayType;
//	//node->value.data.Array = (JSONArray*)malloc_s(sizeof(JSONArray));
//	//ArrayNode = node->value.data.Array;
//	//ArrayNode->nodeCount = 0;
//	//ArrayNode->head = NULL;
//
//	//recursionData->index++;
//	//switch (recursionData->string[recursionData->index])//检索冒号后的第一个字符（对象的值），以判断数据类型
//	//{//TODO 把这几个解析函数重新写一遍适配数组的
//	//case '\"'://字符串类型
//	//	ArrayNode->head = (JSONArrayNode*)malloc_s(sizeof(JSONArrayNode));
//
//	//	AnalysisJSONStringArray(node, recursionData->string, &CurrentIndex);
//	//	break;
//	//case '1'://整数类型
//	//case '2':
//	//case '3':
//	//case '4':
//	//case '5':
//	//case '6':
//	//case '7':
//	//case '8':
//	//case '9':
//	//case '0':
//	//	AnalysisJSONIntArray(node, recursionData->string, &CurrentIndex);
//	//	break;
//	//case '['://数组类型
//	//	//AnalysisJSONArray(node, recursionData->string, &CurrentIndex);
//	//	break;
//	//case 't'://布尔类型
//	//case 'f':
//	//	//AnalysisJSONBool(node, recursionData->string, &CurrentIndex);
//	//case '{'://对象类型
//	//	break;
//	//}
//
//
//}

//计算到下个符号的长度（不会将用于转义的斜杠计入总长度）
static int getLengthOfNextChar(const char* str, int startIndex, char ch)
{
	int len = 0;
	while(1)
	{
		if(str[startIndex] == '\\')		//如果遇到转义符
		{
			startIndex += 2;
			len++;
		}
		else if(str[startIndex++] == ch)	//遇到匹配字符
			break;
		else						//一般情况
			len++;
		if (str[len] == '\0')		//如果到字符串末尾任未检测到字符
			return -1; 
	}
	return len;
}

//从指定位置截取字符串（会将转义符作为一个字符截取，且只按1的大小计算，例如 \n 会被看做 n，长度为1）
static char* interceptString(const char* str, int startIndex, int size)
{
	int index, offset;
	char* returnString = (char*)malloc_s(size + 1);//+1为为\0字符预留位置
	
	for (index = 0, offset = 0; index < size; index++, offset++)
	{
		if (str[startIndex + offset] == '\0')//防止数组越界
		{
			logWarnAll("字符串字符串已到末尾\nstring: %s\nindex: %d\nsize: %d", str, startIndex, size);
			free(returnString);
			return getNULLString();
		}
		else if (str[startIndex + offset] == '\\')//如果遇到转义符
		{
			returnString[index] = str[startIndex + ++offset];//直接跳过当前转义符，并用下一个字符赋值
		}
		else
		{
			returnString[index] = str[startIndex + offset];
		}
	}
	returnString[index] = '\0';

	return returnString;
}

char* GetStringDuplicate(const char* string)
{
	char* duplicate;
	int len = strlen(string);
	duplicate = (char*)malloc_s(len + 1);
	strcpy_s(duplicate, len + 1, string);

	return duplicate;
}

static char* getNULLString()
{
	char* str = (char*)malloc_s(5);
	strcpy_s(str, 5, "NULL");
	return str;
}

//释放JSON数据
void DestoryJSON(JSON* data)
{
	FreeJSON(data);
}

JSON* CreateJSON(char* name, JSONType type)
{
	JSON* json;
	json = (JSON*)malloc_s(sizeof(JSON));
	json->next = NULL;
	json->pre = NULL;
	json->type = type;
	json->name = GetStringDuplicate(name);
	return json;
}

int AddJSON(JSON* json, char* name, JSONType type)
{
	JSON* con = json;
	JSON* newObject;

	newObject = CreateJSON(name, type);

	if (!con)
	{
		free(newObject);
		return -1;
	}
	
	while (con->next != NULL)
	{
		con = con->next;
	}
	con->next = newObject;
	newObject->pre = con;

	return 0;
}

int AddIntToArrayJSON(JSON* json, int intData)
{
	JSON* newData;
	if (!json || json->type != ArrayType)
		return -1;

	newData = CreateJSON(getNULLString(), IntType);
	newData->data.intData = intData;

	JSON* con = json->data.jsonData;
	if (!con)
	{
		json->data.jsonData = newData;
		return 0;
	}

	while (!con->next)
	{
		con = con->next;
	}
	con->next = newData;
	newData->pre = con;

	return 0;
}

int AddStringToArrayJSON(JSON* json, const char* string)
{
	JSON* newData;
	if (!json || json->type != ArrayType)
		return -1;

	newData = CreateJSON(getNULLString(), StringType);
	newData->data.stringData = GetStringDuplicate(string);

	JSON* con = json->data.jsonData;
	if (!con)
	{
		json->data.jsonData = newData;
		return 0;
	}

	while (!con->next)
	{
		con = con->next;
	}
	con->next = newData;
	newData->pre = con;

	return 0;
}

int AddObjectToArrayJSON(JSON* json, JSON* jsonData)
{
	JSON* newData;
	if (!json || json->type != ArrayType)
		return -1;

	newData = CreateJSON(getNULLString(), ObjectType);
	newData->data.jsonData = jsonData;

	JSON* con = json->data.jsonData;
	if (!con)
	{
		json->data.jsonData = newData;
		return 0;
	}

	while (!con->next)
	{
		con = con->next;
	}
	con->next = newData;
	newData->pre = con;

	return 0;
}

int SetIntJSON(JSON* json, char* name, int intData)
{
	JSON* node;
	node = FindJSON(json, name);
	if (!node || node->type !=IntType)
		return -1;

	node->data.intData = intData;
	return 0;
}

int SetStringJSON(JSON* json, char* name, char* string)
{
	JSON* node;
	node = FindJSON(json, name);
	if (!node || node->type != StringType)
		return -1;

	free(node->data.stringData);
	node->data.stringData = string;
	return 0;
}

int SetObjectJSON(JSON* json, char* name, JSON* jsonData)
{
	JSON* node;
	node = FindJSON(json, name);
	if (!node || node->type != ObjectType)
		return -1;

	FreeJSON(node->data.jsonData);
	node->data.jsonData = jsonData;
	return 0;
}

JSON* FindJSON(JSON* json, const char* name)
{
	JSON* con = json;
	while (!con)
	{
		if (!strcmp(con->name, name))
		{
			return con;
		}
		con = con->next;
	}
	return NULL;
}

void FreeJSON(JSON* data)
{
	JSON* tmp;
	if (!data)
		return;

	while (data->next != NULL)
	{
		free(data->name);
		//释放数据
		switch (data->type)
		{
		case IntType:
			//无需操作
			break;
		case StringType:
			free(data->data.stringData);
			break;
		case ObjectType:
			FreeJSON(data->data.jsonData);
			break;
		}
		tmp = data;
		data = data->next;
		free(tmp);
	}
}

/*获取JSON的值*/
//JSON对象
int getJSONValue(JSON* value, JSON* root, const char* name)
{
	JSON* con = root;

	while (con)
	{
		if (!strcmp(name, con->name))
		{
            if (con->type != ObjectType)
                return - 1;
			value->name = con->data.jsonData->name;
			value->next = con->data.jsonData->next;
			value->pre = con->data.jsonData->pre;
			value->type = con->data.jsonData->type;
			value->data = con->data.jsonData->data;
			return 0;
		}
		con = con->next;
	}
	return -1;
}

//int整数
int getIntValue(int* value, JSON* root, const char* name)
{
	JSON* con = root;

	while (con)
	{
		if (!strcmp(name, con->name))
		{
            if (con->type != IntType && con->type != BoolType)
                return -1;
			*value = con->data.intData;
			return 0;
		}
		con = con->next;
	}
	return -1;
}

//String字符串
int getStringValue(char* value, JSON* root, const char* name)
{
	JSON* con = root;

	while (con)
	{
		if (!strcmp(name, con->name))
		{
            if (con->type != StringType)
                return -1;
			strcpy_s(value, strlen(con->data.stringData) + 1, con->data.stringData);
			return 0;
		}
		con = con->next;
	}
	return -1;
}

//Bool布尔
int getBoolValue(int* value, JSON* root, const char* name)
{
	return getIntValue(value, root, name);
}

int StringToInt(char* string)
{
    int num = 0;
    unsigned int unum = 0;
    int temp;
    int temp2;
    int i;
    int index;
    int len;
    if (!string)
        return 0;
    len = strlen(string);

    if (string[0] == '-')
    {
        for (index = 1; index < len; index++)
        {
            temp2 = 1;
            switch (string[index])
            {
            case '1':
                temp = 1;
                break;
            case '2':
                temp = 2;
                break;
            case '3':
                temp = 3;
                break;
            case '4':
                temp = 4;
                break;
            case '5':
                temp = 5;
                break;
            case '6':
                temp = 6;
                break;
            case '7':
                temp = 7;
                break;
            case '8':
                temp = 8;
                break;
            case '9':
                temp = 9;
                break;
            case '0':
                temp = 0;
                break;

            }
            for (i = 0; i < (len - index - 2); i++)
                temp2 *= 10;
            num += temp * temp2;
        }
        num *= -1;
        return num;
    }
    else
    {
        for (index = 0; index < len; index++)
        {
            temp2 = 1;
            switch (string[index])
            {
            case '1':
                temp = 1;
                break;
            case '2':
                temp = 2;
                break;
            case '3':
                temp = 3;
                break;
            case '4':
                temp = 4;
                break;
            case '5':
                temp = 5;
                break;
            case '6':
                temp = 6;
                break;
            case '7':
                temp = 7;
                break;
            case '8':
                temp = 8;
                break;
            case '9':
                temp = 9;
                break;
            case '0':
                temp = 0;
                break;

            }
            for (i = 0; i < (len - index - 1); i++)
                temp2 *= 10;
            unum += temp * temp2;
        }
        return unum;
    }
}
