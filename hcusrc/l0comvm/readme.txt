====================================================================================================
==  修改历史                                                                                        ==
==  从共流项目开始，历史将倒叙增加修改记录，以简化每次的更新                                                 ==
====================================================================================================
==> 上位机HUITPJASON支持
==> 上位机Netdata/Linux性能实时监测工具 => http://www.my-netdata.io
==> 上位机Layui模块化前端UI框架 => http://www.layui.com/
==> 上位机frp内网穿透工具 => https://github.com/fatedier/frp







//= ZJL, 2018 Feb.5 CURRENT_SW_DELIVERY 308=>BFDF项目
= 完善BFHS/BFDF中有关STARTUP消息触发后的处理过程
= 为SYSSWM中软件下载，设置一个下载的时间长度，从而可以根据这个值来决定是否需要启动远程下载
= 注意：经过这一次改动之后，因为数据库表单的变化，将导致BFSC的老程序和新程序不兼容，必须升级数据库才能继续使用
= 基本完成了BFDF的业务和统计功能，包括数据库内容

//= ZJL, 2018 Jan.31 CURRENT_SW_DELIVERY 307=>BFDF项目
= Update BFDF algorithms

//= ZJL, 2018 Jan.22 CURRENT_SW_DELIVERY 306=>BFDF项目
= 测试过程中出现的问题：
  - 系统CFG_START_REQ启动时未检查所有板子的死活，暂时禁止该检查，导致bitmap不正确
  - 返回CFG_RESP消息中，统计参数全为0，检查过程忽略但报错误
  - WS_NEW_EVENT报告错误，消息长度错误
  - 将心跳缺省周期从10s改为1个小时，减缓该定时器疯狂上报对后台的压力
  - 将AQYC的软件下载周期改为10分钟一次
  - 将AQYC的PM报告周期改为1个小时一次
  
//= ZSC, 2018 Jan.12 CURRENT_SW_DELIVERY 305=>AQYC项目
=颗粒物浓度门限值超标，视频功能增强

//= ZJL, 2018 Jan.9 CURRENT_SW_DELIVERY 304=>BFDF项目
= 开始调测BFDF的界面
= Json-c big bug: can not use latest cloned verion13 (/etc/local/lib/libjson.so.3.0.1), but must use
  code verion 12 (libjson.so.3.0.0). This code also copied at Baidu yunpan.
= 完善HooperId的编址

//= ZJL, 2018 Jan.8 CURRENT_SW_DELIVERY 303=>BFHS项目
= To easy test, SYSSWM starting timer set from 15 to 15000. To be corrected.
= 绝对不在提早json_object_put(jsonHlc);//free释放json结构，不然会出现崩溃的情况
= 新增消息没有配置到VM_CONFIG.H中，导致无法接入

//= ZJL, 2018 Jan.6 CURRENT_SW_DELIVERY 302=>BFHS项目
= 持续完善BFHS项目功能，产品联调
= TEST_CMD codec error in HUICOBUSCODEC.C, not yet finish full function. snrBitmap just fill int=1, iso
  of defined array, so decode also use INT method. In future this has to be improved.

//= ZJL, 2018 Jan.4 CURRENT_SW_DELIVERY 301=>BFHS项目
= 修改MQTT的BUG
= 分离不同的发送模式

//= ZJL, 2017 Dec.28 CURRENT_SW_DELIVERY 300=>BFHS项目
= 完善BFHS的统计数据CallCell

//= ZJL, 2017 Dec.19 CURRENT_SW_DELIVERY 299=>BFHS项目
= 简化CLOUDVELA等函数的编写过程，向Object-C靠近
= 简化BFDF/BFHS相应的接收与发送部分

//= ZJL, 2017 Dec.11 CURRENT_SW_DELIVERY 298=>BFHS项目
= 完善BFHS的HUIREST调用函数

//= ZSC, 2017 Dec.9 CURRENT_SW_DELIVERY 297=>AQYC项目
= 增加支持低成本PMS颗粒物传感器，测试通过

//= ZJL, 2017 Dec.6 CURRENT_SW_DELIVERY 296=>BFHS项目
= 增加HUIREST架构

//= ZSC, 2017 Dec.5 CURRENT_SW_DELIVERY 295=>AQYC项目
= 增加支持颗粒物传感器马达PWM控制测试（GPIO PIN15），暂时只高低电平
= 增加支持控制温控开关测试（进一步控制继电器），湿度达到门限值，设定温度值

//= ZJL, 2017 Dec.4 CURRENT_SW_DELIVERY 294=>BFHS项目
= 增加BFHS中动态学习和校准有关的消息

//= ZJL, 2017 Nov.30 CURRENT_SW_DELIVERY 293=>BFHS项目
= 增加HUITP-JSON编解码
= 升级HUITP v3.4

//= ZJL, 2017 Nov.28 CURRENT_SW_DELIVERY 292=>BFHS项目
= HUICOBUS编解码
= 完善BFDF项目的L3算法

//= ZJL, 2017 Nov.27 CURRENT_SW_DELIVERY 291=>BFHS项目
= 将CANITFLEO和CANALPHA分离
= 完善BFDF的编解码
= 改造CANALPHA完成

//= ZJL, 2017 Nov.25 CURRENT_SW_DELIVERY 290=>BFHS项目
= 整理出第一版HUICOBUS字典和消息格式

//= ZJL, 2017 Nov.24 CURRENT_SW_DELIVERY 289=>BFHS项目
= 完善MQTT/HUICOBUS接口部分的定义
= 完善BFHS部分的SUI消息定义，HUITP V3.2 
= 完善bfhsContext参数表
= 完善BFHS的CANITFLEO接收处理过程
= 增加公共SUI消息的处理过程

//= ZJL, 2017 Nov.16 CURRENT_SW_DELIVERY 288=>BFSC项目
= 修正L3BFSCUICOMM   TEST_PARA，从U8改为U32

//= ZSC, 2017 Nov.04 CURRENT_SW_DELIVERY 287=>AQYC项目
= 增加支持中航显示屏（待屏确认需进一步调整优化）

//= ZJL, 2017 Oct.21 CURRENT_SW_DELIVERY 286=>FDWQ项目
= 更新HUITP V3.1复旦消息更新
= 增加士兵profile信息

//= ZJL, 2017 Oct.20 CURRENT_SW_DELIVERY 285=>BFDF项目
= 将下位机指示软件版本显示，从３个轮寻，改为上下位机必须一致
= 修正bug数据库ihusw/hcusw字符串的长度

//= ZJL, 2017 Oct.19 CURRENT_SW_DELIVERY 284=>BFDF项目
= 将CANITFLEO的消息处理完善

//= ZJL, 2017 Oct.19 CURRENT_SW_DELIVERY 283=>BFHS项目
= 完成BFHS主任务的状态机工作流程
= CANITFLEO/BFHSUICOMM/CANITFLEO之间的消息流程转起来了

//= ZJL, 2017 Oct.18 CURRENT_SW_DELIVERY 282=>BFSC项目
= 更新软件下载的版本显示
= 将HCU/IHU软件下载的信息更新到按钮区域

//= ZJL, 2017 Oct.17 CURRENT_SW_DELIVERY 281=>BFHS项目
= 增加BFDF/BFHS的CLOUDVELA编解码函数
= BFHS中的基础处理函数

//= ZJL, 2017 Oct.16 CURRENT_SW_DELIVERY 280=>BFHS项目
= 增加HUITP定义中的BFDF和BFHS部分
= 升级脚步更新为ZIP下载

//= ZJL, 2017 Oct.16 CURRENT_SW_DELIVERY 279=>BFSC项目
= 修正软件下载的bug
= 更新数据库表单的下载处理脚本

//= ZSC, 2017 Oct.16 CURRENT_SW_DELIVERY 278=>AQYC项目
= 增加风向校准（hcudb 更新到199版本：工参配置中增加风险校准值字段）
= 增加上报传感器异常值的修正

//= ZSC, 2017 Oct.15 CURRENT_SW_DELIVERY 277=>AQYC项目
= 增加支持朗亿粉尘仪

//= ZJL, 2017 Oct.13 CURRENT_SW_DELIVERY 276=>BFDF项目
= 完善MQTT的定义

//= ZJL, 2017 Oct.11 CURRENT_SW_DELIVERY 275=>BFSC项目
= 更新hcu_advance.sh脚本

//= ZJL, 2017 Oct.9 CURRENT_SW_DELIVERY 274=>BFDF项目
= 为复旦医疗部分增加HUITP格式定义

//= ZSC, 2017 Oct.1 CURRENT_SW_DELIVERY 273=>AQYC项目
= 告警机制改进
= 记录系统异常重启log（/var/hcu/reboot.log）
= 噪声/扬尘门限值告警照片FTP上传后台服务器

