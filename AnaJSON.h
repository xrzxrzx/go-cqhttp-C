/**
* 此JSON解析库暂不支持 浮点类型解析
*/

#ifndef _ANAJSON_H_
#define _ANAJSON_H_

typedef enum _json_type
{
	IntType,	//整型
	//DoubleType,	//浮点型
	StringType,	//字符串类型
	BoolType,	//布尔类型
	ArrayType,	//数组
	ObjectType	//对象
}JSONType;		//JSON对象数据类型

typedef struct _json
{
	struct _json* next;
	struct _json* pre;
	char* name;
	JSONType type;
	union _json_data
	{
		struct _json* jsonData;	//对象数据和数组数据
        int intData;
		//double doubleData;
		char* stringData;
		int boolData;
	}data;
}JSON;

//NULL from stdio.h
#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

//创建JSON对象
JSON* CreateJSON(char* name, JSONType type);

//添加数据
int AddJSON(JSON* json, char* name, JSONType type);
int AddIntToArrayJSON(JSON* json, int intData);
//int AddDoubleToArrayJSON(JSON* json, double doubleData);
int AddStringToArrayJSON(JSON* json, const char* string);
int AddObjectToArrayJSON(JSON* json, JSON* jsonData);

//编辑数据
int SetIntJSON(JSON* json, char* name, int intData);
//int SetDoubleJSON(JSON* json, char* name, double doubleData);
int SetStringJSON(JSON* json, char* name, char* string);
int SetObjectJSON(JSON* json, char* name, JSON* jsonData);

/*JSON工具*/
//寻找JSON节点
JSON* FindJSON(JSON* json, const char* name);

//解析字符串到JSON
JSON* StringToJSON(const char* string);

//释放JSON数据
void DestoryJSON(JSON* data);
//释放JSON节点数据
void FreeJSON(JSON* data);

/*获取对象的值（返回错误码，0为正常，-1为异常）*/
//JSON对象
int getJSONValue(JSON*value, JSON* root, const char* name);
//int整数
int getIntValue(int* value, JSON* root, const char* name);
//String字符串
int getStringValue(char* value, JSON* root, const char* name);
//Bool布尔
int getBoolValue(int* value, JSON* root, const char* name);
//Array（数组类型）
//int getArrayValue(ValueType* value, JSON* root, const char* name);

//字符串转数字
int StringToInt(char* string);

#endif // !_ANAJSON_H_
