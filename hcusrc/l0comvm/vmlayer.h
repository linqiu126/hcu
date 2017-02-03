/*
 * vmlayer.h
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#ifndef L0COMVM_VMLAYER_H_
#define L0COMVM_VMLAYER_H_

#include "../l0comvm/commsg.h"
#include "../l0comvm/sysdim.h"
#include "../l0comvm/sysconfig.h"
#include "../l0comvm/sysengpar.h"
#include "../l1com/l1comdef.h"


//定义TASK对应的名字
//!!!!!!!!!!!!!!!!ATTENTION!!!!!!!!!!!!!!!!
//Whenever the Task Id is changed, QueID and vmlayer.c/zHcuTaskNameList array should be updated, otherwise error will happen!!!
//不是任务的任务，比如TRACE/CONFIG/3G/GPIO等等，留待清理，简化任务列表
/*
 *
 *   【增加任务】，必须同时修改四个地方：
 *   - HCU_TASK_NAME_ID
 *   - HCU_TASK_QUEUE_ID
 *   - zHcuTaskNameList
 *   - 还要DBICOM中去修改ModuleTrace的数据，不然通过L3UI界面进行控制TRACE也会出错，并备份数据库
 *   - 如果需要完美表现，还得最终需要升级L3UI的CRUD，不然相应的工具会出错
 *
 */