//= ZJL, 2017 Sep.27 CURRENT_SW_DELIVERY 272=>BFDF项目
= 界面显示软件版本
= [bug fix]升级完成后，不能删掉hcu文件
= upgrade.sh更新：去掉crontab
= BFSC的软件版本下载改为无限次
= 改进VM，暂时不用hcuvm机制，恢复vmlayer程序

//= ZJL, 2017 Sep.25 CURRENT_SW_DELIVERY 271=>BFDF项目
= 完成基础分选组合算法的测试

//= ZJL, 2017 Sep.22 CURRENT_SW_DELIVERY 270=>BFDF项目
= 增加每一组的目标范围
= 搭建分组与组合的核心过程

//= ZJL, 2017 Sep.20 CURRENT_SW_DELIVERY 269=>BFDF项目
= 写启动部分的代码

//= ZJL, 2017 Sep.20 CURRENT_SW_DELIVERY 268=>BFDF项目
= 创建消息处理框架函数
= 对BFSC项目有微调，需要REGRESSION该项目的基本工作内容

//= ZJL, 2017 Sep.19 CURRENT_SW_DELIVERY 267=>BFDF项目
= 继续搭建BFDF程序框架，创建基础内部消息

//= ZJL, 2017 Sep.18 CURRENT_SW_DELIVERY 266=>BFDF项目
= 开始编写料斗的双链数据结构操作函数
= 简化BFDF程序遗传，制作程序框架

//= ZJL, 2017 Sep.16 CURRENT_SW_DELIVERY 265=>BFSC项目
= 修改HUITP的picture定义

//= ZJL, 2017 Sep.15 CURRENT_SW_DELIVERY 264=>BFSC项目
= SWM_FTP去掉相应的工参配置
= hcuengpar表单改变了
= [RESTRICITON1] SYSSWM清理目录
= 给SYSSWM增加了一个新的控制机制：超越最大限度就不执行下载了
= 给DB下载增加保护：一旦软件下载成功，但数据库下载不成功，则将软件的版本恢复回滚到之前的情况

//= ZJL, 2017 Sep.14 CURRENT_SW_DELIVERY 263=>AQYC项目
= 完善HUITP中有关TEST_COMMAND和HSMMP_DATA_RESP消息的定义
= 完善MAC地址的读取
= 在传递Inventory_Report时，将MAC_ADDRESS存入了FuncFlag
= [RESTRICITON2] 判定目录是否存在，放开了

//= ZJL, 2017 Sep.13 CURRENT_SW_DELIVERY 262=>AQYC项目
＝　去掉了VMLAYER源代码，增加了新的库libhcuvm

//= ZJL, 2017 Sep.12 CURRENT_SW_DELIVERY 261=>AQYC项目
= 将所有产品CODE改为了十进制

//= ZJL, 2017 Sep.12 CURRENT_SW_DELIVERY 260=>AQYC项目
= 更新AQYC对应的HUITP规范，支持TEST_COMMAND过程，并放在PM模块中了

//= ZJL, 2017 Sep.8 CURRENT_SW_DELIVERY 259=>BFDF项目
= 建立起初始数据表单结构

//= ZJL, 2017 Sep.7 CURRENT_SW_DELIVERY 258=>BFHS项目
= 增加BFHS高速分选称项目
= 完善BFDF的测试部分，整个HATE测试环境基本上差不多了

//= ZJL, 2017 Sep.7 CURRENT_SW_DELIVERY 257=>BFDF项目
= MQTT的初始化改为延时无限循环
= htop用来查阅线程树形结构
= top -H -p12345用来查单个线程的运行状态
= VM中增加了线程名字的信息
= 将HATE机制增加到所有项目中去

//= ZSC, 2017 Sep.6 CURRENT_SW_DELIVERY 256=>AQYC项目
= Add Aggregation report instead single sensor report to default server(socket instead of Curl)

//= ZJL, 2017 Sep.6 CURRENT_SW_DELIVERY 255=>BFDF项目
= 创建测试引擎
= 创建测试表单库
= 创建基本的HATE工作逻辑

//= ZJL, 2017 Sep.5 CURRENT_SW_DELIVERY 254=>BFDF项目
= 增加HATE_TRIGGER_ENENABLE标识，用于将项目转变为测试目的

//= ZJL, 2017 Sep.5 CURRENT_SW_DELIVERY 253=>BFDF项目
= 创建BFDF双线分选组合称流水线项目

//= ZJL, 2017 Sep.1 CURRENT_SW_DELIVERY 252=>BFSC项目
= 先为MQTT的编解码做了一个基础的处理方式

//= ZJL, 2017 Sep.1 CURRENT_SW_DELIVERY 251=>BFSC项目
= 去掉hcuswdb数据表表单
= 增加了一个独立的项目HCWD，专门用于数采仪的硬件看门狗监控
= 为MQTT增加一个编解码的MQTT_JSON_MESSAGE

//= ZJL, 2017 Sep.1 CURRENT_SW_DELIVERY 250=>BFSC项目
=ADVANTECH版本下USBCAN出现问题后，强行退出程序
=清理UICOMM中打印返回的问题
=打印设置

//= ZJL, 2017 Aug.30 CURRENT_SW_DELIVERY 249=>BFSC项目
= 增加MQTT处理过程

//= ZJL, 2017 Aug.29 CURRENT_SW_DELIVERY 248=>BFSC项目
=继续更新HUITP v2.11，增加产品标签的名字，用来支持双线分选组合称
=增加打印机制
= 修改数据库中的全局工参表单，增加SerialPortForQrPrinter参数
=增加hcu_sps232_qr_printer_init / hcu_sps232_send_char_to_ext_printer，分别在L3BFSC初始化和组合出料完成以后使用
=连上RS232串口打印机，就可以打印了，需要工厂实际测试效果

//= ZJL, 2017 Aug.28 CURRENT_SW_DELIVERY 247=>BFSC项目
=改善YCJK的数据汇报格式部分，HUITP升级到2.11
=增加PICTURE实时汇报过程(1小时为单位)
=具体JCJK内容填写，待完善
=JCJK的定时器，STA_DATA还未启动，待完善
= SYSCONFIG.H中产品命名，修改后跟HUITP.H中保持一致

//= ZJL, 2017 Aug.25 CURRENT_SW_DELIVERY 246=>BFSC项目
= 增加HUITP中EQULABLE的消息对
= 改进ALARMINFO中对于DESC的描述
=将BFSC的下载过程关闭，将下载时间改为900s=15分钟，算是正式交付

//= ZJL, 2017 Aug.23 CURRENT_SW_DELIVERY 245=>BFSC项目
=调测IHU软件下载
=遇到问题：　l0dbi/dbi_sysswm.c: 543 : DBISYSSWM: MySQL connection failed, Err Code = !
= 完善了FTP下载过程
= 对于数据表单的多次连续操作，彻底解决了，可参考文档：　http://www.jb51.net/article/27085.htm

//= ZJL, 2017 Aug.22 CURRENT_SW_DELIVERY 244=>BFSC项目
=调查BFSC现场的算法DUMP问题
=下载期望消息长点，内部消息改为了1000。但此时，即便将内部消息缓冲区改为了163840，也抗不住太多的下位机消息过来。故而修改消息长度为800。
= 恢复1000的消息长度，因为L3BFSC死机造成系统没反应，而不是内存不够的原因
=将秤盘数量＝13，先简化问题，确定死机的原因 =>找到原因后，再次改回到17了
=找到了问题：算法搜索中，控制变量i被重新赋值了，导致错误！！！

//= ZJL, 2017 Aug.19 CURRENT_SW_DELIVERY 243=>BFSC项目
= 增加并完善FTP工作机制
=FTP下载之前，是否需要将本地文件删去？待完善
=遇到极其妖艳的问题：同一个内存变量，在不同的线程中，读取到的数值不一样，完全不能理解。。。：HUITP　PACK编译的结果


//= ZJL, 2017 Aug.18 CURRENT_SW_DELIVERY 242=>BFSC项目
=改进下载过程中的错误
=swDL数据表单直接换成先删去，然后再REPLACE
=cfg文件存储还有问题
=改进了BFSC的数据表单REPLACE过程，并修正了其中的BUG

//= ZJL, 2017 Aug.16 CURRENT_SW_DELIVERY 241=>BFSC项目
=继续更新软件下载中的错误
=改造工参数据的读取部分，数据库工参读取的优先级放在最后，缺省放在前面
=去掉量产模式这一机制，配置文件以及工参配置均做成缺省配置，然后再被配置文件所覆盖。
=HUITP在HCU模式下，数据长度改为300，内部消息长度改为了1000，从而尽力提高整个系统的效率
=注意SYSCONFIG.H中有关设备型号的定义，要和HUITP保持一致，不然会出现完全的错误

