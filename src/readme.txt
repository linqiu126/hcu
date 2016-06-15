== 初始化并创建整个程序的体系架构
> 从MYC的WIP项目工程拷贝代码到本项目
> 设置Project Property, 以便编译环境可以正常运行
> project -> build configurartions -> set ative -> debug, ARM的调试环境以后再继续，等待E2E及Linux高手搞定
> add project -> properties -> Path & Symbol -> #include -> GNU-C -> add (/usr/include/libxml2)
> add project -> properties -> Path & Symbol -> #library -> 增加rt/pthread/xml2
> 编译试试，看看是否能生成完整的x86执行文件，并进行运行和Debug
> 调整系统字体以便程序不同行的程序对其: window -> preferences -> apperancet -> Colors and Fonts -> editor -> Courier Regular 10
> 将MYC之前的WIP项目程序代码全部借鉴过来，完成

==程序文件框架
> HCU工程
> SRC是包含所有源文件的目录，里面按照ComVm，L2Frame，L3App以及Webser多个子目录进行组织

》系统级主题任务
> hcu.h/c: HCU启动父进程，启动完成后即停止

》公共消息类任务
> commsg.h: 公共消息
> sysconfig.h: 系统级配置文件
> sysinfo.h/c: 定义系统参数和读取的API库
> configxml.h/c: 基于XML机制的系统配置库
> trace.h/c: 用于TRACE的API库
> vmlayer.h/c: VM虚拟机，简化上层程序调用的API库，主要包括TASK创建和消息通信机制
> mmc.h/c: 人机命令借口任务
> timer.h/c: 定时器任务

》L2底层通信接口模块任务
> L2comdef.h: 公共定义的结构体
> apigpio.h/c: GPIO接口的底层访问库
> apigprs.h/c: GPRS接口的底层访问库
> sps232.h/c: 串口的底层访问库
> sps485.h/c: 串口的底层访问库
> Ethernet.h/c: 以太网底层接口接收
> wifi.h/c: WIFI接口的底层访问库
> Usbnet.h/c: USB当后台网口使用时，底层接口接收
> con3g4g.h/c: 3G接口的底层访问库
> cloudcont.h/c: 连接后台SAE CLOUD，保持心跳及帧协议编解码
> modbus.h/c: 连接485接口并编解码MODBUS帧协议
> hwinv.h/c: 硬件状态和设备的监控及初始化处理
> hcuref.h/c: WIP项目参考代码
> Audio.h/c: 音频
> Camera.h/c: 摄像头
> Ble.h/c: 蓝牙
> Lcd.h/c: 显示屏+工控界面（界面直接使用WEB？）

》L3层3高层应用模块任务
> svrcon.h/c: 业务逻辑总控及处理
> sensoremc.h/c: EMC传感器业务逻辑处理
> sensorhumid.h/c: 湿度传感器业务逻辑处理
> sensorpm25.h/c: PM2.5传感器业务逻辑处理
> sensortemp.h/c: 温度传感器业务逻辑处理
> sensorwindspd.h/c: 风速传感器业务逻辑处理
> sensorwinddir.h/c: 风向传感器业务逻辑处理
> hsmmp.h/c: 高速多媒体处理任务

 》Web server: Http Web Server的简单服务应用

> readme.txt: 本文件，说明参考文档及开发进度说明


== 版本控制
> 软件内部控制版本，CURRENT_SW_RELEASE + CURRENT_SW_DELIVERY, 从20151102开始
> Git版本控制法，待定
> Release版本，待定
> Release ARM交叉编译版本，待定

== Update log: 2015 Nov.3, SW Version: XQ.HCU.SW.R01.002
> ZJL, 2015 Nov.3, CURRENT_SW_DELIVERY 2
= VM底层任务平台软件
> 借鉴下位机KEIL嵌入式程序样例和风格
> 创建任务和通信机制
> TRACE功能
> 简单的DEBUG控制功能
> 在Linux上创建线程，遇到不成功的问题，不明原因
> 在Linux上创建间线程和队列后，主/父进程负责最终清理，MAIN函数最后包含这部分，不然debug在第二次无法正常测试

== Update log: 2015 Nov.4, SW Version: XQ.HCU.SW.R01.003
> ZJL, 2015 Nov.4, CURRENT_SW_DELIVERY 3
= 任务的控制参数归类到VM层
= 消息传送与接受机制
= .h文件包含关系的清理
= 消息TRACE机制

== Update log: 2015 Nov.9, SW Version: XQ.HCU.SW.R01.004
> ZJL, 2015 Nov.9, CURRENT_SW_DELIVERY 4
=  优化程序和BUG FIX
= 任务创建和程序运行必须在ROOT下运行，或者给当前账户必要的权限，必然创建任务不会成功

== Update log: 2015 Nov.10, SW Version: XQ.HCU.SW.R01.005
> 消息结构体定义优化
> 消息发送接受机制测试
> MsgSend/MsgRcv函数只要出现一点点问题，就可能出现问题：输入的参数消息长度比Linux系统规定的长4个字节（整个消息体-TYPE），导致各种不可预知的问题
> 差错恢复机制，包括Task/Queue出错后，如何确保各个任务的自动恢复，目前暂时为做合理的处理，大部分都是直接退出程序体，未来需要仔细考虑恢复问题
> 测试过程中，由于各个任务都是无限工作的，所以运行测试完成后需要手动清理进程
> 子任务阻塞式工作模式工作的挺好！
> 子任务出错后，等待主任务的清理，而不是提前退出
> 硬件版本信息移到SysConfig.h文件中

== Update log: 2015 Nov.12, SW Version: XQ.HCU.SW.R01.006
> 初步FSM状态机，供大家品鉴

== Update log: 2015 Nov.13, SW Version: XQ.HCU.SW.R01.007
>  完整的FSM状态机，应用者只需要实现消息操作函数即可
>  全静态表格控制任务和状态机
>  任务创建自动化
>  问题：当创建第二个任务时，出现阻塞，可能跟任务的优先级等东东相关，TBD

== Update log: 2015 Nov.15, SW Version: XQ.HCU.SW.R01.008
>  修正FSM状态机的部分错误
> 初始化所有的任务和状态机初步内容
> 由于Linux只有一个定时器内核，为了支持多个应用任务的定时器，需要考虑单独构建一套定时器任务服务
> SVRCON主控发送消息初始化其它任务的机制完成

