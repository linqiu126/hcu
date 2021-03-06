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
	TASK_ID_GRPS,
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
	UINT8  hwPlugin;
	UINT8  hwActive;
	UINT8  swTaskActive;
	INT32  processId;
	pthread_t ThrId;
	INT32  QueId;
	UINT8  state;
	char   TaskName[TASK_NAME_MAX_LENGTH];
	FsmStateItem_t *fsmPtr;
	UINT8 QueFullFlag;
}HcuTaskTag_t;
#define HCU_SYSCFG_TASK_PNP_ON 2
#define HCU_SYSCFG_TASK_PNP_OFF 1
#define HCU_SYSCFG_TASK_PNP_INVALID 0xFF
#define HCU_TASK_SW_ACTIVE 2
#define HCU_TASK_SW_DEACTIVE 1
#define HCU_TASK_SW_INVALID 0xFF
#define HCU_TASK_QUEUE_FULL_TRUE 2
#define HCU_TASK_QUEUE_FULL_FALSE 1
#define HCU_TASK_QUEUE_FULL_INVALID 0xFF

typedef struct HcuCurrentTaskTag
{
	UINT32 curProcId;
	char   curProcName[TASK_NAME_MAX_LENGTH];
}HcuCurrentTaskTag_t;


typedef struct FsmArrayElement
{
	OPSTAT (*stateFunc)(UINT32 dest_id, UINT32 src_id, void *param_ptr, UINT32 param_len);
}HcuFsmArrayElement_t;

typedef struct HcuFsmCtrlTable
{
	UINT32 numOfFsmArrayElement;  //每一个具体任务TASK中，定义了多少个STATE-MSGID映射表单
	UINT32 taskId;
	HcuFsmArrayElement_t pFsmArray[MAX_STATE_NUM_IN_ONE_TASK][MAX_MSGID_NUM_IN_ONE_TASK];
}FsmCtrlTable_t;

typedef struct HcuFsmTable
{
	UINT32 numOfFsmCtrlTable;  //Number of running (Task + Instance)
	UINT32 currentTaskId;  //transfer task_id to launched FSM machine, then useless
	FsmCtrlTable_t  pFsmCtrlTable[MAX_TASK_NUM_IN_ONE_HCU];
}HcuFsmTable_t;

//Global variables 全局变量声明的好例子，不能加Static/Const等等修饰性，除非的确不改动
extern HcuTaskTag_t zHcuTaskInfo[MAX_TASK_NUM_IN_ONE_HCU];
extern char *zHcuTaskNameList[MAX_TASK_NUM_IN_ONE_HCU];
extern char *zHcuSysEngTrcModCtrStaticCfg[MAX_MSGID_NUM_IN_ONE_TASK];
extern HcuCurrentTaskTag_t zHcuSysCtrTab.curProc;

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
	UINT32 cloudVelaDiscCnt;
	UINT32 CloudDataTimeOutCnt;
	UINT32 SocketDiscCnt;
}HcuGlobalCounter_t;
extern HcuGlobalCounter_t zHcuGlobalCounter;

//Global VM layer basic API and functions
extern void hcu_vm_system_init(void);  //系统级别的初始化
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
extern OPSTAT hcu_vm_msgid_to_string(UINT32 id, char *string);
extern OPSTAT hcu_timer_polling(time_t sec, UINT32 nsec, void *handler());
extern OPSTAT hcu_timer_set(UINT32 timerid, UINT32 taskid, UINT32 delay);
extern OPSTAT hcu_timer_clear(UINT32 timerid, UINT32 taskid);
extern void hcu_sleep(UINT32 second);
extern void hcu_usleep(UINT32 usecond);  //resulution 10^(-6)s = 1 microsecond
//UNIX下时钟函数非常丰富，这里不再做任何抽象化，上层应用可以直接调用系统库函数进行使用和处理
extern UINT16 hcu_CRC_16(unsigned char *data,int len);

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


#endif /* L0COMVM_VMLAYER_H_ */