//= ZSC, 2017 Aug.1５ CURRENT_SW_DELIVERY 240=>AQYC项目
= Curl发送失败或数据库访问失败测数超过门限值，程序退出（Workaround solution）

//= ZJL, 2017 Aug.15 CURRENT_SW_DELIVERY 239=>BFSC项目
= 更新HUITP中有关INVENTORY_xxx的DESC信息
= 更新打印问题
= 增加多套参数配置的configId
= 去掉HCU_SYSCFG_HW_TYPE，启动HCU_SYSCFG_HW_PRODUCT_CAT_TYPE
= 数据库只能使用REPLACE模式
= 软件下载的改进

//= ZJL, 2017 Aug.10 CURRENT_SW_DELIVERY 238=>BFSC项目
= 更新HUITP中有关倾倒传感器的ＩＥ定义

//= ZJL, 2017 Aug.5 CURRENT_SW_DELIVERY 237=>BFSC项目
= 完善BFSC的算法FEATURE：Proximity算法及优先级算法

//= ZJL, 2017 Aug.3 CURRENT_SW_DELIVERY 236=>BFSC项目
= 完善dbVer的内容

//= ZJL, 2017 Aug.2 CURRENT_SW_DELIVERY 235=>BFSC项目
= 全屏
= 搞研华工控机步骤

add one more in B to secure the result ...

A: hcu auto start
1. copy hcu.sh to /etc/init.d/
2. #cd /etc/init.d
3, #chmod 755 hcu.sh
2. #update-rc.d hcu.sh defaults
3. #mkdir /var/hcu
4. copy working PrjBfsc to /var/hcu
5. cd /var/hcu/
6. #mv PrjBfsc hcu
7. make sure /var/hcu/cfg/hcuboot.cfg exist

restart computer, use following command to check whether hcu has been started automatically:
#top
there will be hcu process running
or
#ps -ef | grep hcu
B.  After login (I will use WeChat to send picture to you)
0. in user run "rm .config/chromium -R" delete all content of ... (new added)
1. launch dish in GUI
2. search for "start"
3. run "startup applications"
4. add Chromium, with following command
/usr/bin/chromium-browser --password-store=basic --app=http://localhost/bfscui --start-fullscreen
restart computer to check whether GUI has been pop up automatically



//= ZJL, 2017 Aug.1 CURRENT_SW_DELIVERY 234=>BFSC项目
= 将IHU FTP下载机制搞完善
= 继续保持文件机制，有一定的好处：测试条件下，可以满足人工放入的诉求
= 去掉ZHB / STDXML格式的协议支持，包括commsg.h中对于相应部分的定义，全部去掉了
= ZHBTJH212中的心跳信号，恢复了，虽然没啥用处

//= ZJL, 2017 July.31 CURRENT_SW_DELIVERY 233=>BFSC项目
= 下载分段中，INVENTORY完成
= 改动量较大，先checkin
= hcuboot.cfg/hcu.sql均做了修改

//= ZJL,, 2017 July.30 CURRENT_SW_DELIVERY 232=>BFSC项目
= 下载分段机制继续
= 建立起基本的DBI数据处理机制
= 更新数据库表单

//= ZJL, 2017 July.30 CURRENT_SW_DELIVERY 231=>BFSC项目
= 下载分段机制
= 删掉三个月的老旧告警数据，放入HWINV工作机制中
= 去掉dbi_bfsc对于huitp.h的依赖
= 清理vmlayer.h/l1comdef.h的包含关系，简化各种包含关系
= 发现ADCZERO/ADCFULL的bug在合并的是否，丢掉了，重新找回来了

//= ZSC, 2017 July.29 CURRENT_SW_DELIVERY 230=>AQYC项目
= 增加对阿尔森扬尘传感器和易谷分体风向风速传感器的支持
= 修改hcudb,调整各传感器定时器值

//= ZJL, 2017 July.28 CURRENT_SW_DELIVERY 229=>BFSC项目
= 增加软件管理的数据表单

//= ZJL, 2017 July.27 CURRENT_SW_DELIVERY 228=>BFSC项目
= Change BFSC/IAP project BIG Endian to SMALL Endian

//= ZJL, 2017 July.24 CURRENT_SW_DELIVERY 227=>BFSC项目
= Bug fix on IHU BIN file analysis
= 规整所有服务器端口

//= ZSC, 2017 July.23 CURRENT_SW_DELIVERY 226=>AQYC项目
= HCUDB 更新hcutracemsgctr表单，增加几个BSFC新定义的消息

//= ZJL, 2017 July.22 CURRENT_SW_DELIVERY 225=>BFSC项目
= 将.HEX文件下载格式改为.BIN文件格式

//= ZSC, 2017 July.19 CURRENT_SW_DELIVERY 224=>AQYC项目
= bug fix for report data error(little endian for AQYC)

//= ZJL/LZH, 2017 July.19 CURRENT_SW_DELIVERY 223=>BFSC项目
= Bug fix on data base of hcubfscconfigpara

//= ZSC, 2017 July.15 CURRENT_SW_DELIVERY 222=>AQYC项目
= SW update to support low cost sensors(清易噪声/易谷风向风速/妙昕温湿度)
= Add picture&video capture once noise value exceed the threshold of alarm.

//= ZJL/LZH, 2017 July.15 CURRENT_SW_DELIVERY 221=>BFSC项目
= Bug fix
= 发现现象：组合的数据不够准确，最后发现是组合算法中的bug, fix完毕

//= ZJL/LZH, 2017 July.7 CURRENT_SW_DELIVERY 220=>BFSC项目
= Bug fix
= 新装系统中的Json，编译工作正常

//= ZJL, 2017 July.3 CURRENT_SW_DELIVERY 219=>BFSC项目
= 制作软件下载的中台假数据
= 完善IAP中有关软件下载的内容，同时对软件版体进行了归一化
= 为SW_PACKAGE增加了upgradeFlag，以便确保REL/VER/UPFLAG能唯一定位一个软件体
= 为CLOUDVELA更新软件版本下载过程中的字段缺失问题

//= ZJL, 2017 July.3 CURRENT_SW_DELIVERY 218=>BFSC项目
= Upgrade on HUITP SPEC V2.8

//= ZJL, 2017 July.2 CURRENT_SW_DELIVERY 217=>BFSC项目
= Remove Big Endian set for BFSC, to aligned with all other projects
= Heart-Beat bug fix

//= ZJL, 2017 June.30 CURRENT_SW_DELIVERY 216=>BFSC项目
= 创建程序升级过程的框架
= 创建boot区更新RELID/VERID的函数
= 创建下位机程序升级服务的程序框架：支持SUI SW INVENTORY / SW PACKAGE过程

//= ZJL, 2017 June.30 CURRENT_SW_DELIVERY 215=>BFSC项目
= 改进VM功能：当某个任务MsgQue满的时候，删除该任务／消息队列，然后重新创建，从而达到重置的目的

//= ZJL, 2017 June.29 CURRENT_SW_DELIVERY 214=>BFSC项目
= Bug fix on SYSPM CRASH
= 数据库表单的访问中，发现can't create unix socket(24)，这是因为linux内核的open files数量中配置了1024，不够用了。将其改为10240或者51200/Unlimited，就够用了 
= MySql采用mysqladmin -uroot -p123456 variables | grep files，也能发现mysql对于文件打开的最大限制数为1024，但据说这个没起作用。
= MySql max_connetions也没改动，因为咱们只有少量的用户在使用
= 经过配置参数的放大以后，这个问题暂时按下不表，未来继续进行观察，看看是否还有其它风险

//= ZJL, 2017 June.28 CURRENT_SW_DELIVERY 213=>BFSC项目
= Bug Fix：组合对于ValidFlag的判定

//= LZH/ZJL, 2017 June.26 CURRENT_SW_DELIVERY 212=>BFSC项目
= 集成UICOMM的改进
= 增加Heart_beat过程，并跟下位机保持同步
= 为了支持组合之后再重新组合，将REPEAT消息翻译成NEW_WS，以触发重新组合
= 支持在组合出料差错条件下，让系统进入STOP状态
= Change WS configuration to 11, from 17
= Active GIVE UP process

//= ZJL, 2017 June.21 CURRENT_SW_DELIVERY 211=>BFSC项目
= 完善NEW_WS_EVENT中对于WS_EVENT的处理
= 完善本地数据库界面的显示工作
= 增加L3BFSC测试代码，查询崩溃的可能原因
= Continue to dig the issues of HCU crash, to add gcc link option: -Wl,--hash-style=sysv
= Change back the uplimt from 100g to 1000g, to match current factory test environment

