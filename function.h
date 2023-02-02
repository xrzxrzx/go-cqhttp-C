#pragma once
#include"gocqhttp_Event.h"

static const int bot_id = 114514;

/*初始化，在初始化事件模块时调用（需包含注册各个事件函数）*/
void initialize(void);

/*测试功能*/
void test(NoticeEventInfo info);
