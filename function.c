#include"Log.h"
#include"function.h"
#include"gocqhttp_Event.h"
#include"gocqhttp_API.h"

void initialize(void)
{
    //注册各个事件函数及一些自己想先初始化的功能
    //注册消息事件函数
    
    //注册请求事件函数
    
    //注册上报事件函数
    registerNoticeFunction(test, notify, "戳一戳");
}

/*测试功能*/
void test(NoticeEventInfo info)
{
    if (info.target_id == bot_id && info.subType.notify == poke)
    {
        send_group_msg_data msg = New_send_group_msg(info.group_id, "不要戳我了！", 0);
        send_group_msg(&msg);
    }
}
