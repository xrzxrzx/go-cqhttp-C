# go-cqhttp-C
* 这是一个基于[go-cqhttp](https://github.com/Mrs4s/go-cqhttp)的C语言QQ机器人开源框架，大致可分为 API操作、
Event消息接收解析以及错误反馈三个部分，但只能在windows系统上使用。
> 制作这个的初衷是为了方便开发者能更加快捷的使用C语言制作出QQ机器人，
即使是对socket编程没有任何基础的编程小白也能在不具备socket相关知识
的情况下快速开发出机器人。

### 在使用此框架后，可以让开发QQ机器人的门槛下降一大截台阶，对于萌新十分友好，甚至可以拿来练练手，让自己有更多的开发经验。

***

因为本框架是基于go-cqhttp编写，所以在使用本框架时，需要同时使用
go-cqhttp开发文档来查询（其实直接看代码的注释也可以用，结构的成员都有详细的注释）。
本框架主要主要分为7个文件，分别为
### 实现go-cqhttp的API操作
* gocqhttp_API.h
* gocqhttp_API.c

### 实现go-cqhttp的事件接收
* gocqhttp_Event.h
* gocqhttp_Event.c

### 用于反馈go-cqhttp-C在使用时的错误
* gocqhttp_err.h
* gocqhttp_err.c

### 已完成的主函数框架
* main.c

### 此外还有两个拓展文件（来自于网络整理），用于转码
* URLcode.h
* URLcode.c

### 以及JSON解析库
* AnaJSON.h
* AnaJSON.c

### 使用方法
使用方法很简单，把main.c文件中注释为
`
//code
`
的地方改为自己的代码即可。
> 也可以用单独创建一个文件编写，再把code注释改为一个集成好的函数即可（在后面会有这样的演示）

可以注意到，在`//code`的上一行，会有
```c
msg.XXX_message = ...;
```
的代码（没有的话说明还没开发好），其实这一行代码是对接收到的消息进行解析的，因为cqhttp会发送很多种不同的事件消息（详情请参照cqhttp帮助文档），main.c里的代码，会对初始消息进行解析已确定他是哪种事件（详情看注释），接着就是通过上面这一行代码进行进一步解析，来获取我没最终需要的信息（如群消息事件我们就需要获取`char*`类型的群消息）<br>
`msg.XXX_message = ...;`本身是一个结构（详情见源代码注释），但其中包含了事件消息的所有信息，只需要访问它的成员即可（不理解的话先往后看）。

API的使用流程也很简单，分为以下两步：
1. 初始化发包（union）
2. 调用API
> API与go-cqhttp同名，包括发包

大致如下：
```c
API名_data data = New_API名;      //初始化发包（union）
API名(&data);                     //代入指针，调用API
```
调用API后，data的数据会被替换成API的返回数据，通过
`
data.recv_data
`
可以查看</br>

并会返回一个`cqhttp_err`的结构，这个结构包含了调用API时所产生的错误，可以用`cqhttp_err_out()`直接进行输出，结构大致如下：
```c
typedef struct
{
	cqhttp_err_list error;	//错误类型
	char function[70];		//错误函数
	int flag;				//是否拥有附加说明
	char instructions[500];	//附加说明
} cqhttp_err;		//错误封装
```
用户也可以通过`set_cqhttp_err()`快速初始化这个结构</br>
> 该结构的错误类型已经预设了几种，用户可以根据需要自行添加

使用起来大概如下：
```c
//输出错误
err = group_message_send(data);
cqhttp_err_out(err);
//自行构建错误
const char func[70] = "当前函数名";//这行代码尽量在每个函数头部都写一次（记得替换成正正的函数名）
cqhttp_err err = set_cqhttp_err(NULLError, func, 1, "空指针异常！");
cqhttp_err_out(err);
```
> 你可能会好奇卫生莫我会要你在每个函数开头都加上一个这个代码，这其实是为了你方便调试，能够方便的调佣`set_cqhttp_err()`

整体效果大概如下
```c
/*main.c 部分节选*/
case group_message:		//群消息
	cqhttp_err_out(set_cqhttp_err(None, func, 1, "recv group_message"));
	msg.group_message = group_message_event_analysis(event_msg);    
	err = group_message_switch(msg.group_message);  //这两行是自己添加的代码
	cqhttp_err_out(err);                            //
	break;
/*自己编写的文件*/
cqhttp_err group_message_switch(group_message_event_data msg)
{
	char func[70] = "group_message_switch";

	printf("来自群 %lu 中 %lu 的消息：%s\n", msg.group_id, msg.user_id, msg.message);
	if (!strcmp(msg.message, "自我介绍"))
		return introduction(msg);//自己实现的函数
	return set_cqhttp_err(None, func, 0, NULL);
}

/*自我介绍*/
cqhttp_err introduction(group_message_event_data msg)
{
	char func[70] = "introduction";
    
	send_group_msg_data data = New_send_group_msg(msg.group_id, "大家好！", 0);
	send_group_msg(&data);
	if (!strcmp(data.recv_msg.status, "ok"))
		return set_cqhttp_err(None, func, 1, "发送消息成功");
	return set_cqhttp_err(None, func, 1, "发送消息失败");
}
```
不过值得注意的一点是，C语言默认使用gbk字符集，但cqhttp只会接收utf8字符集或URL编码，否则会乱码，所以在API中包含了自动转码（转码函数包含在`URLcode.h`中），但这并不意味着开发者不需要注意到这个问题因为当你编写其他网络平台接口的时候很可能会乱码<br>
`URLcode.h`和`URLcode.c`的使用方法也很简单，大致如下：
```c
char* str = 转码函数(data/*原字符串*/);
//使用转码后的data，也就是str
free(str);//记得一定要free！！！因为转码函数转换后的字符串是重新从堆申请的
```

 ### 最后实在使用本开源框架时的注意事项
1.  如果没有十足的把握，请不要擅自修改框架的代码，否则很有可能会报错
2.  建议使用VS结合本框架来编写机器人
3.  如果在使用VS并结合本框架编写机器人，请进行以下设置：
> 打开菜单中的“项目”选项->选择当前项目的“项目属性”->
选择“C/C++”->找到“SDL检查”选项，将其改成“否”。
这样设置后便不会出现框架无法过编译的情况，但仍会出现
提示你将某些函数改成加了后缀"_s"的某些函数的Warning，
不理会即可
4. 如有遇到某些BUG，请加QQ群：721829413询问，这是使用go-cqhttp开发群器人的开发者交流群，go-cqhttp的开发者也在

如果遇到了某些go-cqhtpp的某些功能需要使用但本框架还未开发，请勿催促，
作者现在还是高中生，学校是半月休，所以开发进度会有些慢，但还请耐心等待