== Update log: 2016 Nov.15, SW Version: XQ.HCU.SW.R01.009
>  Timer机制搞定
>  第一个例子初步完成
> 基本做到以下两点：其它任务只需要按图索冀
> MSG_COM_INIT->任务做初始化，进入正常工作状态INITED
> MSG_COM_RESTART->任务做重新启动，将所有变量和状态以及QUEUE清空，进入正常工作状态INITED

== Update log: 2015 Nov.17, SW Version: XQ.HCU.SW.R01.010
> 高级信号量SIGALRM会打断MsgRcv以及SLEEP过程，使用技巧规避。据说他还会打断SOCKET的Accept函数，待确定
> 目前的妥协是，暂时使用Create_Timer函数在TIMER任务中生成定时器，并使用信号量方式，精度只有1S级别，1MS和10MS的精度暂时不考虑
> 定时器明显会打断SLEEP函数，目前使用循环规避。但1S精度的定时器信号量打断uSleep(1000)时，该函数直接返回-1,而不是剩下的微秒数，所以要注意
> 但是，如果uSleep(100)设置在100MS以下时，则没啥问题，基本上被打断的可能性降低到很低，还是可以使用的
》未来还需要仔细重新考虑实时定时器的设计问题
> 完善第一个例子PM25的状态机，给其他传感器搭建框架
> 完善MODBUS/CLOUD的程序框架
> 底层3G/SPS写STUB空函数，以便调佣和调试
> 准备MMC及HWINV任务，特别是硬件清单任务，它将是硬件底层状态维护以及PnP功能的基础

== Update log: 2015 Nov.18, SW Version: XQ.HCU.SW.R01.011
> 增加MMC以及HWINV两个任务模块，建立起程序框架
> 继续优化PM25业务逻辑，MODBUS/CLOUD两边的程序模拟数据更为接近实际情况
> 周期模式完成，瞬时模式留待未来完成
> 控制命令字还是个不确定性的问题

== Update log: 2015 Nov.19, SW Version: XQ.HCU.SW.R01.012
> 调试圆桶算法完成，基本上可用，重新将最大记录数调整为20000条
> 6个传感器全部启动了，基本上开始跑起来！

== Update log: 2015 Nov.20, SW Version: XQ.HCU.SW.R01.013
> Modbus Pack/uppack func

== Update log: 2015 Nov.21 SW Version: XQ.HCU.SW.R01.014
>  融合找到现成的串口程序以及Http Web Server程序

== Update log: 2015 Nov.22 SW Version: XQ.HCU.SW.R01.015
>  Add Linux Target Arm Project Config
>  Define TRUE/FALSE MACRO in hcu.h

== Update log: 2015 Nov.23 SW Version: XQ.HCU.SW.R01.016 //ZJL
> 增加各个底层接口的接收任务机制，采用阻塞式
> 创建ETHERNET, USBNET, WIFI, 3G4G, HSMMP, BLE, SPS232, SPS485任务模块框架
> 一种最为简单的硬盘存储传感器记录的机制搞定，以后可以逐步丰富：碎片文件/内容按照时间轴/记录数量/定时清理等等，待完成

== Update log: 2015 Nov.24 SW Version: XQ.HCU.SW.R01.017 //ZJL
> 碎片文件目录的创建和更新机制
> 碎片文件名的创建以及定时更新机制
> 碎片文件的写入，读取和清理删除（比如超过半年的周期）未来实现
> 高精度时钟10MS，采用临时启动线程的方式，基本搞定，后面通过摄像头HSMMP处理任务，看看效果，至少它没有打断SLEEP，说明其工作没大问题
> TRACE机制丰富化：MSG_TIME_OUT需要被过滤，不然TRACE消息太多，MSG_TRACE_ON = 1 (ALL) or 2 (ALL but TIME_OUT)，未来需要实现更多丰富的TRACE机制

== Update log: 2015 Nov.25 SW Version: XQ.HCU.SW.R01.018 //ZJL
> 基本的音视频程序框架， 完善整个处理流程的子函数及功能分解
> 针对TI MX6设备的设备名定义到L2COMDEF.H中，以便HWINV定期扫描以及其它应用直接使用，避免不同应用之间重复定义以及冲突
> 接收到视频图像或者视频流以后的处理，等待视频摄像头物理硬件处理过程（CAMERA模块）完善后再行处理
> 视频流的后处理：采用标准MPEG4或者H264标准格式，碎片存入硬盘，碎片传后台，或者视频流传后台？

== Update log: 2015 Nov.29 SW Version: XQ.HCU.SW.R01.020 //MYC
> Correct DebugTracePrint issue of file name length not same
> Correct Debug (x86, not for ARM) Project Configuration, remove libxml2 lib directory to remove link warning
> API to Connect USB GPRS via HW-E261 Card
  1. Files modifiled: sysinfo.h and sysinfo.c (to be cleaned?) 
  2. Data struacture defined in sysinfo.h (to be cleaned)
  3. API added forexrternal Call:
		UINT32 HcuGetSysInfo(SysInfo_t *si);
  		UINT32 UsbUmtsPppInit();
		UINT32 UsbUmtsPppdStart();
		UINT32 UsbUmtsPppdSwShutdown(); 
  		UsbUmtsCardScan();  => To be complete, as currently no USB is scanned, HW-E261 is chosen as default!!!
		UINT32 IsGprsPppdOk();
		UINT32 IsNetItfExist(char *itfname);
		void KillPppdProcess();
  4. At system startup, bring PPP for HW-E261

		//INIT the whole system
		void hcu_vm_system_init(void)
		{
			...		
			/* MYC: Start to initialized USB UMTS CARD */
			UsbUmtsPppInit();
			UsbUmtsPppdStart();
			...
		}
   5. Note1: Ctrl+C (TERM SIG) will kill hcu process, resulting in pppd killed, and all ppp interface will be down
   6. Note2: Tested in TI A8
   7. Note3: TARGET_LINUX_ARM needs to be added	
   
== Update log: 2015 Nov.29 SW Version: XQ.HCU.SW.R01.021 //LZH
> CURL OK

== Update log: 2015 Nov.29 SW Version: XQ.HCU.SW.R01.022 //MYC
> Optimize USB UMTS CARD
> CURL lib added in x86 Debug Build Config (Not working yet for ARM A8, Needs to compile ARM lib, TO DO)
  In x86 ubuntu:
		apt-get install curl libcurl3 libcurl3-dev php5-curl
		apt-get install libcurl4-gnutls-dev
  TO DO: TO ADD CURL LIB in ARM, or not possible to compile in Target!!!!   