enum HCU_TASK_NAME_ID
{
	TASK_ID_MIN = 0,
	TASK_ID_HCUMAIN,
	TASK_ID_HCUVM,
	TASK_ID_TRACE,
	TASK_ID_CONFIG,
	TASK_ID_TIMER,
	TASK_ID_MMC, //Man Machine Communication
	TASK_ID_GPIO,
	TASK_ID_I2C,
	TASK_ID_SPI,
	TASK_ID_PWM,    //脉冲控制
	TASK_ID_ADC,    //模拟量通道
	TASK_ID_SWITCH, //开关量
	TASK_ID_RELAY,  //继电器
	TASK_ID_MOTOR,  //马达
	TASK_ID_ZEEGBE,
	TASK_ID_GPRS,
	TASK_ID_SPS232,
	TASK_ID_SPS485,
	TASK_ID_BLE,
	TASK_ID_ETHERNET,  //ETHERNET
	TASK_ID_WIFI,
	TASK_ID_USBNET,  //USB used for network connection
	TASK_ID_3G4G,
	TASK_ID_HARDDISK,
	TASK_ID_CAMERA,
	TASK_ID_MICROPHONE,
	TASK_ID_FLASH,
	TASK_ID_GPS,
	TASK_ID_LCD,
	TASK_ID_LED,
	TASK_ID_HWINV, //Hardware Inventory
	TASK_ID_SPSVIRGO,
	TASK_ID_CLOUDVELA,
	TASK_ID_MODBUS,
	TASK_ID_AVORION,
	TASK_ID_I2CBUSLIBRA,
	TASK_ID_SPIBUSARIES,
	TASK_ID_NBIOTCJ188,
	TASK_ID_NBIOTQG376,
	TASK_ID_HSMMP,  //High Speed MultiMedia processing
	TASK_ID_EMC,
	TASK_ID_HUMID,
	TASK_ID_PM25,
	TASK_ID_TEMP,
	TASK_ID_WINDDIR,
	TASK_ID_WINDSPD,
	TASK_ID_NOISE,
	TASK_ID_AIRPRS,  //气压
	TASK_ID_CO1,     //一氧化碳
	TASK_ID_LIGHTSTR,//光照
	TASK_ID_ALCOHOL, //酒精
	TASK_ID_HCHO,    //甲醛
	TASK_ID_TOXICGAS,//有毒气体
	TASK_ID_IWM,  //智能水表
	TASK_ID_IHM,  //智能热表
	TASK_ID_IGM,  //智能煤气表
	TASK_ID_IPM,  //智能电表
	TASK_ID_SVRCON,
	TASK_ID_SYSPM,  //性能统计
	TASK_ID_PM25SHARP,  //for pm25sharp sensor by shanchun
	TASK_ID_CANITFLEO,
	TASK_ID_COM_BOTTOM, //统一的任务定义结束
	TASK_ID_L3AQYCG10,
	TASK_ID_L3AQYCG20,
	TASK_ID_L3TBSWRG30,
	TASK_ID_L3GQYBG40,
	TASK_ID_L3CXILC,
	TASK_ID_L3CXGLACM,
	TASK_ID_L3NBLPM,
	TASK_ID_L3NBHPM,
	TASK_ID_L3BFSC,
	TASK_ID_BFSCUICOMM,
	TASK_ID_L3OPWLOTDR,
	TASK_ID_MAX,
	TASK_ID_INVALID = 0xFFFFFFFF,
}; //end of HCU_TASK_NAME_ID
//定义TASK对应的MESSAGE_QUEUE的ID
enum HCU_TASK_QUEUE_ID
{
	TASK_QUE_ID_MIN = HCU_TASK_QUEUE_ID_START,
	TASK_QUE_ID_HCUMAIN,
	TASK_QUE_ID_HCUVM,
	TASK_QUE_ID_TRACE,
	TASK_QUE_ID_CONFIG,
	TASK_QUE_ID_TIMER,
	TASK_QUE_ID_MMC, //Man Machine Communication
	TASK_QUE_ID_GPIO,
	TASK_QUE_ID_I2C,
	TASK_QUE_ID_SPI,
	TASK_QUE_ID_PWM,
	TASK_QUE_ID_ADC,
	TASK_QUE_ID_SWITCH, //开关量
	TASK_QUE_ID_RELAY,  //继电器
	TASK_QUE_ID_MOTOR,  //马达
	TASK_QUE_ID_ZEEGBE,
	TASK_QUE_ID_GRPS,
	TASK_QUE_ID_SPS232,
	TASK_QUE_ID_SPS485,
	TASK_QUE_ID_BLE,
	TASK_QUE_ID_ETHERNET,
	TASK_QUE_ID_WIFI,
	TASK_QUE_ID_USBNET,
	TASK_QUE_ID_3G4G,
	TASK_QUE_ID_HARDDISK,
	TASK_QUE_ID_CAMERA,
	TASK_QUE_ID_MICROPHONE,
	TASK_QUE_ID_FLASH,
	TASK_QUE_ID_GPS,
	TASK_QUE_ID_LCD,
	TASK_QUE_ID_LED,
	TASK_QUE_ID_HWINV,
	TASK_QUE_ID_SPSVIRGO,
	TASK_QUE_ID_CLOUDVELA,
	TASK_QUE_ID_MODBUS,
	TASK_QUE_ID_AVORION,
	TASK_QUE_ID_I2CBUSLIBRA,
	TASK_QUE_ID_SPIBUSARIES,
	TASK_QUE_ID_NBIOTCJ188,
	TASK_QUE_ID_NBIOTQG376,
	TASK_QUE_ID_HSMMP,
	TASK_QUE_ID_EMC,
	TASK_QUE_ID_HUMID,
	TASK_QUE_ID_PM25,
	TASK_QUE_ID_TEMP,
	TASK_QUE_ID_WINDDIR,
	TASK_QUE_ID_WINDSPD,
	TASK_QUE_ID_NOISE,
	TASK_QUE_ID_AIRPRS,  //气压
	TASK_QUE_ID_CO1,     //一氧化碳
	TASK_QUE_ID_LIGHTSTR,//光照
	TASK_QUE_ID_ALCOHOL, //酒精
	TASK_QUE_ID_HCHO,    //甲醛
	TASK_QUE_ID_TOXICGAS,//有毒气体
	TASK_QUE_ID_IWM,  //智能水表
	TASK_QUE_ID_IHM,  //智能热表
	TASK_QUE_ID_IGM,  //智能煤气表
	TASK_QUE_ID_IPM,  //智能电表
	TASK_QUE_ID_SVRCON,
	TASK_QUE_ID_SYSPM,
	TASK_QUE_ID_PM25SHARP, //for pm25sharp sensor by shanchun
	TASK_QUE_ID_CANITFLEO,
	TASK_QUE_ID_COM_BOTTOM,
	TASK_QUE_ID_L3AQYCG10,
	TASK_QUE_ID_L3AQYCG20,
	TASK_QUE_ID_L3TBSWRG40,
	TASK_QUE_ID_L3GQYBG40,
	TASK_QUE_ID_L3CXILC,
	TASK_QUE_ID_L3CXGLACM,
	TASK_QUE_ID_L3NBLPM,
	TASK_QUE_ID_L3NBHPM,
	TASK_QUE_ID_L3BFSC,
	TASK_QUE_ID_BFSCUICOMM,
	TASK_QUE_ID_L3OPWLOTDR,
	TASK_QUE_ID_MAX,
	TASK_QUE_ID_NULL = 0xFFFFFFFF,
}; //end of HCU_TASK_QUEUE_ID