//= ZJL, 2017 June.18 CURRENT_SW_DELIVERY 210=>BFSC项目
= 完善REPEAT WS的消息处理过程，防止HCU重启以后，陷入所有传感器只汇报REPEAT_WS状态，而无法进入组合状态
= 修正统计报表爆表的问题
= 修正RESET下位机重新配置参数不完整的问题

//= ZSC, 2017 June.17 CURRENT_SW_DELIVERY 209=>AQYC项目
= 一旦Msg Queue满退出HCU，以便Daemon重启HCU，保证业务不中断
= 修改几个潜在的段错误Bug

//= ZJL, 2017 June.12 CURRENT_SW_DELIVERY 208=>BFSC项目
= 改善ERROR_INQ的处理过程

//= ZJL, 2017 June.10 CURRENT_SW_DELIVERY 207=>BFSC项目
= 修复git合并的一些问题
＝修复L3BFSC初始化的一些问题
＝BFSCUICOMM喂测试数据
＝简化CANID的选择性问题
= func_syspm_get_memoccupy / func_syspm_get_cpu_temp中，文件的操作没有对失败做保护，导致段错误程序崩溃，修正过来了
= syspm_globaldatasave process error, as database element not matched with real external db definition

//= ZSC, 2017 June.4 CURRENT_SW_DELIVERY 206=>AQYC项目
= PM增加汇报CPU温度

//= ZJL, 2017 June.4 CURRENT_SW_DELIVERY 205=>BFSC项目
= 配置信息改为单发，而非群发，因为每个下位机的配置信息不一样
= 完善calibration表单
= Pop UICOMM中的测试数据

//= ZJL, 2017 June.1 CURRENT_SW_DELIVERY 204=>BFSC项目
= 完成基础CANITFLEO和L3BFSC的消息编解码及状态机处理机制
= 完善基础程序

//= ZJL, 2017 May.31, CURRENT_SW_DELIVERY 203=>BFSC项目
= 改造USBCAN模块的函数命名
= 封装USBCAN的发送和接收API
= 完成CANITFLEO的编码函数工作
= 清理老旧编解码函数
= 接收消息处理的框架搭建

//= ZJL, 2017 May.29, CURRENT_SW_DELIVERY 202=>BFSC项目
= HUITP.H被MYC更新
= 清理sysengpar中有关BFSC的数据结构，这部分结构不再使用，包括数据库表单也不再使用，改用了ＵＩ界面配置的JASON文件进行配置
= 增加了hcuBfscFb2Ui数据库表单，用于UICOMM向UI界面通知执行的结果
= 建立启动过程的差错处理框架

//= ZJL, 2017 May.27, CURRENT_SW_DELIVERY 201=>BFSC项目
= 编写UICOMM的状态机框架
= 继续L3BFSC启动部分的程序框架

//= ZSC, 2017 May.25, CURRENT_SW_DELIVERY 200=>AQYC项目
= bug fix by shanchun for segment fault when plug in the 3G dougle, root cause to be check

//= ZSC, 2017 May.20, CURRENT_SW_DELIVERY 199=>AQYC项目
= 颗粒物查询TSP，PM2.5，PM10的Bug修复

//= ZSC, 2017 May.18, CURRENT_SW_DELIVERY 198=>AQYC项目
= bug fix for PM25 data unpack

//= ZJL/MYC, 2017 May.25, CURRENT_SW_DELIVERY 197=>BFSC项目
= 整合BFSC之前改动的所有内容，特别是ＣＡＮ接口的驱动程序代码
= 创建AdvanceTechBuntu目标机的编译环境
= 清理HUITP FRAME的协议接口规范
= 准备调整整个系统的状态机机制

= HUITP调整到v2.5



//= ZSC, 2017 May.16, CURRENT_SW_DELIVERY 196=>AQYC项目
= bug fix for noise data format

//= ZSC, 2017 May.6, CURRENT_SW_DELIVERY 195=>AQYC项目
= Update for double server

//= ZSC, 2017 Mar.16, CURRENT_SW_DELIVERY 194=>AQYC项目
= Update for HUITP

//= ZSC, 2017 Mar.16, CURRENT_SW_DELIVERY 193=>AQYC项目
= 依据《HJ_T212-2005国标协议完成》完成层三大部分功能

//= ZSC, 2017 Mar.13, CURRENT_SW_DELIVERY 192=>AQYC项目
= 依据《建设工程颗粒物与噪声在线监测技术规范》 对上报数据设置标志位


//= ZSC, 2017 Mar.10, CURRENT_SW_DELIVERY 191=>AQYC项目
=加入L3 2061 procedure
=定义监测数据标识符判断表，增加判断上报数据有效性标识符

//= ZSC, 2017 Mar.8, CURRENT_SW_DELIVERY 190=>AQYC项目
=调整数据上报精度格式


//= ZSC, 2017 Mar.8, CURRENT_SW_DELIVERY 189=>AQYC项目
=修改串口non-blocking 读取的timer值
=噪声仪（232转485接USB）不可与其他485设备共同使用一个串口，程序调整为设置串口虚拟标识

//= ZSC, 2017 Mar.6, CURRENT_SW_DELIVERY 188=>AQYC项目
=修改noise,使所有传感器使用同一个串口

//= ZSC, 2017 Mar.4, CURRENT_SW_DELIVERY 187=>AQYC项目
=修改LLC 中 pack/unpack用到的时间转换函数的bug


//= ZSC, 2017 Mar.3, CURRENT_SW_DELIVERY 186=>AQYC项目
=增加L3 功能(设计任意duration所有传感器Min/Max/Avg聚合数据生成)
=修改若干LLC上行处理 bugs
=修改socket和联通云不能连接问题（心跳参数调整）

//= ZSC, 2017 Feb.28, CURRENT_SW_DELIVERY 185=>AQYC项目
=增加L3 功能(2051消息)
=修改LLC bugs（bug： LLC层终结的消息消息头需带QN）
=修改socket不稳定Bug（cloudvela的频繁的online<->offline切换问题）

//= ZJL, 2017 Feb.24, CURRENT_SW_DELIVERY 184=>BFSC项目
=改进HUITP的消息定义，从而明确状态机的工作过程


//= ZSC, 2017 Feb.24, CURRENT_SW_DELIVERY 183=>AQYC项目
=Update alarminfo table hcudb.sql for test

//= ZJL, 2017 Feb.23, CURRENT_SW_DELIVERY 182=>BFSC项目
=给所有项目增加新的LLCZHB模块，以便让项目编译变得统一和方便，当然采用虚拟LINK的方式，不是普通文件import的方式
=解决了以下BUG#001：原因是链路没建立成功，FAILURE导致状态机报错。改为返回OPRSUCC，就没有问题了。
==> 从BFSCUICOMM模块定时扫描，送回UI界面的结果给L3BFSC模块：机制是定时扫描数据库，还是有类似于ISR中断的钩子？
= 实现了最为无耻的界面控制方法
=数据库表单再更新
=需要更新本地L3UI界面