== Update log: 2015 Dec.1 SW Version: XQ.HCU.SW.R01.023 //ZJL
> 将USB启动程序放到HWINV模块中去
> 完善GPS和LCD基础模块任务
> 基本确定连接建立的方式，完善CLOUTCONT模块相应功能
> 移动webser目录到src目录下，以便统一管理源代码

== Update log: 2015 Dec.2 SW Version: XQ.HCU.SW.R01.024 //ZJL
> 完善后台通信

== Update log: 2015 Dec.2 SW Version: XQ.HCU.SW.R01.025 //MYC
> 尝试UMTS通信和交叉编译

== Update log: 2015 Dec.7 SW Version: XQ.HCU.SW.R01.026 //ZJL
> 调同与后台的ETHERNET通信方式
> 后台通信底层与后台云SAE联调完成
> 后台数据层格式化
> EMC定时器改为6Second，各个传感器任务对应的后启动时间从随机最大60second减少为6second

== Update log: 2015 Dec.8 SW Version: XQ.HCU.SW.R01.027 //MYC
> //MYC 2015/12/08, For 335xD, RS485 is ttyO2 !!!
> "/dev/ttySAC0" changed to "/dev/ttyO0"
> #define PM25_TIMER_DURATION_PERIOD_READ 60 to #define PM25_TIMER_DURATION_PERIOD_READ 6, only for test
> fd = open(HCU_DEFAULT_DEVICE_USB_CAMERA, O_RDONLY, 0); to fd = open(HCU_DEFAULT_DEVICE_USB_CAMERA0, O_RDONLY, 0);
> Add Linux_Arm_A8 target lib for libcurl Build Configuration, target can be built
> Check log file "hcu.R01.027(PM25_OK).log for target run result (Video Camara also inserted)

== Update log: 2015 Dec.8 SW Version: XQ.HCU.SW.R01.028 //ZJL
> change PM25_TIMER_DURATION_PERIOD_READ back to 6second
> 增加中环保ZHB标准的解码功能，后台接口协议目前支持两种：XML和ZHB，需要通过SYSCONFIG.H中的预定义来选择
> 修正各种传感器离线数据都存为PM25的bug
> 定义完整各个传感器的操作命令字，并完善Modbus.c/Sensorxxx.c中对于这些操作命令足的处理框架，以备接收从后台来的操作命令并进行相应的处理

== Update log: 2015 Dec 9 SW Version: XQ.HCU.SW.R01.029 //ZJL
> 控制命令收到后，传递到外设的功能，暂时没有详尽实现，待完善，未来可以跟后台的任务一起，一个模块一个模块的实现
> 后台基于以太网口的编解码协议全部完成，包括自定义XML格式和中环保两种格式，需要通过SYSCONFIG.H中的全局变量进行自由切换
> 为了HCU调测的方便，需要后台同时支持XML和ZHB两种协议栈，而且还需要根据关键字进行自动判定，这个对于后台来说不难，有一定的工作量，但意义重大

== Update log: 2015 Dec 11 SW Version: XQ.HCU.SW.R01.030 //ZJL
> 确定前后台的握手信号，用在心跳检测过程中，或者链路证实中
> 后台接口的链路层握手信号，只是在链路层终结，不带入业务L3中去
> 需要跟后台约定好这个格式和过程：心跳检测，后台必须回；普通数据上报后，后台可以回证实，也可以不回
> 基于中环保ZHB的协议，明确的有帧号，但基于自定义的XML格式却不存在帧号，这个需要考虑下，是否需要增加这个机制
> XML格式中，FE00表示发送和接受的心跳指示
> ZHB格式中，数据区长度=0表示心跳帧，未来可能需要修改命令指示，这部分等待协议的明确
> 出现的潜在问题：Ubuntu14中，接收出现阻塞现象，导致CLOUD模块的接收消息区满的情形，这跟Ubuntu15的状态完全不一样，而且U14运行时，也不是每一次都如此。
> 设置CURL接收的TIME_OUT为1800ms(实验过，太短会导致接收超时不成功，一般的延迟在1200ms左右)，暂时解决该问题，U15下依然正常，但U14下依然不正常，需要再调查


== Update log: 2015 Dec 15 SW Version: XQ.HCU.SW.R01.031 //ZJL
> 所有的后台数据MsgType都改为hcu_text/hcu_command/hcu_heart_beat，以跟后台的要求保持一致

== Update log: 2015 Dec 18 SW Version: XQ.HCU.SW.R01.032 //ZJL
> 增加FFMPEG库
> ZHB中环保协议中的系统命令的定义  "ZHB_HRB"   "ZHB_NOM"，未来需要等待国家协议更为详细的定义后再行完善

== Update log: 2015 Dec 19 SW Version: XQ.HCU.SW.R01.033 //ZSC
> 完善GPS工作机制

== Update log: 2015 Dec 20 SW Version: XQ.HCU.SW.R01.034 //ZJL
> 将L2COMDEF.h中的启动L3模块定时器改为60秒，以延迟不同任务模块启动的时间
> 将EMC的时钟调整到60秒，以正常化EMC的行为
> 将CLOUD连接后台的心跳定时器改为15秒，以便合理化其验证时长，另外，还改为XML格式，中环保ZHB格式待后台完善后继续调试
> 遗留问题：偶尔会出现usleep()的无效问题，可能还是信号量的不合理设置，需要待调查
> 遗留问题：偶尔出现后台发送不成功，及时再ETHERNET的情况下，非常值得调查起原因
> 遗留问题：目前情况下，一旦一次发送失败，该数据将丢失，而不是保存下来下次继续发送，未来需要改进，以便改善数据的准确性
> 其它通信模式，包括3G/4G的情形

== Update log: 2015 Dec 21 SW Version: XQ.HCU.SW.R01.035 //ZJL
> 集成FFMPEG的库函数，更新了项目工程中的库，并使用L4版本VMware，编译出.so库，否则无法成功

== Update log: 2015 Dec 23 SW Version: XQ.HCU.SW.R01.036 //ZJL
> 首次集成FFMPEG功能进来，实现了USB摄像头在x86上的正常工作，并生成了本地标准格式文件YUV格式，以及H264和AVI格式
> 摄像头的工作方式应高层要求的方式进行
> 后台->CLOUD->HSMMP->CAMERA->HSMMP->CLOUD->后台的消息流程及程序框架建立好了，具体内容，包括后台针对高速视频的控制命令，待完成
> 摄像头初始化时工作一次，是为了测试，未来完善后，将工作在受控模式下