/*
** Fsm INFORMATION structure.
*/
#define FSM_STATE_ENTRY  0x00
#define FSM_STATE_IDLE  0x01
#define FSM_STATE_END   0xFE
#define FSM_STATE_INVALID 0xFF

typedef struct FsmStateItem
{
	UINT32 msg_id;
	UINT8 state;
	OPSTAT (*stateFunc)(UINT32 dest_id, UINT32 src_id, void *param_ptr, UINT32 param_len);
}FsmStateItem_t;

typedef struct HcuTaskTag
{
	UINT32 TaskId;
	UINT8  pnpState;
	INT32  processId;
	pthread_t ThrId;
	INT32  QueId;
	UINT8  state;
	char   taskName[TASK_NAME_MAX_LENGTH];
	FsmStateItem_t *fsmPtr;
	UINT8 QueFullFlag;
	void*  taskFuncEntry;
}HcuTaskTag_t;
#define HCU_TASK_QUEUE_FULL_NULL 0
#define HCU_TASK_QUEUE_FULL_TRUE 1
#define HCU_TASK_QUEUE_FULL_FALSE 2
#define HCU_TASK_QUEUE_FULL_INVALID 0xFF

typedef struct HcuCurrentTaskTag
{
	UINT32 curProcId;
	char   curProcName[TASK_NAME_MAX_LENGTH];
}HcuCurrentTaskTag_t;


typedef struct FsmArrayElement
{
	OPSTAT (*stateFunc)(UINT32 dest_id, UINT32 src_id, void *param_ptr, UINT32 param_len);
}FsmArrayElement_t;

typedef struct FsmCtrlTable
{
	UINT32 numOfFsmArrayElement;  //每一个具体任务TASK中，定义了多少个STATE-MSGID映射表单
	UINT32 taskId;
	FsmArrayElement_t pFsmArray[MAX_STATE_NUM_IN_ONE_TASK][MAX_MSGID_NUM_IN_ONE_TASK];
}FsmCtrlTable_t;

typedef struct FsmTable
{
	UINT32 numOfFsmCtrlTable;  //Number of running (Task + Instance)
	UINT32 currentTaskId;  //transfer task_id to launched FSM machine, then useless
	FsmCtrlTable_t  pFsmCtrlTable[MAX_TASK_NUM_IN_ONE_HCU];
}FsmTable_t;

//任务配置的基础配置信息
typedef struct StrHcuGlobalTaskInputConfig
{
	UINT8 taskInputId;
	char  taskInputName[TASK_NAME_MAX_LENGTH];
	void* fsmFuncEntry;
}StrHcuGlobalTaskInputConfig_t;

//Global variables 全局变量声明的好例子，不能加Static/Const等等修饰性，除非的确不改动
extern HcuTaskTag_t zHcuTaskInfo[MAX_TASK_NUM_IN_ONE_HCU];
extern char *zHcuTaskNameList[MAX_TASK_NUM_IN_ONE_HCU];
extern char *zHcuMsgNameList[MAX_MSGID_NUM_IN_ONE_TASK];
extern HcuCurrentTaskTag_t zHcuCurrentProcessInfo;

