# **go-cqhttp-C简介**
* 这是一个基于[go-cqhttp](https://github.com/Mrs4s/go-cqhttp)的C语言QQ机器人开源框架，大致可分为 API操作和
事件接收解析响应调用两个部分，但只能在windows系统上使用。
> 制作这个的初衷是为了方便开发者能更加快捷的使用C语言制作出QQ机器人（虽说我不觉得除了我还有谁会用C语言写机器人，这个项目算是自己想用C语言写机器人的所做的产物）

### 在使用此框架后，可以让开发QQ机器人的门槛下降一大截台阶，对于萌新十分友好，甚至可以拿来练练手，让自己有更多的开发经验。
>### 框架较之前有了很大的改进这一年的时间里，前前后后重构了三次，日志模块重写了两次，JSON解析模块还没全部完成就直接重写，事件的响应机制也重写了，删去了很多冗杂的东西，现在框架整体易用了很多

***

因为本框架是基于go-cqhttp编写，所以在使用本框架时，需要同时使用
go-cqhttp开发文档来查询（其实直接看代码的注释也可以用，结构的成员都有详细的注释）。
> **特别注意：**<br>
> 部署go-cqhttp时，配置文件85行以后直接按这个复制，如果熟悉本框架的话当我没说
> ```yml
># 连接服务列表
>servers:
>  # 添加方式，同一连接方式可添加多个，具体配置说明请查看文档
>  #- http: # http 通信
>  #- ws:   # 正向 Websocket
>  #- ws-reverse: # 反向 Websocket
>  #- pprof: #性能分析服务器
>  # HTTP 通信设置
>  - http:
>      # 服务端监听地址
>      host: 127.0.0.1
>      # 服务端监听端口
>      port: 5700
>     # 反向HTTP超时时间, 单位秒
>      # 最小值为5，小于5将会忽略本项设置
>      timeout: 5
>      # 长轮询拓展
>      long-polling:
>        # 是否开启
>        enabled: false
>        # 消息队列大小，0 表示不限制队列大小，谨慎使用
>        max-queue-size: 2000
>      middlewares:
>        <<: *default # 引用默认中间件
>      # 反向HTTP POST地址列表
>      post:
>      #- url: '' # 地址
>      #  secret: ''           # 密钥
>      - url: http://127.0.0.1:5701/ # 地址
>      #  secret: ''          # 密钥
> ```

本框架主要主要分为三大模块
### 实现go-cqhttp的API操作及事件接收响应调用
* gocqhttp_API.*
* gocqhttp_Event.*

### 机器人主体（机器人各功能实现）
* main.c
* robot.*
* function.*

### 工具模块
* AnaJSON.*
* URLcode.*
* Log.*
* Tool.*

<br>

# 使用方法
* 把项目克隆后直接在克隆的目录创建项目把源码文件全部添加进去（CMakeKists.txt文件我不会写，先这样用吧）
* 项目创建好后，别的文件都不要动，直接在function.*两个文件里声明并实现就行，功能实现完后按照功能类型再注册就完成了。

## **机器人各功能的事件响应**

go-cqhttp目前一共有五种类型响应，此框架目前仅实现了其中用得到的三种，分别为：消息事件、请求事件和上报事件（详情参考[官方文档](https://github.com/ishkong/go-cqhttp-docs/tree/main/docs/event)）
> 在`function.c`文件中的`initialize()`函数实现中，调用三个注册函数即可完成对功能的注册已经自动调用（功能函数需按照规定声明），这三个函数分别为：
>```c
>registerMessageFunction()//注册消息事件功能
>registerRequestFunction()//注册请求事件功能
>registerNoticeFunction()//注册上报类型功能
>```
>* 当某个功能注册失败时会返回-1<br>
>* 每个功能都要有一个名字，相当于她们的id，这个id为字符串类型`char*`，为三个函数的最后一个参数<br>
>* 三个函数的第一个函数，都是要注册的功能函数，他们的声明都有着各自的规则：
>   1. 消息事件 `void (*function)(MessageEventInfo)`返回`void`类型，有一个`MessageEventInfo`类型参数，这个类型包含了最基本的消息事件数据，相比cqhttp精简了很多，可以直接翻cqhttp的文档，或者直接看我从文档抄下来的注释
> 	2. 请求事件 `void (*function)(RequestEventInfo)`同上，只不过是换了个参数类型
> 	3. 上报事件`void (*function)(NoticeEventInfo)`同上同上

## **API的使用还是和以前一样，分为以下两步**：
1. 初始化发包（union类型）
2. 调用API
> API与go-cqhttp同名，包括发包

**大致如下：**
```c
API名_data data = New_API名;      //初始化发包（union）
API名(&data);                     //代入指针，调用API
```
调用API后，data的数据会被替换成API的返回数据，通过
`
data.recv_data
`
可以查看</br>


不过值得注意的一点是，C语言默认使用gbk字符集，但cqhttp只会接收utf8字符集或URL编码，否则会乱码，所以在API中包含了自动转码（转码函数包含在`URLcode.h`中），但这并不意味着开发者不需要注意到这个问题因为当你编写其他网络平台接口的时候很可能会乱码<br>
`URLcode.h`和`URLcode.c`的使用方法也很简单，大致如下：
```c
char* str = 转码函数(data/*原字符串*/);
//使用转码后的data，也就是str
free(str);//记得一定要free！！！因为转码函数转换后的字符串是重新从堆申请的
```
### **funciton.\*里有个小demo，可以参考一下**

---

## **日志模块**
日志模块较之前有了很大不同（听了大佬的建议做很多修改），现在模块可直接使用格式化字符串（printf一样的用法），也不用再声明一个错误类型<br>
>使用日志模块前要先调用`init_Log()`函数初始化（此框架已经调用过，不用再额外调用），日志会直接写入到`log`文件里

现在，日志模块输出分为三个等级，分比为：info、warn和err
```c
logInfo()
logWarn()
logErr()
```
这三个函数只会输出最基本的一些信息，及时间、日期、等级和日志，如果需要输出一些更完整的信息，比如：代码行、函数名、文件等，则需调用其对应的三个宏：
```c
logInfoAll()
logWarnAll()
logErrAll()
```
这三个宏的用法和三个函数一样，只是输出更完整了一些<br>
不过还有一个比较特殊的
```c
logPreset()
logPresetAll()
```
这个函数和宏接受的是预设好的错误类型参数，预设可以更具需求更改

<br>

# 已知但还未修复的BUG
**后天就开学了，修不了了**
1. JSON解析模块解析JSON对象时可能会出错，直接导致API `get_msg()` 无法使用
2. JSON解析模块解析CQ码时可能会出错
3. `NoticeEventInfo`类型的`comment`成员还不可用，因为我还没来得及写
4. API发送数据后如果没有收到响应可能会进入死循环，跳出循环机制还没加

<br>

# **最后感谢各位帮助了我的大佬**