== Update log: 2015 Dec 24 SW Version: XQ.HCU.SW.R01.037 //ZJL
> 月份bug修正，目前的月份是从0-11，需要改为1-12，符合正常的逻辑
> 受控的视频处理完整逻辑和过程
> 视频满足条件时，记录文件将移到视频服务器目录，先通过1/10次数定时来完成，以后将通过后台配置来制定触发
> 视频链接以标准格式（XML/ZHB)发送到后台待完成

== Update log: 2015 Dec 25 SW Version: XQ.HCU.SW.R01.038 //ZJL
> 完善视频发送到后台的XML/ZHB编解码功能
> 为了调试视频，调整定时器 #define TIMER_DURATION_REDUCE_COLLAPTION_IN_1_MINUTES 6  //shall be 60second，以便HSMMP尽快到来
> #define HSMMP_TIMER_DURATION_PERIOD_CAMERA_READ 20 //should be 600second = 10分钟, in second
> #define HSMMP_CAMERA_CAPTURE_DURATION_DEFAULT 1 //10 second
> 打印打开，也会影响HSMMP短时定时器的超时
> 当前只能支持H264格式，MKV/AVI格式还不正常
> 视频崩溃过于频繁，需要仔细调试参数
> 如果第一次视频没有采集成功，而相应的现场没有清理，必然造成第二次程序崩溃
> 考虑到CAMERA程序过于容易崩溃，需要考虑是否可以将其做成一个独立的进程
> 视频多次启动/停止的实验需要继续进行

== Update log: 2015 Dec 27 SW Version: XQ.HCU.SW.R01.039 //ZJL
> 优化HSMMP与CAMERA之间数据传递GPS/TIME的问题，以便为CAMER单独变成进程做准备
> 修正PM25在CLOUDCONTROL中pack错误的问题
> 单独进程尝试

== Update log: 2015 Dec 28 SW Version: XQ.HCU.SW.R01.040 //ZJL
> 增加噪声模块的程序框架，完全参照了HUMID等传感器的程序方式，EQUIPID等继续保留
> 优化VMLAYER的消息TRACE函数，未来增加消息时，必须增加VMLAYER.C中消息的描述，不然TRACE MESSAGE会出现解析错误
> XML数据发送给后台的时间戳从分钟颗粒度改为瞬时秒为单位的颗粒度
> 噪声制造周期性的假数据，以便后台测试

== Update log: 2015 Dec 28 SW Version: XQ.HCU.SW.R01.041 //ZJL
> 进程实验
> 创建了3种启动进程的方式：单进程/双进程和多进程，由SYSCONF.H进行控制
> 多进程中，打算让SVRCON单独成为一个WATCHDOG进程，但由于它依赖TIMER的运行，而且TIMER还未改造为远程模块，还得依靠共享内存变量的运行，故而模式3并不能正常工作，未来需要进一步完善

== Update log: 2015 Dec 29 SW Version: XQ.HCU.SW.R01.042 //ZJL
> 完善进程机制
> 增加心跳机制，让HCUMAIN发挥余热，当定时器使用，产生定时5S的心跳信号给总控SVRCON模块
> 消息发送接收机制中，没有使用全局变量，而是直接使用了QUEID通过KEYID查询的方式获取，这可以避免多进程之间共享数据不同步的麻烦
> 多进程机制完全在HCUMAIN中完成，DEFAULT选择还是单进程
> 将CAMERA及启动任务的定时时长改为标准值，另外，还将各个传感器的时钟恢复到标准

== Update log: 2015 Dec.30 SW Version: XQ.HCU.SW.R01.043 //ZJL
> 完善所有任务模块状态机的初始化状态：可以接受其它模块的INIT消息，而不只是SVRCON主控
> 完善接收到INIT_FB消息的接收状态机，使得任何模块在任何时候都具备处理该消息的能力
> 完善心跳机制，ACTIVE等工作状态下，需要具备处理该消息的能力，暂时只触发CAMERA的心跳消息发送
> sleep()频繁被高级信息打断，导致出错告警太多，使用rand()方式抑制掉99.9%，留下0.1%的报告量，未来再研究解决方案
> hcuerr.log文件机制完成，可以记录所有的运行错误信息，包括RESTART中的信息，通过打印的方式记录下来
> 优化TRACE机制中，MSG_ID_TIME_OUT/MSG_ID_HEART_BEAT/MSG_ID_HERAT_BEAT_FB，以便均不显示出来
> 完善软件RESTART机制：所有模块增加错误计数器，错误导致计数器增加，到达门限后，自己给自己发送RESTART消息，从而导致重启
> TIMER涉及到所有模块的正常运行，它的错误重启，将导致整个系统的重启
> 高层业务模块和CLOUDCONT模块靠定时器挂载错误检查，L1/L2模块的错误计数器门限检查挂载在心跳信号上
> 心跳开放了L3模块以及重要的L2模块，其它的均采用下行调用/上行消息的形式，并不适合于使用心跳消息的方式

== Update log: 2015 Dec.30 SW Version: XQ.HCU.SW.R01.044 //ZSC
> change the series port operation mode from blocking to non-blocking 
> move the series port initial procedure to MODBUS startup in order to avoid  init series port frequentely