[BUG#001]
发现在ETHERNET模块启动的时候，启动完成后，又收到了MSG_ID_COM_INIT，是从SVRCON发过来的，但此时SVRCON已经进入休眠了，不太可能发送该消息，怪哉
如果ETHERNET模块不启动，则该问题消失。该问题出现在链路建立不成功，从而ETH模块给自己发送MSG_ID_COM_RESTART以后

=BFSC的工惨配置表单以及统计数据展示均设置好了
＝数据表单中，还欠缺读取设置标志位，这个需要做一个循环来读取，然后就可以开始控制。具体的，需要等待着如何设计界面，才能确定：到底是单独弄一个控制数据表单，还是复用SID=1的表单结构
=数据库表单更新，需要系统及时更新



//= ZJL, 2017 Feb.22, CURRENT_SW_DELIVERY 181=>AQYC项目
=LLC编解码完全完成，规范中所有的测试用力都测试通过，测试脚本数据在程序中。
＝程序环境全部恢复为XML格式数据，不影响任何执行。

//= ZJL, 2017 Feb.21, CURRENT_SW_DELIVERY 180=>AQYC项目
=完成初步的编码过程，问题不少
=千万小心pack()宏编译包含的内容，不得包含任何内部*h文件，不然可能引起灾难
=CLOUDVELA的协议改为ZHB
=CLOUDVELA启动的是否，就去启动HEART-BEAT过程
=CLOUDVELA过程中，将接收消息的过程，放在COM状态下，以便不受链路状态的控制
=LLCZHB的初始密码设置过程中，去掉密码检测，此时检测肯定不过的

//= ZJL, 2017 Feb.20, CURRENT_SW_DELIVERY 179=>AQYC项目
=完成消息编码中字典的全部构造
=测试完善消息砖头的正确性

//= ZJL, 2017 Feb.19, CURRENT_SW_DELIVERY 178=>AQYC项目
=完成了LLC层所有的链路逻辑控制功能
=修正密码，使用char s[7]的方式，而非整数
=将xxx-Value的数据考虑在内
=通过将信息单元／消息和函数命名为尾数编号，找到了潜在的错误，确保编码和俗名的一致性

//= ZJL, 2017 Feb.18, CURRENT_SW_DELIVERY 177=>AQYC项目
=增加LLCZHB模块，以及ZHBHJT212-L2CODEC两个模块，分别完成LLC链路层功能，以及MAC层的编解码功能。MAC依然挂在CLOUDVELA模块中。
=搭建LLCZHB/AQYCG20对于ZHB协议处理的命令控制框架
=搭好编解码调用关系的程序框架：CLOUDVELA不需要再动，只需要搞ZHBHJT212编解码即可
=完成LLCZHB内部模块的状态机。流程完成了需要在LLC中处理的流程，以及第一个完整的RTD业务流程。后面继续，全部完成后，就继续MAC层的编解码

//= ZJL, 2017 Feb.17, CURRENT_SW_DELIVERY 176=>BFSC项目
=清理端口的工参数数据定义，清理掉工参表中对协议的配置，只能从SYSCONFIG.H中进行读取，简化工参配置。数据库更新。
=给ZHBHJT编写消息字典
=增加YCJK的相应聚合消息，任务目标付给了TASK_ID_L3AQYCG20
=增加YCJK的HUITP消息编解码过程和内容体
=在TASK_ID_L3AQYCG20中，增加控制／数据中继的完善能力框架
=通过TASK_ID_L3AQYCG20可以对所有其它传感器进行控制。消息将采用L3MOD_EXG_CTRL_REQ/RESP/REPORT三条消息对所有通信进行控制

//= ZJL, 2017 Feb.17, CURRENT_SW_DELIVERY 175=>BFSC项目
= 增加随机HCUNAME的方式，并删去了系统中对于该参数的工参配置，改为固定的hcuboot.cfg设备配置
=为ZHBMN增加物理配置信息
=该版本需要支持：１.创建/var/hcu/cfg目录结构，并将hcuboot.cfg移过去　２/import更新数据库。数据库表单预计hcuboot.cfg都在hcusrc程序目录中

//= ZSC, 2017 Feb.16, CURRENT_SW_DELIVERY 174=>AQYC项目
=更改完成hcu_ethernet_curl_data_send，此API供事件驱动调用发送给Home server时调用，curl连接无心跳检测

//= ZJL, 2017 Feb.16, CURRENT_SW_DELIVERY 173=>BFSC项目
=增加BFSC的统计算法
=统一将文件目录改到/var/hcu/目录下了
=去掉swDbInfo，合并到hwBurnId区域中了，以简化设备标签

//= ZJL, 2017 Feb.15, CURRENT_SW_DELIVERY 172=>BFSC项目
=改进软件下载等功能
=移除func_cloudvela_time_out_period_for_sw_db_report，该工作机制内容移动到SYSSWM模块中去了
=去掉了CLOUDVELA_SW_REP定时器
=将FTP下载API函数移动到FTP服务中去了，不再放到链路中
=优化CANITFLEO打印
==> 发送给后台统计报告：通过定时器和时间流逝计数器搞定
==> 16个组合秤的状态，所有状态存入本地全局表，并存入数据库表单，以便跟界面进行交互。这个还需要跟UI界面的工作方式相关。工作机制确定，具体编码待完善。
===>下载任务，移出CLOUDVELA，放到L3xxxxTASK中去

//= ZJL, 2017 Feb.15, CURRENT_SW_DELIVERY 171=>BFSC项目
=完成L2 CLOUDVELA上行解码，现在全部完成了。代码量大，再提交。

//= ZJL, 2017 Feb.15, CURRENT_SW_DELIVERY 170=>BFSC项目
=完成L3MOD模块级别的函数归一化，从而在L2-L3之间搭建完整的API处理框架
=完成CLOUDVELA主函数中的编码。量足够大，分标签提交。

//= ZJL, 2017 Feb.14, CURRENT_SW_DELIVERY 169=>BFSC项目
=对传统的EMC/TEMP/WINDDIR/WINDSPD/TEMP/HUMID/HSMMP/NOISE进行改造
=统一采用DATA-REQ/DATA-RESP, CTRL-REQ/CTRL-RESP, DATA-REPORT/DATA-CONFIRM这种方式
=将所有传感器的L2消息全部制造出来了，下面就是填写处理函数内容
=收发，搭建完成所有的消息处理通道

//= ZJL, 2017 Feb.14, CURRENT_SW_DELIVERY 168=>BFSC项目
=将ALARM功能移到SYSPM功能模块中去，清理完毕。改动量较大，单独备份标签。
=ALARM合并

//= ZJL, 2017 Feb.14, CURRENT_SW_DELIVERY 167=>BFSC项目
=完成INVENTORY, SW-PACKAGE的L2消息处理过程
=完成HEART-BEAT的改造
=增加双链路的工作框架，在不影响目前功能的情况下，进行了略微的改进，一切是以保护FEFAULT为基础，HOME只能算是BEST EFFORTS
=使用上下文机制，简化链路ID的设计：LinkID由pack/unpack函数生产，由其它底层链路决定是否使用，以及如何使用

//= ZJL, 2017 Feb.13, CURRENT_SW_DELIVERY 166=>BFSC项目
=完成了ALARM/PERFORM的L2消息处理

//= ZJL, 2017 Feb.13, CURRENT_SW_DELIVERY 165=>BFSC项目
=完成BFSC项目部分的解码消息框架。L2处理的结果如何带到L3，待定。

//= ZJL, 2017 Feb.13, CURRENT_SW_DELIVERY 164=>BFSC项目
=清理JD/SAE等服务器地址，只保留最为必要的服务器配置项
=同步清理了数据库，先做标签备份
=完整的两套BH_STD_FORMAT，相互之间不冲突。ETH链路建立两套的具体过程，待完善。

//= ZJL, 2017 Feb.13, CURRENT_SW_DELIVERY 163=>BFSC项目
= 清理STDXML, STDZHB，以及HUITPXML的编解码格式函数的位置，统一化处理

//= ZJL, 2017 Feb.13, CURRENT_SW_DELIVERY 162=>BFSC项目
= 移除离线数据发送功能，将它放到上层L3处理。同步清除MEMDISK离线存储功能。
=使用#if-#endif技巧来处理CLOUDVELA填入消息时的参数检查，确保执行时的内存安全。这种方式虽然使用了#if编译技巧，相比程序执行过程中检查技巧，要很好很快的确定问题的根源
  特别是在面对大量的系统参数设置情形，可以在研发阶段就消灭潜在的低级错误
＝清理了数据库／硬盘／内存存储离线数据的工作机制，只保留数据库机制，缺省只存入本地数据库
＝清理SYSCONFIG.H中选择和标定三种存储模式的标识位，以后所有的数据缺省都存入本地数据库

//= ZJL, 2017 Feb.12, CURRENT_SW_DELIVERY 161=>BFSC项目
＝搭建好后台通信的消息处理框架，并采用#ifdef关联到L3BFSC项目，降低对其它项目的影响
= 清理CLOUDVELA中的差错出错函数，归一化到统一宏定义模式，完成。

//= ZSC, 2017 Feb.12, CURRENT_SW_DELIVERY 160=>AQYC项目
＝创建HKvision API
＝实现PM25 门限值告警触发拍照及录像

//= ZJL, 2017 Feb.11, CURRENT_SW_DELIVERY 159=>BFSC项目
＝完成HUITP和内部消息的编写，为打通后台工作逻辑框架打下基础

//= ZJL, 2017 Feb.11, CURRENT_SW_DELIVERY 158=>BFSC项目
= msgie_struct_gps_pos_info_t结构去掉了，合并到zHcuVmCtrTab.hwinv.gps中去了
=合并sps232的模块动态数据到hwinv中去
=继续完善BFSC的内部消息定义
=逻辑是，服务层的数据和服务，逐渐集中到VM层次，上层业务应用不沾惹。
= 对于底层硬件扫描的数据，比如传感器定时扫描出的内容，得需要考虑是否可以采用这种形式，而不是复杂的状态机控制模式
=合并PM25Sharp / Hcho / 到zHcuVmCtrTab.codab共享数据区了，简化此类简易传感器的数据传递。其实原来的机制本来就是共享变量，但目前的做法更加可靠／通用和统一。
=归一化传感器的配置，去掉SYSCONFIG.H对于传感器是否激活的配置
=增加HUITP消息长度检查的#ifdef判定
=L3BFSC: 防止HUITP和内部消息在关键结构上定义的不一致，导致程序死机
=完成了所有传感器激活控制的列表归并

//= ZJL, 2017 Feb.10, CURRENT_SW_DELIVERY 157=>BFSC项目
=完成BFSC HUITP的消息编写
=去掉MSG QUEUE机制
=清理HCU_SYSDIM, HCU_SYSCFG, HCU_SYSMSG全局常量命名机制，跟HUITP统一命名机制保持一致，改善程序可读性，降低出错的概率

//= ZJL, 2017 Feb.9, CURRENT_SW_DELIVERY 156=>BFSC项目
= 编译技术保证系统BFSC中配置的秤盘数量小于HUITP的传输能力
=尝试增加公共FSM状态，简化程序设计
=VM-COMMON机制完成，增加一个FSM_STATE_COMMON，在此状态下，任何消息都可以进入

//= ZJL, 2017 Feb.9, CURRENT_SW_DELIVERY 155=>BFSC项目
=清理完成所有TIMERID资料，包括各个项目的内容
=去掉HWINV/DIBCOM中对SYSCONFIG中定时器宏定义的读取
=增加量产标签，工程参数基础部分在研发阶段，允许从SYSCONFIG中读取，克制界面UI配置的数据。一旦该标签去激活，或者量产设置激活，则只能从数据库中读取
=去掉数据库读取失败依然从缺省参数读取的机制。在实际运行中遇不到，而且让程序的扩展性和维护性变差。
=统一的将外部UI中好的参数都POP到SYSCONFIG中，从而维护程序中参数到最新状态
=统一归一化所有的参数配置，组成类别，清理不合理配置，包括特定配置部分
=基本上清理完毕。视频服务器/软件下载/烧录区/设备标签，待完善。
=对于TimerName/ModName/MsgName的访问，必须访问工程参数表，不得访问静态表，因为静态表需要搜索，请注意！
= 改进TIME_OUT消息的TRACE，现在可以支持打印TIMER_ID的模式以及信息

//= ZJL, 2017 Feb.8, CURRENT_SW_DELIVERY 154=>BFSC项目
=解决了TIMERID初始化的问题，下一步就是将系统中所有的旧TIMERID全部清除掉
= 待解决SYSCONFIG.H中特定TIMER部分
=所有项目回归编译完成

//= ZJL, 2017 Feb.7, CURRENT_SW_DELIVERY 153=>BFSC项目
= 修正任务启动中一部分任务忘记从工参数据表获取的问题
= 增加了SYSSWM软件管理模块，准备用于软件下载等功能
=完善任务模块的启动初始化过程，以及TIMER的配置过程，提高智能化程度，降低研发复杂度和配置工作量
=去掉工参中对模块是否激活的控制
= 完成TASK启动中有关pnpFlag的设置
= 给所有项目增加SYSSWM模块
= 清理所有模块对于SYSCOFIG.H中对模块启动的配置
=三种TIMERID统一编址改进

//= ZJL, 2017 Feb.7, CURRENT_SW_DELIVERY 152=>BFSC项目
=回归所有项目的编译
=MessageTrace / Module Trace的ctrlFlag起作用了 

//= ZJL, 2017 Feb.7, CURRENT_SW_DELIVERY 151=>BFSC项目
=清理全局表，简化全局表单的控制/初始化等内容
=改造量太大，形成单独的版本
=验证过BFSC/G20项目

//= ZJL, 2017 Feb.6, CURRENT_SW_DELIVERY 150=>BFSC项目
=改进HUITP中对MsgType的定义，跟老旧xml/zhb完全区分开来
=改进zHcuGlobalTaskInputConfig的初始化内容，简化模块的TRACE改造设置过程
=改进zHcuMsgNameList的初始化内容，简化模块的TRACE改造设置过程
= 挖掘出MySQL中INSERT和UPDATE，以及REPLACE的关系。对于可能潜在有冲突的地方，需要尽可能使用SQL语句本身的能力去做，而非大量通过函数API再行处理
=因为智能初始化，去掉了TASK_ID_COM_BOTTOM
=因为智能初始化，去掉了MSG_ID_COM_BOTTOM
=Trace数据表单重新备份到dbi文件中去了。以后这两部分在量产之前，采用自动初始化的方式进行，降低增加消息和模块的复杂度及工作量。

//= ZJL, 2017 Feb.5, CURRENT_SW_DELIVERY 149=>BFSC项目
=修正SysParEng的定义，结构定义名字合理化
=继续完善HUITP协议：初步移植并集成好，待完善具体的消息体样例

//= ZJL, 2017 Feb.5, CURRENT_SW_DELIVERY 148=>BFSC项目
= 按照HCU NAME命名规则改进，将SysParEng中有关自身名字的长度从12改为20个
=改进strcpy为strncpy
=改进启动区的结构
=读取hcuboot.cfg区域，完善了处理过程
=该文件处于/var/www目录下，采用sysconfig.h进行配置，可按照项目进行修改的。
==> 设备物理地址的配置方式
 》》 初步想使用脚本的方式，而不使用数据库工参配置，就是为了安全，因为这一块就不需要被配置，而是工厂出来后就定下来了
》》 也就是说，这块配置信息应该成为基础平台的一部分，交给UBOOT来搞定，这里只负责读取即可。
》》目前暂时支持两种方式：如果不配置物理地址，数据库中的配置也接受。如果配置了物理地址，则物理地址将会覆盖数据库配置的地址

//= ZJL, 2017 Feb.4, CURRENT_SW_DELIVERY 147=>BFSC项目
=优化TASK NAME
=增加l2codec目录结构，将xml/zhb编解码函数移动到该目录，简化目录结构
=集成HUITPXML，完成垃圾集成，未来再进行详细的完善

//= ZJL, 2017 Feb.4, CURRENT_SW_DELIVERY 146 =>BFSC项目
=工程参数化设备物理标识区
=为每一个项目建立单独的设备标识，标识码来自于HUITP协议定义
=完善物理烧录区域的处理过程
=上位机启动时自身标识的改进，软件模块标识
=DBI操作API的归一化，并改善所有项目
=去掉HW_MODULE，只保留PEM信息，记录当前硬件的最新硬件状态，也就是当前硬件最新采用了哪个PEM版本。

//= ZJL, 2017 Feb.3, CURRENT_SW_DELIVERY 145 =>BFSC项目
=清理Commsg.h中的无效消息结构
=将Commsg.h中的消息进行必要的注释并归类
=改动量较大

//= ZJL, 2017 Feb.3, CURRENT_SW_DELIVERY 144 =>BFSC项目
=去掉zHcuTaskNameList[]数组，改用zHcuTaskInfo[i].taskName
= 改动量较大，且编译过所有项目任务，所以留下独立的标签

//= ZJL, 2017 Feb.3, CURRENT_SW_DELIVERY 143 =>BFSC项目
=进程启动模式的定义优化
=上位机func handler的优化 
=上位机VM初始化简化
=简化VM的初始化过程和配置过程，包括Heart-Beat及任务创建的简化过程
=增加Zeegbe模块，保持跟工程参数配置的一致性，特别是Module-Trace的控制功能
=模块SVRCON的改进
=SVRCON模块发送INIT出去的改进

//= ZJL, 2017 Feb.2, CURRENT_SW_DELIVERY 142 =>BFSC项目
=有关数据表单中hcusysengpar的模块是否激活，目前的未激活采用了0（NULL），而并非2（DEACTIVE） ，包括DBICOM和数据表单在内，全部改进完成。
=清理了VMDA之前来自于IHU中继承来的消息，涉及到COMMSG.H/VMLAYER.H，以及DBCOM数据库表单。
=统一引用FsmTask[]，为了方便进入task管理的FuncHandler，并统一增加Hcu前缀，就是为了减少全局重复的概率
= 引入统一任务初始化变量，并改善每一个任务模块的状态机初始化表名字
= 改造HCU的启动过程，全部移植到VM中来，降低HCU启动过程的复杂度，将完善的启动过程全部纳入到VM的管理过程中来。

//= ZJL, 2017 Feb.2, CURRENT_SW_DELIVERY 141 =>BFSC项目
=改进分项目情况下的#error编译选项，显著标定#else选项，确保多项目选择的安全
=上位机复杂的L3状态机进行受控保护，确保从错误中进行恢复 => 使用优化的高级方法！！！
= 改进ETHERNET模块中错误打印和信息打印
=归一化与后台的接口格式
=归一化模块是否激活，去掉SYSCONFIG中对于模块激活的0/1配置方式
=归一化通信媒介选择
=归一化工作进程方式
=去掉HTTP_TEST描述

//= ZJL, 2017 Feb.1, CURRENT_SW_DELIVERY 140 =>BFSC项目
= 改进LED的闪烁功能：外部程序模块可以通过调用LED提供的LED闪烁图案获得任何想要的拉灯效果: hcu_led_galowag_start/stop
=全局差错打印宏改进：HCU_ERROR_PRINT_TASK。后面再行大范围替换。

//= ZSC, 2017 JAN.18, CURRENT_SW_DELIVERY 139
= Regression测试完成，修改几个bug

//= ZSC, 2017 JAN.16, CURRENT_SW_DELIVERY 138
= 修改上下行全部用socket通信机制替代curl机制，需要和后台regression测试
\
//= ZSC, 2017 JAN.13, CURRENT_SW_DELIVERY 137
= 调整Alarm 表单，增加severity 和 alarm clear flag 
= 修改几个测试中发现的bug

//= ZSC, 2017 JAN.5, CURRENT_SW_DELIVERY 136
= HCU inventory中增加eth0 MAC address
= Performance 报告增加CPU/MEM/DISK 利用率

//= ZSC, 2017 JAN.1, CURRENT_SW_DELIVERY 135
= 改变HCU inventory report 机制（由命令行触发变为定期上报：因为在用的DB可能升级变化）
= HCU inventory中增加本地数据库版本信息报告

//= ZSC, 2016 DEC.29, CURRENT_SW_DELIVERY 134
= 编译告警清除
= DB中增加socketsrvadd 和 socketsvrhbtimer

//= ZSC, 2016 DEC.27, CURRENT_SW_DELIVERY 133
= 为hcu升级增加软件版本检查

//= ZSC, 2016 DEC.7, CURRENT_SW_DELIVERY 132 
= bug fix:Curl是否在链接状态都要处理 socket收到的server端来的消息，HCU要去除Curl全换成socket通信方式,待完成

//= ZSC, 2016 DEC.4, CURRENT_SW_DELIVERY 131 =>G20项目/BFSC
= add application layer heart beat check for socket
= 将#define HCU_CANITFLEO_TIMER_10MS_SIM_DATA 50000，未来应该保留为50。目前只是为了抑制大量CANITFLEO生成的WS_EVENT消息
= 编码器中对于DATA_REPORT部分的对其BUG
= 为了方便Ubuntu和树莓派对CANITFLEO双方的初始化，使用编译选项对初始化进行分类处理
= 数据库表单重新更新
= 将大量不需要启动的任务模块从BFSC中去掉了，特别I2C模块，如果直接启动，会造成树莓派执行出错，待查问题来源

//= ZSC/ZJL, 2016 DEC.3, CURRENT_SW_DELIVERY 130 =>BFSC项目
= 修改完善CloudVela中对于数据格式发送的BFSC部分
= CAN函数测试程序上线
= 继续完善CAN的操控

//= ZSC/ZJL, 2016 DEC.1, CURRENT_SW_DELIVERY 129 =>BFSC项目
= 周期性读取完成
= 从后台来的控制命令，启动和停止，命令执行链路完成。CANITFLEO中的具体命令下发以及得到反馈，待完善。
= CLOUDVELA相应的命令也完成了pack/unpack

//= ZSC/ZJL, 2016 NOV.30, CURRENT_SW_DELIVERY 128 =>BFSC项目/G2项目
= 完善Rasperry上的G20编译环境
= CANITFLEO的完整命令字：编解码函数已经完成
= 定时发送完整的传感器数据状态给后台

//= ZJL, 2016 NOV.17, CURRENT_SW_DELIVERY 127 =>BFSC项目
= 完整HUITP统一字段的定义，以便跟下位机及后台统一FRAME及消息结构定义
==> CLOUDVELA来的消息，所有消息统一定义？格式解码？发送到L3BFSC模块中来：初步给出通道
= HELLOWORLD程序中，加入了CANTEST的工具源代码，待研究

//= ZJL, 2016 NOV.17, CURRENT_SW_DELIVERY 126 =>BFSC项目
= 加快CATITFLEO的仿真速度
= 研究了不同参数的分布，进料的随机模式和顺序模式对成功率的影响，特别是秤盘数量对于成功率的影响
= 基本结论如下
》随机模式，成功率在70%附件，顺序模式在90%附件
》将目标上限分布调整到0.5%-2%等不同区间时，成功率可能会下降5-10%个百分点
》将秤盘数量调整到14个时成功率可达80%，减少到8个时，成功率降低到55%
》将进料最快调整到100ms，即10个物料/秒，系统稳定工作，这意味着整个状态机可以稳定可靠的工作

//= ZJL, 2016 NOV.17, CURRENT_SW_DELIVERY 125 =>BFSC项目
= 优化函数中HCU_PRINT_ERROR_L3BFSC和HCU_DEBUG_PRINT_NOR的可读性
= 完善了整个状态机工作机制，统计内容放入总控表

//= ZJL, 2016 NOV.16, CURRENT_SW_DELIVERY 124 =>BFSC项目
= 测试10MS时钟，将其打开，因为后续任务可能需要这个中等精度时钟
= 打开了所有的硬件模块，以充分测试树莓派的硬件接口能力。关闭无关的L3任务模块
= 增加L3BFSC的状态机，完善各种分支状态和初始化状态，形成完成的工作模式
= 完成组合算法
= 完善L3BFSC/CANITFLEO/BFSCUICOMM状态机
= 基本完成主要状态机

//= ZJL, 2016 NOV.16, CURRENT_SW_DELIVERY 123 =>BFSC项目
= 由于SYSDIM的复杂性，暂时恢复一次全局定义，不用各个项目分别定义，毕竟咱们主要对付的是高级A7以上的平台，内存还没到达如此严重的地步
= 简化TASK和MESSAGE ID的平铺直叙，统一编码，不然关键共享任务模块的编写过于复杂
= 扩大SYSDIM中所有系统级参数，以便容纳更多的任务和消息
= 修改SYSCONFIG中有关BFSC的启动模块，只留下必要的模块
= 内部HeartBeat优化
= 打印最终PNP状态优化

//= ZJL, 2016 NOV.15, CURRENT_SW_DELIVERY 122 =>BFSC项目
= 固定设置MSG_ID_COM_BOTTOM以及TRASK_ID_COM_BOTTOM朝上的TRACE选项，不用再通过数据表单以及开关项进行设置了
= 再启动一个L3BFSC项目的任务模块，专门负责读取数据库中的由L3UI所设置的比特位，以便及时跟界面进行交互
= CLOUDVELA消息定义和任务状态机
= BFSCUICOMM消息定义和任务状态机
= CANITFLEO的消息定义和任务状态机

//= ZJL, 2016 NOV.15, CURRENT_SW_DELIVERY 121 =>BFSC项目
=可视化SVRCON的INIT结果
= 增加各个项目独立的L3APPCTRL模块：因为是独立的工作模块，所以不需要数据库部分单独设立。
= TIMER由SYSDIM+L1TIMER单独搞定，不需要通过工程参数来过渡
= 所有项目的新L3APPCTRL任务框架搭建好了，

//= ZJL, 2016 NOV.15, CURRENT_SW_DELIVERY 120 =>BFSC项目
= 删去SPIBUS的选择性临时#define
= 增加CANITF公共模块，修改启动部分，任务模块部分，TIMER部分，以及数据库部分
= TRACE MODULE和MSG部分需要跟着一起修改
= 增加一种xxx_COM_BOTTOM机制：当任务模块/消息数量超过BOTTOM之后，数据库中不再可以跟踪修改，而只能通过程序单独赋予，变为常量定义
= 增加CANITFLEO模块的程序框架
= 工程参数sysengpar对应的控制界面，需要在L3UI中进行修改，这部分还未修改。

//= ZJL, 2016 NOV.14, CURRENT_SW_DELIVERY 119 =>BFSC项目
= 为每个项目提供独特的任务模块，以及相应的L3APPxxx层面的目录结构
= 通过VM TASK等方式的控制，形成项目分类处理的机制，希望不再影响数据库，以降低编程复杂度和界面的频繁调整，为后面增加新的任务模块提供模板
= 增加项目OpwillOtdr项目
= 将各个项目独特的控制文件调整到l3appctrl的统一目录下
= 删掉了老旧的平行“独特项目控制APP模块”对应的不同目录，简化整个文件目录结构，同时采用import/file system/virtual link的方式进行引用，并选取自己独立相关部分

//= ZJL, 2016 NOV.14, CURRENT_SW_DELIVERY 118 =>BFSC项目
= SYSCONFIG.H中将不同项目的参数控制分开控制，以便各个项目可以分别调整
= 为了降低参数配置的复杂性，增加程序可配的参数部分。如果没有必要，未来不再增加可通过数据库及界面配置的参数表
= TASK_ID / QUEUE_ID / MSG_ID，由于涉及到参数表和数据库的操作，没法针对单个项目进行优化，必须集合所有的项目，将所有项目全部放在一起
   这降低了单个项目系统的效率，但也简化了界面配置和程序编写的效率。
= 任务启动，完全靠SYSCONFIG进行配置，其任务号，消息号，均统一编码
= 删掉的AIRSYNC/MODBUS消息恢复，不然数据库TRACECTRL表单出错
= hcudb.sql所有项目共享，故而放到hcrsrc目录中，以便所有项目复用
= 数据库重新import，执行正常

//= ZJL, 2016 NOV.14, CURRENT_SW_DELIVERY 117 =>BFSC项目
= 为了项目中区分的完整性，增加了NBIOT_LPM_CJ， NBIOT_HPM_QG，虽然只是协议栈的不同，但目前项目是按照这个区分的，所以先保留
= 按照IHU的定义方式，将项目定义放在SYSDIM.H中，形成全局可配置的不同项目实体。一个技巧：ELSE不放，以便在编译检查时发现问题
= 去掉hcuref.c，这个参数程序退出了历史舞台
= 去掉AIRSYNC等类型的消息，同样退出了历史舞台

//= ZJL, 2016 NOV.14, CURRENT_SW_DELIVERY 116 =>BFSC项目
= 修改VMLAYER.x中的bug，memset(zHcuTaskInfo, 0, sizeof(zHcuTaskInfo));，不然导致段错误。这个错误如果是直接import原有项目，不会出错
   但一旦重新建立新项目就会出错。重新重新如此修正，应该不再出现问题了。
= 增加PrjTbswrG30, PrjGqybG40,  PrjCxIlc, PrjGlacm, PrjTestMode项目
= 除了挂墙仪表项目一定会使用树莓派之外，其它项目可能会使用335D/RasberryPi等不同硬件平台，所以军采用了4种Build-Configuration的G10项目模板。
= 挂墙仪表G40采用了AqycG20的项目模板，只有3种build-configuration

//= ZJL, 2016 NOV.13, CURRENT_SW_DELIVERY 115 =>BFSC项目
=增加和创建新的多项目共流框架
=创建BFSC项目程序框架
= 编译所有工作环境通过
= hcu.sh文件需要按照不同项目，进行分门别类的处理


=======================================================================================================
=============  文件起始位置  =============================================================================
=======================================================================================================

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
> SPS232的串口初始化放到每一次进行，这样可以让不同速度的串口同时存在，而不互斥。
> 由于外设的响应需要一分钟，导致每单次初始化串口之后必须等待1分钟才能再读取数据，整体读取速率将会变慢，但的确能正常工作。
> 在实际多个串口外设工作的时候，还是需要按照下列方式进行配置，不然数据读取反应速度会极其慢
	==> 为了设置串口传感器互斥，必须在SYSCONFIG中设置  #define HCU_SENSOR_PRESENT_SHARP HCU_SENSOR_PRESENT_NO

== Update log: 2016 June.16 SW Version: XQ.HCU.SW.R01.097 //ZJL
> 映射SPSAPI到SPS485，规范化SPS485模块功能
> MODBUS的串口初始化移到SPS485模块中去了
> 所有的其它任务，均直接调用SPSAPI，而不在是SPS485了。
> 由于这次改动较大，直接做一个版本，提交到DEV，防止更多的错误

== Update log: 2016 June.16 SW Version: XQ.HCU.SW.R01.098 //ZJL
> 增加LED任务模块，以便单独完成LED相关的控制
> 这个模块的增加，涉及到DBICOM等部分的修改，以及工程参数表单的修改，还有L3UI的修改，数据库的备份等。
> 注意，该过了数据库，需要重新IMPORT数据库以及L3UI到目标地

== Update log: 2016 June.16 SW Version: XQ.HCU.SW.R01.099 //ZJL
> 按照传感器说明书，完善ZP01VOC功能
> 实验LED以及马达功能
> CLOUDVELA过于庞大，将其中的XML和ZHB两种编解码函数集合全部拆出来，形成独立的文件模块，分别为BHCODECXML及BHCODECZHB
> LED自己编制的闪灯方案工作正常
> PWM控制的LED闪灯工作正常
> PWM控制的马达SG90偶尔工作一下，不能连续工作，待确定其正常工作的条件

== Update log: 2016 July.14 SW Version: XQ.HCU.SW.R01.100 //ZSC
> add the function: convert .h264 to mp4 so that the captured video can be played via http

//= ZJL, 2016 July.18, CURRENT_SW_DELIVERY 101
> 删掉垃圾信息
> 定义更多的项目配置信息，包括AQYC/TBSWR/智能仪表等，数据库中的更新并存入基础数据待定。 =》项目名字需要改变长度定义，目前显然不够
> 智能物联网的模块定义，搭建整体任务模块框架
> 先完善engpar数据表单
> 再完善ModuleCtrl模块
> 基本完成6个新任务模块的搭建

//= ZJL, 2016 July.21, CURRENT_SW_DELIVERY 102
> L3UI待完善，包括CRUD
> 先搞传感器数据库的存储和函数，完成
> HWINV对传感器IWM/IHM/IGM/IPM数据的定期清理
> 消息定义，CJ188/QG376/IWM/IHM/IGM/IPM模块状态机和处理函数框架搭建完成
> 消息结构定义完成

//= ZJL, 2016 July.22, CURRENT_SW_DELIVERY 103
> L3UI待完善，包括CRUD
> 清理全局常量的命名形式
> 建立SOCKET工作的假机制，以便继续CJ88 
> 建立CJ188链路
> L3和NBIOTCJ88链路之间的消息处理函数
> CJ188上行链路编码完成

//= ZJL, 2016 July.23, CURRENT_SW_DELIVERY 104
> DL解码工作完成

//= ZJL, 2016 July.24, CURRENT_SW_DELIVERY 105
> L3消息处理机制全部完成
> L3UI及CRUD完成

//= ZJL, 2016 July.24, CURRENT_SW_DELIVERY 106
> 临时集成了USB-DAC功能，放在L1HWOPT下了
> apt-get install libusb.*以后，并增加usb编译库后，才能通过.
> 目前已经将Unbuntu和Raspi环境配置好了，335D的环境需要再配置才好使。如果一时搞不好，可以使用编译选项将其隔绝，反正335D暂时也用不到。
> 修正了NBIOTCJ188取非的一个BUG

//= ZJL, 2016 July.25, CURRENT_SW_DELIVERY 107
> 修正了时钟的问题，之前一直跟SLEEP相互冲突，现在修改为线程模式后，该问题得到解决。总体上感觉，这种方式的静态性不够好，但足够使用的了。
> 目前时钟可以支持到最低ns级，考虑到系统的实时性，暂时将10ms和1ms的定时关掉了。如果需要，可以在TIMER.H中修改配置信息进行打开

//= ZSC, 2016 July.30, CURRENT_SW_DELIVERY 108
> bug fix for video play(E2E)。
> add socket client(blocking receive), to replace CMD polling 


//= ZSC, 2016 Aug.26, CURRENT_SW_DELIVERY 109
> add socket client heart beart checking 
> add socket client re-connect mechanism 


//= ZSC, 2016 Sep.20, CURRENT_SW_DELIVERY 110
> bug fix for PM25 Sensor address config 

//= ZSC, 2016 Oct.20, CURRENT_SW_DELIVERY 111
> update for support AWA noise sensor(ASCII code/decode) 
> bug fix in spibusaries module

//= ZSC, 2016 Oct.28, CURRENT_SW_DELIVERY 112
> update for support KUANKE windspd&dir(equid and register add modify)
> bug fix for series port read/write to fix segement failure error 

//= ZSC, 2016 Nov.4, CURRENT_SW_DELIVERY 113
> update for support USB GPS receive(ttyAMC0)
> add alarm report function(for Sensors) 

//= ZSC, 2016 Nov.6, CURRENT_SW_DELIVERY 114 =>AQYC项目
> add Pm report function(for Module restart & Cloud Discon & Socket Discon) 











== Coming Later...
> 如何将UT-TEST环境包装进咱们项
> MMC机制，让人工可以通过命令行进行控制，以便启动和配置模块的运行与否
> MSG_COM_STOP->任务停留在DISABLED状态
> 从网络时钟同步机制，并合理跟GPS时钟融合的问题
> 蓝牙工作模块
> Audio/Camera作为传感器，有两种工作方式原理：一是永恒工作，就像其它传感器一样，数据存在本地，上层需要，直接取结果；二是完全由上层L3控制启动工作与否。目前采用的是第一种方式，是否足够好，需要再讨论。
> 从功耗和效率的角度，也许视频采用按需工作比较好，而噪声采用永恒工作的模式比较好，再研究并确定

== 遗留问题
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
> ps -ef | grep hcu
> kill -9 12345

=======================================================================================================
=============  文件终止位置  =============================================================================
=======================================================================================================