//任务模块RESTART的一些全局定义
#define HCU_RUN_ERROR_LEVEL_0_WARNING 10
#define HCU_RUN_ERROR_LEVEL_1_MINOR 100
#define HCU_RUN_ERROR_LEVEL_2_MAJOR 1000
#define HCU_RUN_ERROR_LEVEL_3_CRITICAL 10000
#define HCU_RUN_ERROR_LEVEL_4_DEAD 100000
extern UINT32 zHcuRunErrCnt[MAX_TASK_NUM_IN_ONE_HCU];

//全局Counter，用于性能指标统计之用
typedef struct HcuGlobalCounter
{
	UINT32 errCnt[MAX_TASK_NUM_IN_ONE_HCU];  //以每个任务为单位
	UINT32 restartCnt;
	UINT32 cloudVelaConnCnt;
	UINT32 cloudVelaConnFailCnt;
	UINT32 cloudVelaDiscCnt;
	UINT32 CloudDataTimeOutCnt;
	UINT32 SocketDiscCnt;
	UINT32 cpu_occupy;
	UINT32 mem_occupy;
	UINT32 disk_occupy;
}HcuGlobalCounter_t;
extern HcuGlobalCounter_t zHcuGlobalCounter;

//Global VM layer basic API and functions
extern void hcu_sw_restart(void);  //软件状态机初始化, TBD
extern OPSTAT hcu_task_create(UINT32 task_id, void *(*task_func)(void *), void *arg, int prio);
extern OPSTAT hcu_task_delete(UINT32 task_id);
extern OPSTAT hcu_msgque_create(UINT32 task_id);
extern OPSTAT hcu_msgque_delete(UINT32 task_id);
UINT32 hcu_msgque_inquery(UINT32 task_id);
extern OPSTAT hcu_msgque_resync(void);
extern OPSTAT hcu_task_create_and_run(UINT32 task_id, FsmStateItem_t* pFsmStateItem);
extern OPSTAT hcu_message_send(UINT32 msg_id, UINT32 dest_id, UINT32 src_id, void *param_ptr, UINT32 param_len); //message send
extern OPSTAT hcu_message_rcv(UINT32 dest_id, HcuMsgSruct_t *msg);
extern UINT32 hcu_message_rcv_no_wait(UINT32 dest_id, HcuMsgSruct_t *msg);
extern OPSTAT taskid_to_string(UINT32 id, char *string);
extern OPSTAT msgid_to_string(UINT32 id, char *string);
extern OPSTAT hcu_timer_polling(time_t sec, UINT32 nsec, void *handler());
extern OPSTAT hcu_timer_set(UINT32 timerid, UINT32 taskid, UINT32 delay);
extern OPSTAT hcu_timer_clear(UINT32 timerid, UINT32 taskid);
extern void hcu_sleep(UINT32 second);
extern void hcu_usleep(UINT32 usecond);  //resulution 10^(-6)s = 1 microsecond
//UNIX下时钟函数非常丰富，这里不再做任何抽象化，上层应用可以直接调用系统库函数进行使用和处理
extern UINT16 hcu_CRC_16(unsigned char *data,int len);
//高级错误打印方式
#define HCU_ERROR_PRINT_TASK(taskid, ...)	do{zHcuRunErrCnt[taskid]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)