== Update log: 2015 Dec.31 SW Version: XQ.HCU.SW.R01.045 //ZJL
> 修改支持的编译链接库：去掉SDL/SDLmain，增加SDL2
> 完善视频，修正BUG，经过测试，稳定性很好，可以连续工作长时间，而且其帧数定时精度在2.5%（100秒测试结果）以内，非常好
> 将串口初始化放到MODBUS初始化的最后，而且不能返回差错，不然会出现逻辑错误
> 该版本支持的U14，需要卸载SDL1.2，并重新安装SDL2.0，方法
  》0. apt-get remove sdl1.2*
  》1. 官网http://www.libsdl.org/download-2.0.php下载源代码后
  》2. 编译安装到/usr/local中 (./configure --enable-shared --prefix=/usr/local, make, make install
  》3. 重新编译安装FFMPEG2.8.4，不然会出现部分库API找不到的现象
  》4. 然后再安装apg-get install mplayer，可以支持视频*.yuv格式
  》最终执行apt-get autoremove，清理无用的依赖库
> 基于以上的修改，很快会搞定U14/U15的L5版本，并上传云端，以方便懒汉的使用 
  
== Update log: 2016 Jan.1 SW Version: XQ.HCU.SW.R01.046 //ZJL
> 支持噪声仪MODBUS协议，细节中的寄存器地址以及数据长度，暂时待定
> 同时支持MODBUS/SPS485和AUDIO两种采样设备，缺省选择MODBUS

== Update log: 2016  Jan.3 SW Version: XQ.HCU.SW.R01.047 //ZJL
> TRUE/FALSE微调
> CAMER.C / REFRESH-RATE bug微调
> SDL做成#ifdef模式，以免影响FFMPEG库和下载版本
> 心跳时间间隔从15秒改为60秒
> 去掉SDL2后，这是第一个能成功下载到目标板SS35D的版本，配合相应的镜像是“ARM335D/L1-下载工程版本”
> 完善SDL2打开，关闭，以及去掉三种情况下的稳定性，并修正相应BUG

== Update log: 2016  Jan.4 SW Version: XQ.HCU.SW.R01.048 //ZJL
> L2FRAME目录结构调整
> Rename cloudcont -> cloudvela 船帆座
> Rename camera->avorion AV层2帧协议处理 猎户座
> 增加CAMERA任务模块
> rename audio->sosvirgo 串口通信模块 处女座
> 增加microphone模块
> 优化抑制了部分WARNING

== Update log: 2016  Jan.5 SW Version: XQ.HCU.SW.R01.049 //ZJL
> 增加L3UI/L4BI的程序模板
> 修改L4WEBSER到L0WEBSER，更为正确的层次结构
> 创建所有的L2DBI程序框架
> 设置两个基本的API函数框架，以便与JT进行对测

== Update log: 2016  Jan.6 SW Version: XQ.HCU.SW.R01.050 //ZJL
> 将L2DBI下放到L0DBI，更准确的反映其定位和层次结构
> 规整HWINV中有关系统配置的数据表单
> 基于L5镜像，安装mysqlclient, sudo apt-get install libmysqlclient-dev
> 项目编译，需要增加库mysqlclient
> LAMP下的数据库环境，必须搭建好testuser/123456/hcudb的数据库环境，不然这部分数据库表单将运行不成功
> 以后每次DELIVERY，数据库将直接导出，编译环境时，必须及时将该数据库导出部分导入到数据库环境中，不然会出问题

== Update log: 2016  Jan.7 SW Version: XQ.HCU.SW.R01.051 //ZJL
> 修改MODBUS的功能码，以适应噪声功能码=04的情况
> 增加内存数据/数据库/硬盘数据存入的全局控制变量
> 全局配置表将分为两种类型：工程参数和全局性数据。我们只是将工程性参数移到数据库控制表中，而对于全局性配置数据，依然放在SYSCONF.H中
> 全局数据表/API/数据库初步搞定，暂时放在HWINV中进行测试，未来可以移到HCU.C启动模块中
	- 全局控制表的TRACE模块比较麻烦，暂时未完成
	- 全局变量表暂时未起作用，需要一步一步的来完善，一次性改入，可能造成回归性问题
> 传感器的操作API函数基本完成，数据库表单建立好了
> 传感器的数据库存储功能，已经挂载到传感器L3APP任务中去了，未来可以通过查看数据库，观察各个传感器运行的成果

== Update log: 2016  Jan.11 SW Version: XQ.HCU.SW.R01.052 //ZJL
> 创建sysengpar.h，里面存储所有的工参参数
>公参强行未来可以设计成不同项目（项目号）为基础
>工参在HCU中进行初始化，工参在HWINV中进行定期扫描
>数据库和文件系统的定时扫描，扫除过期数据，功能完成并测试试用过了 

== Update log: 2016  Jan.12 SW Version: XQ.HCU.SW.R01.053 //ZJL
> 工参调试，替换所有的工参成为全局变量，并让其在程序中起作用
> 工参中的TRACE_DEBUG_ON分成全关=0/全开=1=INF/2=NOR/4=IPT/8=CRT/16=FAT不同等级的情形，未来可以在程序中进行调整，方便将TRACE分为不同输出等级，从而方便调试
> 创建sysversion.h，将更新的历史记录单独存在其中

== Update log: 2016  Jan.13 SW Version: XQ.HCU.SW.R01.054 //ZJL
> 建立MESSAGE TRACE的系统性方法
> 使用数据库进行配置参数
> TRACE采用TRACEMODE总控，TraceModule和TraceMsgId进行细控的方式
> 特殊应用技巧：MIN=0的初始项，用于全配置


== Update log: 2016  Jan.14 SW Version: XQ.HCU.SW.R01.055 //ZJL
> 将LAMP下的Yii2Basic集成进来，方便眼睛看着，唯一的目的是为了调试方便
> 该版本必须跟U14-L7相互配合，不然目录不对，会遇到麻烦
> 基本表单均生成完成，并生成主页，帮助开发调试者使用
> 系统启动暂时启动了，可通过配置控制表单将起去掉
> 调测神器（消息级，模块级）均具备，可以通过主页进去使用
> 准备了U7-Ubuntu版本，用于开发调试，所有开发内容均具备，方便懒汉

== Update log: 2016  Jan.15 SW Version: XQ.HCU.SW.R01.056 //ZJL
> HSMMP模块的完善，HSMMPLINK等内容存入数据库
> HSMMP相应的数据库模块，数据库操作界面均完善
> 优化全局消息格式，尽力复用基础数据格式定义，从而为以后完善或者优化这些基础数据格式打下基础
> 视频数据存入数据库的尝试
> HSMMP中数据库存储，遇到特别奇怪的问题：数据库存成功了，但程序崩溃了，暂时不存入HSMMP数据库中。跟EMC等相比，只是多了一些LINK等CHAR的字段，而其它EMC均成功

== Update log: 2016  Jan.16 SW Version: XQ.HCU.SW.R01.057 //ZJL
> 修改噪声工作值，4B，而不是2B
> 统一修改汇报值给后台，增加数据域的格式，表示定标
> 统一修改本地界面和数据库针对的格式域改变

== Update log: 2016  Jan.16 SW Version: XQ.HCU.SW.R01.058 //ZJL
> 更新后台汇报格式，增加格式域控制

== Update log: 2016 Jan.19 SW Version: XQ.HCU.SW.R01.059 //ZJL
> 创造一个SYS PERFORMANCE的目录，先搞定error counter的定期存入数据库功能，以及界面展示
> 修正BUG：
	==>  HSMMP中数据库存储，遇到特别奇怪的问题：数据库存成功了，但程序崩溃了，暂时不存入HSMMP数据库中。跟EMC等相比，只是多了一些LINK等CHAR的字段，而其它EMC均成功
	==> #define DBI_MAX_SQL_INQUERY_STRING_LENGTH 1024
	
	
	== Update log: 2016 Jan.22  SW Version: XQ.HCU.SW.R01.060 //ZSC
> Add Sorcket handling(Server) to receive the control command from SAE Cloud
> Add control cmd operation(PowerOnOff) for PM25 sensor, template for all others command operation on the sensors
> Modify file: modbus.h, modbus.c, l1comdef.h, cloudvela.h, cloudvela.c, ethernet.c, sensorpm25.h, sensorpm25.c


	== Update log: 2016 Feb.6  SW Version: XQ.HCU.SW.R01.061 //ZSC
> Add CMD polling
> Add all control cmd operation  for PM25 sensor
> Modify file: time.h, modbus.c, l1comdef.h, cloudvela.h, cloudvela.c, sensorpm25.c

== Update log: 2016 Feb.7  SW Version: XQ.HCU.SW.R01.062 //ZJL
> 试图增加SqlLite函数,并通过dbi_emc函数建立了第一个式例
> Step1: 给firefox安装sqlite manager插件，并创建hcudb.sqlite的数据库，放在src目录下
> Step2: install sqlite3 for linux
  -> apt-get install sqlite3
  -> Download from http://www.sqlite.org/download.html
	$tar xvfz sqlite-autoconf-3071502.tar.gz
	$cd sqlite-autoconf-3071502
	$./configure --prefix=/usr/local
	$make
	$make install
> 注意:更新版本时,需要更新sysversion.h中的版本标识,不然软件版本(SW Delivery)并不随着版本的升级而增加.
> SQLite3的样例,均放置在L0DBI目录中,包括SQLite3.txt
> SQLite3的数据库建立在 #define HCU_DB_SQLITE3_LOC_AND_FILE "/home/hitpony/workspace/hcu/hcudb.sqlite",它由文件进行控制与管理
> SQLite3的表单已经全部创建好了,数据记录备份在/home/hitpony/workspace/hcu/hcudb.sqlite

== Update log: 2016 Feb.17  SW Version: XQ.HCU.SW.R01.063 //QL
> Import L3UI的V3本地界面部分
> 4S轮不同传感器的数据展示,完美呈现
> 拷贝到本地/var/www/html/yii2basic后,一定要修改文件属性,不然yii2框架执行会出错的

== Update log: 2016 Feb.22  SW Version: XQ.HCU.SW.R01.064 //ZSC
> correct some bugs for CMD polling
> HCU test passed for all PM25 command


== Update log: 2016 Feb.24  SW Version: XQ.HCU.SW.R01.065 //ZSC
> adjust GPS receive timer 
> save sensor config data to local DB
> adjust HCU msg format: move backtype after equid

== Update log: 2016 March. 01  SW Version: XQ.HCU.SW.R01.066 //MYC
> Update Project Configuration for Debug (X86) and Linux_Arm_A8
For x86:
 - Install LAMP (Apache, Mysql, Php)
 - Instal sqlite3
For ARM:
 - Install mysql_arm, ncuses5_arm, sqlite3_arm
 
!!! Before run in target arm linux, make sure set LD_LIBRARY_PATH 
root@ok335x:/home/forlinx# LD_LIBRARY_PATH=/usr/local/mysql_arm/lib:/usr/local/ncurses5_arm/lib:/usr/local/arm/linux_arm_a8/lib:/usr/local/arm/linux_arm_a8_curl/lib:/usr/local/pcre_arm/lib:/usr/local/zlib_arm:/usr/local/libpng_arm/lib:/root/ffmpeg_build_arm/lib

== Update log: 2016 Mar.8  SW Version: XQ.HCU.SW.R01.067 //ZSC
> send GPS EW/NS to Cloud

== Update log: 2016 Mar.13  SW Version: XQ.HCU.SW.R01.068 //ZSC
>report local time to align with unpack method in Cloud


== Update log: 2016 Mar.16  SW Version: XQ.HCU.SW.R01.069 //ZSC
> add EW & NS datafield for local DB for alll sensors 

== Update log: 2016 Mar.18  SW Version: XQ.HCU.SW.R01.070 //ZSC
> add OD config for sensor timer/CMD polling timer/heath check timer 

== Update log: 2016 Mar.21  SW Version: XQ.HCU.SW.R01.071 //ZJL
> 修改数据库中EW/NS为小写ew/ns，以防止潜在的出错机会
> 数据库备份，统一到HCU根目录下来
> 更新L3UI以及本地WWW/HTML下的界面，并重新引入到该源程序目录下src/l3ui
> 为了防止多个项目选项confuguration，去掉了x86这个老的选项，重新固定在x86->debug上

== Update log: 2016 Mar.23  SW Version: XQ.HCU.SW.R01.072 //ZJL
> 定时器以及串口配置在数据库和界面上的生成
> 由于该数据部分并没有改掉数据库，导致HCU程序下载到335D班子上之后，出现segmentation failure崩溃，完善后该问题应该不再出了。

== Update log: 2016 Mar.24  SW Version: XQ.HCU.SW.R01.073 //ZJL
> HWINV中增加SYSINFO，去掉CPU信息，优化而不打印，只是自动扫描
> 优化掉打印中多出的\n，以方便打印信息的整体观测
> 增加判断消息队列满的处理方式， func_hwinv_scan_message_queue(void)
> 一旦满，定时扫描程序将重新启动该模块。满的判定就在message_send函数中，所有目标发送不成功，都认为是目标模块的队列满



== Update log: 2016 April 09  SW Version: XQ.HCU.SW.R01.074 //ZSC
> Workaround solution to fix the bug(remove the first two byte of cmd response received from aiqiyun： 13 10 )
>  Fix the bug (engineer parameter)


== Update log: 2016 April 15  SW Version: XQ.HCU.SW.R01.075 //ZSC
> change to non-blocking & asynchronous read over serial port
> remove the  redundant  while loop for some tasks


== Update log: 2016 April 16  SW Version: XQ.HCU.SW.R01.076 //ZSC
> Fix the bug: HCU startup(remove Standard input config for serial port)


== Update log: 2016 April 16  SW Version: XQ.HCU.SW.R01.077 //ZSC
> add HSMMP timers in sysengpar config

== Update log: 2016 April 25  SW Version: XQ.HCU.SW.R01.078 //MYC
> add Project Config RasberryPi (for Ubuntu MATE 15.x)

== Update log: 2016 May 01  SW Version: XQ.HCU.SW.R01.079 //MYC
> add Project Config RasberryPi (for Raspbian Jessie)

== Update log: 2016 May 07  SW Version: XQ.HCU.SW.R01.079DEMO //JT/ZSC/QL/LZH
> For Project Config RasberryPi (for Raspbian Jessie)
> hcu access to hcudb in mysql OK, modify two things: 1) via root; 2) grant testuser for hcudb access right in mysql (not used, keep root)
> add PM25 sensor read function via UART0 in Rapsberry Pi
> add MACRO TARGET_RASPBERRY_PI3B, change in two files: 1) commsg.h for mysql.h 2) sps485.c for UART0 (/dev/ttyAMA0)
> update hcudb.sql in the hcu dir (make sure it is imported to mysql)
> Update yii2basic in /var/www/html/yii2basic, copy to worksapce/hcu/l3ui for backup
> GPS is not working in 79DEMO

