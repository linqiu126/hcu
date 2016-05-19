/*
 * gpio.c
 *
 *  Created on: 2016年5月10日
 *      Author: test
 */

#include "../l1hwopt/gpio.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the GPIO
*/
FsmStateItem_t FsmGpio[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_gpio_task_entry}, //Starting

	//System level initialisation, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_gpio_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_gpio_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialiSation
    {MSG_ID_COM_RESTART,        FSM_STATE_GPIO_RECEIVED,            	fsm_gpio_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_GPIO_RECEIVED,            	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_GPIO_RECEIVED,       			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_GPIO_RECEIVED,       			fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表
float zHcuGpioTempDht11;
float zHcuGpioHumidDht11;
float zHcuGpioToxicgasMq135;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_gpio_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_GPIO, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("GPIO: Error Set FSM State at fsm_gpio_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_gpio_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_GPIO, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("GPIO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_GPIO], zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_GPIO, FSM_STATE_GPIO_INITED) == FAILURE){
		HcuErrorPrint("GPIO: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_gpio_int_init() == FAILURE){
		HcuErrorPrint("GPIO: Error initialiSe interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuRunErrCnt[TASK_ID_GPIO] = 0;
	zHcuGpioTempDht11 = HCU_SENSOR_VALUE_NULL;
	zHcuGpioHumidDht11 = HCU_SENSOR_VALUE_NULL;
	zHcuGpioToxicgasMq135 = HCU_SENSOR_VALUE_NULL;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_GPIO, FSM_STATE_GPIO_RECEIVED) == FAILURE){
		zHcuRunErrCnt[TASK_ID_GPIO]++;
		HcuErrorPrint("GPIO: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("GPIO: Enter FSM_STATE_GPIO_ACTIVED status, Keeping refresh here!\n");
	}

	//进入循环工作模式
	while(1){
		func_gpio_read_data_dht11();
		hcu_sleep(RPI_GPIO_SENSOR_READ_GAP/2);
		func_gpio_read_data_mq135();
		hcu_sleep(RPI_GPIO_SENSOR_READ_GAP/2);
	}

	return SUCCESS;
}

OPSTAT fsm_gpio_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("GPIO: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuGlobalCounter.restartCnt++;
	fsm_gpio_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_gpio_int_init(void)
{
	return SUCCESS;
}

//Starting point for Raspberry-Pi function definition
//目前传感器读取有个问题：
//1.如果传感器没有连接，要么该线程死锁，要么读取错误的数据，需要搞定如何判定
//2.传感器中途拔掉，如何检测出来
//3.如何确定最优读取间隔时间，以便既能降低读取频度，又能及时反应其变化

UINT32 databuf;
OPSTAT func_gpio_read_data_dht11(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int i;
	float tmp1, tmp2, tempSum, humidSum;
    pinMode(RPI_GPIO_PIN_DHT11_DATA, OUTPUT);
    digitalWrite(RPI_GPIO_PIN_DHT11_DATA, 1);

	tempSum = 0;
	humidSum = 0;
	databuf = 0;
	for (i=0; i<RPI_GPIO_READ_REPEAT_TIMES; i++){
		delay(200);
        pinMode(RPI_GPIO_PIN_DHT11_DATA, OUTPUT);
        digitalWrite(RPI_GPIO_PIN_DHT11_DATA, 1);
        delay(200); //wiringPi functions
        if(func_gpio_readSensorDht11Data(RPI_GPIO_PIN_DHT11_DATA))
        {
//        	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
//            	HcuDebugPrint("GPIO: Sensor DHT11 Original read result Temp=%d.%dC, Humid=%d.%d\%, DATA_GPIO#=%d\n", (databuf>>8)&0xFF, databuf&0xFF, (databuf>>24)&0xFF, (databuf>>16)&0xFF, RPI_GPIO_PIN_DHT11_DATA);
//        	}
        	tmp1 =  (databuf>>8)&0xFF;
        	tmp2 = databuf&0xFF;
        	tempSum += tmp1 + tmp2/256;
        	tmp1 =  (databuf>>24)&0xFF;
        	tmp2 = (databuf>>16)&0xFF;
        	humidSum += tmp1 + tmp2/256;
            databuf=0;
        }
        else
        {
            databuf=0;
        }
    }

	//求平均
	zHcuGpioTempDht11 = tempSum / RPI_GPIO_READ_REPEAT_TIMES;
	zHcuGpioHumidDht11 = humidSum / RPI_GPIO_READ_REPEAT_TIMES;
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
    	HcuDebugPrint("GPIO: Sensor DHT11 Transformed float result Temp=%6.2fC, Humid=%6.2f\%, DATA_GPIO#=%d\n", zHcuGpioTempDht11, zHcuGpioHumidDht11, RPI_GPIO_PIN_DHT11_DATA);
	}

    return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

//读取MQ135气体传感器的数据
OPSTAT func_gpio_read_data_mq135(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int toxicgas, i;
	float toxicgasSum;

	toxicgasSum = 0;
	for (i=0; i<RPI_GPIO_READ_REPEAT_TIMES; i++){
		delay(100);
	    pinMode(RPI_GPIO_PIN_MQ135_DATA, INPUT);
	    delay(100);
	    //目前还有较多的问题，读取的数据总是HIGH，待完善
	    toxicgas = digitalRead(RPI_GPIO_PIN_MQ135_DATA);
	    toxicgasSum += toxicgas;

//		if ((toxicgas == 1) && (zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE)
//			HcuDebugPrint("GPIO: Sensor MQ135 Original read result pollution= [HIGH], DATA_GPIO#=%d\n", RPI_GPIO_PIN_MQ135_DATA);
//		else if ((toxicgas == 0) && (zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE)
//			HcuDebugPrint("GPIO: Sensor MQ135 Original read result pollution= [LOW], DATA_GPIO#=%d\n", RPI_GPIO_PIN_MQ135_DATA);
//		else
//		{
//			if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
//				HcuDebugPrint("GPIO: Sensor MQ135 Original read result pollution= [NULL-%d], DATA_GPIO#=%d\n", toxicgas, RPI_GPIO_PIN_MQ135_DATA);
//			}
//		}
	}

	//求平均
	zHcuGpioToxicgasMq135 = toxicgasSum / RPI_GPIO_READ_REPEAT_TIMES;
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("GPIO: Sensor MQ135 Transformed float average read result pollution= %d[Times], DATA_GPIO#=%d\n", (int)zHcuGpioToxicgasMq135, RPI_GPIO_PIN_MQ135_DATA);
	}

	//先准备好如此的数据存储框架，未来再改进该数据的正确性

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

#ifdef TARGET_RASPBERRY_PI3B
UINT8 func_gpio_readSensorDht11Data(UINT32 pin)
{
	UINT8 crc;
	UINT8 i;

    pinMode(pin, OUTPUT);
    digitalWrite(pin, 0);
    delay(25);
    digitalWrite(pin, 1);
    pinMode(pin, INPUT);
    pullUpDnControl(pin, PUD_UP);

    delayMicroseconds(27);
    if(digitalRead(pin)==0)
    {
        while(!digitalRead(pin));

        for(i=0;i<32;i++)
        {
            while(digitalRead(pin));
            while(!digitalRead(pin));
            delayMicroseconds(RPI_GPIO_HIGH_TIME);
            databuf*=2;
            if(digitalRead(pin)==1)
            {
                databuf++;
            }
        }

        for(i=0;i<8;i++)
        {
            while(digitalRead(pin));
            while(!digitalRead(pin));
            delayMicroseconds(RPI_GPIO_HIGH_TIME);
            crc*=2;
            if(digitalRead(pin)==1)
            {
                crc++;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}
#endif




//Inherit from old APIGPIO.C
/*
** Local static variables
*/

//估计不会用到这个变量，待确定
//static unsigned char traceFileName[] = __FILE__;


#define BASE 0x7f008000

static volatile void *mem;

#define CON_IN	0
#define CON_OUT	1
#define CON_F1	2
#define CON_F2	3
#define CON_F3	4
#define CON_F4	5
#define CON_F5	6
#define CON_INT	7

#define	R_Z	0
#define	R_DOWN	1
#define	R_UP	2


/* 6410 port assignment */
static struct port {
	const char *name;
	int offset;
	int last;
	enum { pt_a, pt_f, pt_h } type;
		} ports[] = {
			{ "A", 0x000,  8, pt_a },
			{ "B", 0x020,  7, pt_a },
			{ "C", 0x040,  8, pt_a },
			{ "D", 0x060,  5, pt_a },
			{ "E", 0x080,  5, pt_a },
			{ "F", 0x0a0, 16, pt_f },
			{ "G", 0x0c0,  7, pt_a },
			{ "H", 0x0e0, 10, pt_h },
			{ "I", 0x100, 16, pt_f },
			{ "J", 0x120, 12, pt_f },
			{ "K", 0x800, 16, pt_h },
			{ "L", 0x810, 15, pt_h },
			{ "M", 0x820,  6, pt_a },
			{ "N", 0x830, 16, pt_f },
			{ "O", 0x140, 16, pt_f },
			{ "P", 0x160, 15, pt_f },
			{ "Q", 0x180,  9, pt_f },
			{ NULL, }
};


static void print_n(const char *name, int last)
{
	int i;

	printf("%s ", name);
	for (i = 0; i <= last; i++)
		printf("%2d ", i);
	putchar('\n');
}


/* ----- Read a pin -------------------------------------------------------- */


static const char *pin_a(int con, int dat, int pud)
{
	static char res[4];

	res[0] = " >ABCDEIXXXXXXXX"[con];
	res[1] = dat ? '1' : '0';
	res[2] = " DUX"[pud];
	return res;
}


static const char *pin_f(int con, int dat, int pud)
{
	static char res[4];

	res[0] = " >FI"[con];
	res[1] = dat ? '1' : '0';
	res[2] = " DUX"[pud];
	return res;
}


static const char *pin(const struct port *p, int num)
{
	uint32_t con, con1, dat, pud;

	con = *(uint32_t *) (mem+p->offset);
	if (p->type != pt_h) {
		dat = *(uint32_t *) (mem+p->offset+4);
		pud = *(uint32_t *) (mem+p->offset+8);
	} else {
		con1 = *(uint32_t *) (mem+p->offset+4);
		dat = *(uint32_t *) (mem+p->offset+8);
		pud = *(uint32_t *) (mem+p->offset+12);
	}

	switch (p->type) {
	case pt_a:
		return pin_a(
		    (con >> (num*4)) & 15,
		    (dat >> num) & 1,
		    (pud >> (num*2)) & 3);
	case pt_f:
		return pin_f(
		    (con >> (num*2)) & 3,
		    (dat >> num) & 1,
		    (pud >> (num*2)) & 3);
	case pt_h:
		return pin_a(
		    num < 8 ? (con >> (num*4)) & 15 : (con1 >> (num*4-32)) & 15,
		    (dat >> num) & 1,
		    (pud >> (num*2)) & 3);
	default:
		abort();
	}
}


/* ----- Set a pin --------------------------------------------------------- */


static void set_a(const struct port *p, int num, int c, int d, int r)
{
	uint32_t con, dat, pud;

	con = *(uint32_t *) (mem+p->offset);
	con = (con & ~(15 << (num*4))) | (c << (num*4));
	*(uint32_t *) (mem+p->offset) = con;

	if (d != -1) {
		dat = *(uint32_t *) (mem+p->offset+4);
		dat = (dat & ~(1 << num)) | (d << num);
		*(uint32_t *) (mem+p->offset+4) = dat;
	}

	pud = *(uint32_t *) (mem+p->offset+8);
	pud = (pud & ~(15 << (num*2))) | (r << (num*2));
	*(uint32_t *) (mem+p->offset+8) = pud;
}


static void set_f(const struct port *p, int num, int c, int d, int r)
{
	uint32_t con, dat, pud;

	con = *(uint32_t *) (mem+p->offset);
	con = (con & ~(3 << (num*2))) | (c << (num*2));
	*(uint32_t *) (mem+p->offset) = con;

	if (d != -1) {
		dat = *(uint32_t *) (mem+p->offset+4);
		dat = (dat & ~(1 << num)) | (d << num);
		*(uint32_t *) (mem+p->offset+4) = dat;
	}

	pud = *(uint32_t *) (mem+p->offset+8);
	pud = (pud & ~(3 << (num*2))) | (r << (num*2));
	*(uint32_t *) (mem+p->offset+8) = pud;
}


static void set_h(const struct port *p, int num, int c, int d, int r)
{
	uint32_t con, dat, pud;

	if (num < 8) {
		con = *(uint32_t *) (mem+p->offset);
		con = (con & ~(15 << (num*4))) | (c << (num*4));
		*(uint32_t *) (mem+p->offset) = con;
	} else {
		con = *(uint32_t *) (mem+p->offset+4);
		con = (con & ~(15 << (num*4-32))) | (c << (num*4-32));
		*(uint32_t *) (mem+p->offset+4) = con;
	}

	if (d != -1) {
		dat = *(uint32_t *) (mem+p->offset+8);
		dat = (dat & ~(1 << num)) | (d << num);
		*(uint32_t *) (mem+p->offset+8) = dat;
	}

	pud = *(uint32_t *) (mem+p->offset+12);
	pud = (pud & ~(15 << (num*4))) | (r << (num*4));
	*(uint32_t *) (mem+p->offset+12) = pud;
}


static void set_pin(const struct port *p, int num, int c, int d, int r)
{
	if (num > p->last) {
		fprintf(stderr, "invalid pin %s%d\n", p->name, num);
		exit(1);
	}
	switch (p->type) {
	case pt_a:
		set_a(p, num, c, d, r);
		break;
	case pt_f:
		if (c > 3) {
			fprintf(stderr, "pin %s%d is F-type\n", p->name, num);
			exit(1);
		}
		set_f(p, num, c, d, r);
		break;
	case pt_h:
		set_h(p, num, c, d, r);
		break;
	default:
		abort();
	}
}


/* ----- Dump all ports ---------------------------------------------------- */


static void dump_all(void)
{
fprintf(stderr, "not yet implemented\n");
exit(1);
#if 0
	const struct port *p;
	uint32_t con, dat, pud;
	int i;

	for (p = ports; p->name; p++) {
		con = *(uint32_t *) (mem+p->offset);
		dat = *(uint32_t *) (mem+p->offset+4);
		if (p->type == pt_a) {
			print_n(p->name, p->last);
			printf("%*s ", strlen(p->name), "");
			for (i = 0; i <= p->last; i++)
				printf("%s ",
				    pin_a((con >> i) & 1, (dat >> i) & 1));
			putchar('\n');
		}
		else {
			pud = *(uint32_t *) (mem+p->offset+8);
			print_n(p->name, p->last);
			printf("%*s ", strlen(p->name), "");
			for (i = 0; i <= p->last; i++)
				printf("%s",
				    pin_a((con >> (i*2)) & 3, (dat >> i) & 1,
				    (pud >> i) & 1));
			putchar('\n');
		}
	}
#endif
}


/* ----- Command-line parsing ---------------------------------------------- */


static void __attribute__((noreturn)) usage(const char *name)
{
	fprintf(stderr,
"usage: %s [pin[=value] ...]\n\n"
"  pin = <letter><number>, e.g., A5, b10\n"
"  value = <control><pull-down>, with\n"
"    <control> = 0 (output 0), 1 (output 1), Z (input), A or F (primary\n"
"                function), B, C, ... E (additional function), I (interrupt)\n"
"    <pull-down> = D (pull-down), U (pull-up), or nothing\n\n"
"  Examples: A5=ZR, b10=0\n",
    name);
	exit(1);
}


static void port_op(const char *name, const char *op)
{
	const char *eq, *s;
	const struct port *p;
	int num, c, d = -1, r = R_Z;
	char *end;

	eq = strchr(op, '=');
	if (!eq)
		eq = strchr(op, 0);
	num = strcspn(op, "0123456789");
	if (!num || op+num >= eq)
		usage(name);
	for (p = ports; p->name; p++)
		if (strlen(p->name) == num && !strncasecmp(p->name, op, num))
			break;
	if (!p->name) {
		fprintf(stderr, "invalid port \"%.*s\"\n", num, op);
		exit(1);
	}
	num = strtoul(op+num, &end, 10);
	if (end != eq)
		usage(name);
	if (!*eq) {
		s = pin(p, num);
		if (*s == ' ')
			s++;
		printf("%s\n", s);
		return;
	}
	switch (eq[1]) {
	case '0':
		d = 0;
		c = CON_OUT;
		break;
	case '1':
		d = 1;
		c = CON_OUT;
		break;
	case 'Z':
	case 'z':
		c = CON_IN;
		break;
	case 'F':
	case 'f':
	case 'A':
	case 'a':
		c = CON_F1;
		break;
	case 'B':
	case 'b':
		c = CON_F2;
		break;
	case 'C':
	case 'c':
		c = CON_F3;
		break;
	case 'D':
	case 'd':
		c = CON_F4;
		break;
	case 'E':
	case 'e':
		c = CON_F5;
		break;
	case 'I':
	case 'i':
		c = CON_INT;
		break;
	default:
		usage(name);
	}
	if (eq[2]) {
		switch (eq[2]) {
		case 'D':
		case 'd':
			r = R_DOWN;
			break;
		case 'U':
		case 'u':
			r = R_UP;
			break;
		default:
			usage(name);
		}
		if (eq[3])
			usage(name);
	}
	set_pin(p, num, c, d, r);
}


UINT32 GpioInit()
{

	int fd;

	HcuDebugPrint("GpioInit: start ...\n");

	fd = open("/dev/mem", O_RDWR);
    if (fd < 0)
    {
		HcuErrorPrint("GpioInit: Open [/dev/mem] failed.\n");
		return FAILURE;
	}

	mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE);
	if (mem == MAP_FAILED)
	{
		HcuErrorPrint("GpioInit: [/mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE);] failed.\n");
		return FAILURE;
	}

	HcuDebugPrint("GpioInit: completed ...\n");
	return SUCCESS;
}

void GpioPortOp(char *op)
{
	/*
	 * "usage: %s [pin[=value] ...]\n\n"
	 * "  pin = <letter><number>, e.g., A5, b10\n"
	 * "  value = <control><pull-down>, with\n"
	 * "    <control> = 0 (output 0), 1 (output 1), Z (input), A or F (primary\n"
	 * "                function), B, C, ... E (additional function), I (interrupt)\n"
	 * "    <pull-down> = D (pull-down), U (pull-up), or nothing\n\n"
	 * "  Examples: A5=ZR, b10=0\n",
	 *
	 * 	 * GPM0-3 -> LED0-3
	 * 	 *
	 */
	if ( NULL!=op)
	{
		HcuDebugPrint("GpioPortOp: call API port_op(\"GpioPortOp\", %s)\n", op);
		port_op("GpioSet", op);
	}
	else
	{
		HcuDebugPrint("GpioPortOp: NULL op code defined, return");
	}
}

#if 0
int main(int argc, char **argv)
{
	int fd, i;

	fd = open("/dev/mem", O_RDWR);
        if (fd < 0) {
		perror("/dev/mem");
		exit(1);
	}
	mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE);
	if (mem == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	if (argc == 1)
		dump_all();
	else
		for (i = 1; i != argc; i++)
			port_op(*argv, argv[i]);
	return 0;
}
#endif