/*FSM related APIs */
extern UINT32 FsmInit(void);
extern UINT32 FsmAddNew(UINT32 task_id, FsmStateItem_t* pFsmStateItem);
extern UINT32 FsmRemove(UINT32 task_id);
extern UINT32 FsmRunEngine(UINT32 msg_id, UINT32 dest_id, UINT32 src_id, void *param_ptr, UINT32 param_len);
extern UINT32 FsmProcessingLaunch(void);
extern OPSTAT FsmSetState(UINT32 task_id, UINT8 newState);
extern UINT8  FsmGetState(UINT32 task_id);
extern OPSTAT fsm_com_do_nothing(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_com_heart_beat_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Extern APIs
extern int msgget(key_t key, int msgflg);
extern int msgctl(int msqid, int cmd, struct msqid_ds *buf);
extern int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
extern ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
extern OPSTAT hcu_hwinv_read_engineering_data_into_mem(void);
extern OPSTAT hcu_hwinv_create_storage_dir_env(void);
extern OPSTAT hcu_hwinv_read_mac_address(void);


//统一定义，不会影响编译
extern FsmStateItem_t HcuFsmHcuvm[];                             //状态机
extern FsmStateItem_t HcuFsmTrace[];                             //状态机
extern FsmStateItem_t HcuFsmConfig[];                            //状态机
extern FsmStateItem_t HcuFsmTimer[];                             //状态机
extern FsmStateItem_t HcuFsmMmc[];                               //状态机
extern FsmStateItem_t HcuFsmGpio[];                              //状态机
extern FsmStateItem_t HcuFsmI2c[];                               //状态机
extern FsmStateItem_t HcuFsmSpi[];                               //状态机
extern FsmStateItem_t HcuFsmPwm[];                               //状态机
extern FsmStateItem_t HcuFsmAdc[];                               //状态机
extern FsmStateItem_t HcuFsmSwitch[];                            //状态机
extern FsmStateItem_t HcuFsmRelay[];                             //状态机
extern FsmStateItem_t HcuFsmMotor[];                             //状态机
extern FsmStateItem_t HcuFsmZeegbe[];                            //状态机
extern FsmStateItem_t HcuFsmGprs[];                              //状态机
extern FsmStateItem_t HcuFsmSps232[];                            //状态机
extern FsmStateItem_t HcuFsmSps485[];                            //状态机
extern FsmStateItem_t HcuFsmBle[];                               //状态机
extern FsmStateItem_t HcuFsmEthernet[];                          //状态机
extern FsmStateItem_t HcuFsmWifi[];                              //状态机
extern FsmStateItem_t HcuFsmUsbnet[];                            //状态机
extern FsmStateItem_t HcuFsm3g4g[];                              //状态机
extern FsmStateItem_t HcuFsmHarddisk[];                          //状态机
extern FsmStateItem_t HcuFsmCamera[];                            //状态机
extern FsmStateItem_t HcuFsmMicrophone[];                        //状态机
extern FsmStateItem_t HcuFsmFlash[];                             //状态机
extern FsmStateItem_t HcuFsmGps[];                               //状态机
extern FsmStateItem_t HcuFsmLcd[];                               //状态机
extern FsmStateItem_t HcuFsmLed[];                               //状态机
extern FsmStateItem_t HcuFsmHwinv[];                             //状态机
extern FsmStateItem_t HcuFsmSpsvirgo[];                          //状态机
extern FsmStateItem_t HcuFsmCloudvela[];                         //状态机
extern FsmStateItem_t HcuFsmModbus[];                            //状态机
extern FsmStateItem_t HcuFsmAvorion[];                           //状态机
extern FsmStateItem_t HcuFsmI2cbuslibra[];                       //状态机
extern FsmStateItem_t HcuFsmSpibusaries[];                       //状态机
extern FsmStateItem_t HcuFsmNbiotcj188[];                        //状态机
extern FsmStateItem_t HcuFsmNbiotqg376[];                        //状态机
extern FsmStateItem_t HcuFsmHsmmp[];                             //状态机
extern FsmStateItem_t HcuFsmEmc[];                               //状态机
extern FsmStateItem_t HcuFsmHumid[];                             //状态机
extern FsmStateItem_t HcuFsmPm25[];                              //状态机
extern FsmStateItem_t HcuFsmTemp[];                              //状态机
extern FsmStateItem_t HcuFsmWinddir[];                           //状态机
extern FsmStateItem_t HcuFsmWindspd[];                           //状态机
extern FsmStateItem_t HcuFsmNoise[];                             //状态机
extern FsmStateItem_t HcuFsmAirprs[];                            //状态机
extern FsmStateItem_t HcuFsmCo1[];                               //状态机
extern FsmStateItem_t HcuFsmLightstr[];                          //状态机
extern FsmStateItem_t HcuFsmAlcohol[];                           //状态机
extern FsmStateItem_t HcuFsmHcho[];                              //状态机
extern FsmStateItem_t HcuFsmToxicgas[];                          //状态机
extern FsmStateItem_t HcuFsmIwm[];                               //状态机
extern FsmStateItem_t HcuFsmIhm[];                               //状态机
extern FsmStateItem_t HcuFsmIgm[];                               //状态机
extern FsmStateItem_t HcuFsmIpm[];                               //状态机
extern FsmStateItem_t HcuFsmSvrcon[];                            //状态机
extern FsmStateItem_t HcuFsmSyspm[];                             //状态机
extern FsmStateItem_t HcuFsmPm25sharp[];                         //状态机
extern FsmStateItem_t HcuFsmCanitfleo[];                         //状态机
extern FsmStateItem_t HcuFsmL3aqycg10[];                         //状态机
extern FsmStateItem_t HcuFsmL3aqycg20[];                         //状态机
extern FsmStateItem_t HcuFsmL3tbswrg30[];                        //状态机
extern FsmStateItem_t HcuFsmLgqgybg40[];                         //状态机
extern FsmStateItem_t HcuFsmL3cxilc[];                           //状态机
extern FsmStateItem_t HcuFsmL3cxglacm[];                         //状态机
extern FsmStateItem_t HcuFsmL3nblpm[];                           //状态机
extern FsmStateItem_t HcuFsmL3nbhpm[];                           //状态机
extern FsmStateItem_t HcuFsmL3bfsc[];                            //状态机
extern FsmStateItem_t HcuFsmBfscuicomm[];                        //状态机
extern FsmStateItem_t HcuFsmL3opwlotdr[];                        //状态机


//高级定义，简化程序的可读性
#define HCU_DEBUG_PRINT_INF		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_INF_ON) != FALSE) HcuDebugPrint
#define HCU_DEBUG_PRINT_NOR		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE) HcuDebugPrint
#define HCU_DEBUG_PRINT_IPT		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_IPT_ON) != FALSE) HcuDebugPrint
#define HCU_DEBUG_PRINT_CRT		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_CRT_ON) != FALSE) HcuDebugPrint
#define HCU_DEBUG_PRINT_FAT		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE) HcuDebugPrint

//移植并来自HCU主入口模块
extern int hcu_vm_main_entry(void);
//VM初始化
OPSTAT hcu_vm_system_init(void);  //系统级别的初始化
OPSTAT hcu_vm_application_task_env_init();
//不同WORK MODE下进程的启动函数
void hcu_vm_working_mode_single_start(void);
void hcu_vm_working_mode_double_start(void);
void hcu_vm_working_mode_multipy_start(void);
//进程启动函数主入口
void hcu_vm_process_single_mainapp_entry(void);
//对于双进程及多进程模式，还需要再仔细考量，目前暂不推荐使用，而且也没有经过仔细测试
void hcu_vm_process_double_mainapp_entry(void);
void hcu_vm_process_multipy_mainapp_entry(void);
void hcu_vm_process_create_sensor_avorion_only(void);
void hcu_vm_process_multipy_entry_supervisor(void);
//VM本地函数
//UINT32 hcu_vm_system_task_init(void);
OPSTAT hcu_vm_system_task_init_call(UINT32 task_id, FsmStateItem_t *p);
void hcu_vm_task_send_init_to_timer(void);
void hcu_vm_task_send_init_to_avorion(void);
void hcu_vm_task_send_init_to_svrcon(void);
void hcu_vm_task_send_hb_to_svrcon(void);
void hcu_vm_task_create_all(void);
void hcu_vm_task_create_all_but_avorion(void);
void hcu_vm_task_create_hcumain_env(void);
void hcu_vm_task_delete_all_and_queue(void);
void hcu_vm_task_delete_except_avorion_and_hcumain(void);
void hcu_vm_task_delete_except_timer_and_hcumain(void);
void hcu_vm_task_delete_except_svrcon_and_hcumain(void);


#endif /* L0COMVM_VMLAYER_H_ */