== Update log: 2016 May 10  SW Version: XQ.HCU.SW.R01.080 //ZJL
> 增加定义CURRENT HW TYPE及HW MODULE
> 增加任务模块框架：GPIO，I2C，SPI，PWM，ADC，SWITCH（开关量）， RELAY（继电器），MOTOR（马达），SYSPM（性能统计）
> 增加L3传感器框架：Airprs（气压），Co1（一氧化碳），Lightstr（光照），Alcohol（酒精），HCHO（甲醛Formaldehyde），toxicgas（有毒气体）

== Update log: 2016 May 10  SW Version: XQ.HCU.SW.R01.081 //ZJL
> 增加DBI对应传感器的程序函数框架
> 更新并完善L3UI的本地界面
> PM25SHARP的最新界面貌似不在版本中，需要从别处倒入

== Update log: 2016 May 11  SW Version: XQ.HCU.SW.R01.082 //ZJL
> L3新增传感器时钟处理，包括工参配置
> SYSPM模块的处理：一个小时出一个报告，完整的数据存储到DB中，而且通过L3UI可以展示出来
> 所有模块的RESTART过程均启动计数器 zHcuGlobalCounter.restartCnt++
> CLOUDVELA中增加CONN/DISC的计数器 zHcuGlobalCounter.cloudVelaConnCnt++, zHcuGlobalCounter.cloudVelaDiscCnt++
> ETHERNET增加发送数据超时计数器，zHcuGlobalCounter.CloudDataTimeOutCnt++

== Update log: 2016 May 13  SW Version: XQ.HCU.SW.R01.083 //ZSC
> L3新增PM25SHARP
> DB update for PM25SHARP
> AV file storage directory(config in syspar) creat as soon as system startup 

== Update log: 2016 May 14 SW Version: XQ.HCU.SW.R01.084 //ZJL
> 为了测试目的，增加I2CBUS及SPIBUS L2FRAME模块，但暂时没有起到作用
> 将DURATION_OF_INIT_FB_WAIT_MAX change to 20000(us), hcu_usleep(dest_id*DURATION_OF_INIT_FB_WAIT_MAX), to avoid conflict systematically.
> hcu_usleep() is in us iso ms!!!
> Add GPIO function modules
> Add I2C function modules
> Add SPI function modules
> Add DBI TEMP DHT11/SHT20/RHT03 operation API and database
> Add DBI HUMID DHT11/SHT20/RHT03 operation API and database
> Combine PM25Sharp to PM25 files, and format PM25SHARP as data structure, within commsg.h
> Hwinv add scan of PM25Shart/TEMP/HUMID (DHT11/SHT20/RHT03) and delete old data.
> PM25SHARP的数据结构定标? 待解决.

== Update log: 2016 May 15 SW Version: XQ.HCU.SW.R01.085 //ZJL
> Add test mode for MQ135/GPIO, test result is always HIGH, somhow not so reasonable, to be study

== Update log: 2016 May 17 SW Version: XQ.HCU.SW.R01.086 //ZJL
> 初始化INIT_WAIT_FOR_FB的基础延时时间，335D和树莓派相差比较大，使用编译开关分别定义。
> 增加BH1750/GY30光强完整工作的模块，包括数据库，DBIAPI以及L3处理
> 增加BMP180/GY68气压完整工作的模块，包括数据库，DBIAPI以及L3处理
> 增加MQ135/有毒气体完整工作的模块，包括数据库，DBIAPI以及L3处理

== Update log: 2016 May 18 SW Version: XQ.HCU.SW.R01.087 //ZSC
> add curl FTP modle 
> add the module for HCU SW download

== Update log: 2016 May 18 SW Version: XQ.HCU.SW.R01.088 //ZJL
> 增加BMP180温度传感器完整工作的模块，包括数据库，DBIAPI以及L3处理
> 完善各个传感器工作的上下界
> 初始化底层传感器变量HCU_SENSOR_VALUE_NULL
> 设置连续读取次数，并做平均，以解决稳定性问题
> 根据气压数据，计算出海拔高度数据，并跟气压一起存入独立的数据库表单中去
> PM25SHARP的工作过程调整为定时方式，以便更为灵活/方便/统一的方式对其进行处理控制
> 手工合并ZJL/ZSC代码

== Update log: 2016 May 20 SW Version: XQ.HCU.SW.R01.089 //ZSC
> add the module for HCU SW inventory

== Update log: 2016 May 23 SW Version: XQ.HCU.SW.R01.090 //ZSC
> add the module for the captured video upload
> merge hcu.sh(the scripts of auto starp-up) into hcu project

== Update log: 2016 May 31 SW Version: XQ.HCU.SW.R01.091
> MQ135, GPIO#5 -> change to GPIO#6
> 去掉部分内部模块HEART_BEAT，不然SVRCON的消息队列满

== Update log: 2016 May.31 SW Version: XQ.HCU.SW.R01.092 //ZSC
> upload AV file (softlink)
> delete log file in case hard disk usage > 90%

== Update log: 2016 June.2 SW Version: XQ.HCU.SW.R01.093 //ZJL
> 增加MTH01-SPI传感器： DBICOM， SensorTemp/SensorHumid, SPI.*, 数据库文件
> 增加BMPD300 PM2.5传感器，内容同上
> Add  extern UINT16 hcu_CRC_16(unsigned char *data,int len);

== Update log: 2016 June.5 SW Version: XQ.HCU.SW.R01.094 //多人修改中，还未形成正式版本
> 完善BMP180气压高度温度的计算算法
> 修正MessageQue满时一直重启[TIMER]任务的错误
> 修正了一些多行\n\n等问题
> 修正了字符串结束符'/0'为'\0'，不然出现严重告警的问题。
> 修正了PM25SHARP中的SUM_2S没有初始化的问题
> 使用SYSCONFIG全局控制静态变量，控制底层传感器是否启动
> 优化VM消息发送时msgsnd error的提示信息
> 删除任务并重新创建/启动任务成功
> 本版本出现的新问题：
问题1：PM25SHARP长时间跑，会出现消息缓冲区满的情况，这应该是READ阻塞的原因，待解决
>>  MODBUS采用了SELECT机制，搞成非阻塞，MODBUS.c line 430
>> 	ret = hcu_sps485_serial_port_get(&gSerialPort, currentModbusBuf.curBuf, MAX_HCU_MSG_BODY_LENGTH);//获得的数据存在currentModbusBuf中
>>  PM25SHARP需要修改为一样的机制
问题2：长时间运行SLEEP会被打断
>>M25SHARP修改为非阻塞机制

== Update log: 2016 June.11 SW Version: XQ.HCU.SW.R01.095 //ZJL
> 增加有毒气体VOC ZP01，MQ3酒精传感器以及甲醛ZE08CH2O传感器
> 同步修改的还有数据库
> MQ3酒精传感器完成，完全复用范式
> ZE08CH2O甲醛传感器使用串口，串口的基本功能通过minicom工具调好后，基本上可以工作了。
> 在树莓派下，Pin#14/15管脚对应的TX/RX是ttyARM0，工参配置为0端口
> ZE08CH2O的帧协议解码完成
> PM25SHARP的帧协议解码重新撰写
> ZP01VOC有毒气体传感器，还未拿到，待测试。

== Update log: 2016 June.15 SW Version: XQ.HCU.SW.R01.096 //ZJL
> 重点解决串口的问题
> autorun改为l0autorun，以便对其其它目录。如果该目录下的hcu.sh也用到这个目录绝对路径，则需要同步修改
> 去掉hw-inv中对磁盘的定时打印，太多太烦，不方便调试
> SPS232相关的处理函数移动到SPSAPI.×中，然后直接调用它们
























== Coming Later...
> 如何将UT-TEST环境包装进咱们项
> MMC机制，让人工可以通过命令行进行控制，以便启动和配置模块的运行与否
> MSG_COM_STOP->任务停留在DISABLED状态
> 从网络时钟同步机制，并合理跟GPS时钟融合的问题
> 蓝牙工作模块
> Audio/Camera作为传感器，有两种工作方式原理：一是永恒工作，就像其它传感器一样，数据存在本地，上层需要，直接取结果；二是完全由上层L3控制启动工作与否。目前采用的是第一种方式，是否足够好，需要再讨论。
> 从功耗和效率的角度，也许视频采用按需工作比较好，而噪声采用永恒工作的模式比较好，再研究并确定

== 遗留问题
1. 消除WARNING，其中有一种情况是函数未在.h中进行申明
2. 自创账户没有足够的权限启动TASK，必须在ROOT权限下工作，但ROOT权限下并没有安装中文输入，导致必须两边切IDE环境，比较麻烦
3. HCU创建的文件，其权限不确定，HWINV还有bug
4. 网络在非连接转连接态，HCU并不能正常恢复到连接态，需要重启才能恢复。HWINV还有bug
> 遗留问题：偶尔会出现usleep()的无效问题，可能还是信号量的不合理设置，需要待调查
> 遗留问题：偶尔出现后台发送不成功，及时再ETHERNET的情况下，非常值得调查起原因
> 遗留问题：目前情况下，一旦一次发送失败，该数据将丢失，而不是保存下来下次继续发送，未来需要改进，以便改善数据的准确性

==TIPS & Best Practice===
> chmod -R 777 *   => 一旦在ROOT下操作后，可能造成普通账户编译不过，文件不能访问，该命令将放开所有相关文件的权限，要在ROOT下操作
> 调试切换账户: sudo -i / root password,  then "cd /usr/eclipse“， "./eclipse", we could execute debug/run without any restrict
> 本地API都以func_开头，全局API都以hcu_开头，状态机的消息处理函数都以fsm_开头
> 全局变量都以zHcuxxx开头，typedef的结构均采用_t的形式表征结构体
> 程序中尽量少使用数值，原则上都采用.h文件中定义常量的形式来进行
> 函数传递指针给子函数，比较高效安全，直接传递结构体整体，堆栈使用太多
> 深度结构的结构型数组，取得其地址，尽量不要使用数组内的元素的地址，而应该直接用数组本身，因为元素的顺序会变化和修改，导致程序后期的维护可能出错
> 为了测试效果，PM25定时器设置为5S，随即启动延迟设置为模3，均应该调整为1分钟
> CLOUD链路检测心跳定时确定为15s，还可以更短些。测试时先保持这个标准
> 环境安装，可以参考《设计要点》
> 注意，#项目目录下的include内容是否完整，不然会出现编译问题
> HcuDebugPrint/HcuErroPrint必须按照其他例子来进行，不然会出现打印信息混乱的状态
> 升级版本变为新工程，借用原先的项目时，使用替代，而不是合并，不然不少旧文件会重现
> 函数命名规则： func_task_xxx => 本地函数 fsm_task_xxx => 状态机函数 hcu_task_xxx => 全局函数
> 全局变量命名规则： zHcuTaskxxx => 全局表单，使用extern进行修饰，以便其它模块引用
> 如果发现有些传感器发送的速度过快或者过慢，可以在L3目录中查看，相关的传感器.h中有时长定义，可以适当调整
> 另外，对于各个传感器的启动性问题，都采用了随机延迟的方案，减少各个任务之间的碰撞概率。延迟碰撞的时长设置在L2COMDEF.h文件中

