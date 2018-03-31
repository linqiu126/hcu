/*
 * Test.c
 *
 *  Created on: Mar 31, 2018
 *      Author: hitpony
 */



#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>

#define KEY_RESERVED        0
#define KEY_ESC            1
#define KEY_1            2
#define KEY_2            3
#define KEY_3            4
#define KEY_4            5
#define KEY_5            6
#define KEY_6            7
#define KEY_7            8
#define KEY_8            9
#define KEY_9            10
#define KEY_0            11
#define KEY_MINUS        12
#define KEY_EQUAL        13
#define KEY_BACKSPACE        14
#define KEY_TAB            15
#define KEY_Q            16
#define KEY_W            17
#define KEY_E            18
#define KEY_R            19
#define KEY_T            20
#define KEY_Y            21
#define KEY_U            22
#define KEY_I            23
#define KEY_O            24
#define KEY_P            25
#define KEY_LEFTBRACE        26
#define KEY_RIGHTBRACE        27
#define KEY_ENTER        28
#define KEY_LEFTCTRL        29
#define KEY_A            30
#define KEY_S            31
#define KEY_D            32
#define KEY_F            33
#define KEY_G            34
#define KEY_H            35
#define KEY_J            36
#define KEY_K            37
#define KEY_L            38
#define KEY_SEMICOLON        39
#define KEY_APOSTROPHE        40
#define KEY_GRAVE        41
#define KEY_LEFTSHIFT        42
#define KEY_BACKSLASH        43
#define KEY_Z            44
#define KEY_X            45
#define KEY_C            46
#define KEY_V            47
#define KEY_B            48
#define KEY_N            49
#define KEY_M            50
#define KEY_COMMA        51
#define KEY_DOT            52
#define KEY_SLASH        53
#define KEY_RIGHTSHIFT        54
#define KEY_KPASTERISK        55
#define KEY_LEFTALT        56
#define KEY_SPACE        57
#define KEY_CAPSLOCK        58
#define KEY_F1            59
#define KEY_F2            60
#define KEY_F3            61
#define KEY_F4            62
#define KEY_F5            63
#define KEY_F6            64
#define KEY_F7            65
#define KEY_F8            66
#define KEY_F9            67
#define KEY_F10            68
#define KEY_NUMLOCK        69
#define KEY_SCROLLLOCK        70
#define KEY_KP7            71
#define KEY_KP8            72
#define KEY_KP9            73
#define KEY_KPMINUS        74
#define KEY_KP4            75
#define KEY_KP5            76
#define KEY_KP6            77
#define KEY_KPPLUS        78
#define KEY_KP1            79
#define KEY_KP2            80
#define KEY_KP3            81
#define KEY_KP0            82
#define KEY_KPDOT        83

#define KEY_ZENKAKUHANKAKU    85
#define KEY_102ND        86
#define KEY_F11            87
#define KEY_F12            88
#define KEY_RO            89
#define KEY_KATAKANA        90
#define KEY_HIRAGANA        91
#define KEY_HENKAN        92
#define KEY_KATAKANAHIRAGANA    93
#define KEY_MUHENKAN        94
#define KEY_KPJPCOMMA        95
#define KEY_KPENTER        96
#define KEY_RIGHTCTRL        97
#define KEY_KPSLASH        98
#define KEY_SYSRQ        99
#define KEY_RIGHTALT        100
#define KEY_LINEFEED        101
#define KEY_HOME        102
#define KEY_UP            103
#define KEY_PAGEUP        104
#define KEY_LEFT        105
#define KEY_RIGHT        106
#define KEY_END            107
#define KEY_DOWN        108
#define KEY_PAGEDOWN        109
#define KEY_INSERT        110
#define KEY_DELETE        111
#define KEY_MACRO        112
#define KEY_MUTE        113
#define KEY_VOLUMEDOWN        114
#define KEY_VOLUMEUP        115
#define KEY_POWER        116    /* SC System Power Down */
#define KEY_KPEQUAL        117
#define KEY_KPPLUSMINUS        118
#define KEY_PAUSE        119
#define KEY_SCALE        120    /* AL Compiz Scale (Expose) */

#define KEY_KPCOMMA        121
#define KEY_HANGEUL        122
#define KEY_HANGUEL        KEY_HANGEUL
#define KEY_HANJA        123
#define KEY_YEN            124
#define KEY_LEFTMETA        125
#define KEY_RIGHTMETA        126
#define KEY_COMPOSE        127

#define KEY_STOP        128    /* AC Stop */
#define KEY_AGAIN        129
#define KEY_PROPS        130    /* AC Properties */
#define KEY_UNDO        131    /* AC Undo */
#define KEY_FRONT        132
#define KEY_COPY        133    /* AC Copy */
#define KEY_OPEN        134    /* AC Open */
#define KEY_PASTE        135    /* AC Paste */
#define KEY_FIND        136    /* AC Search */
#define KEY_CUT            137    /* AC Cut */
#define KEY_HELP        138    /* AL Integrated Help Center */
#define KEY_MENU        139    /* Menu (show menu) */
#define KEY_CALC        140    /* AL Calculator */
#define KEY_SETUP        141
#define KEY_SLEEP        142    /* SC System Sleep */
#define KEY_WAKEUP        143    /* System Wake Up */
#define KEY_FILE        144    /* AL Local Machine Browser */
#define KEY_SENDFILE        145
#define KEY_DELETEFILE        146
#define KEY_XFER        147
#define KEY_PROG1        148
#define KEY_PROG2        149
#define KEY_WWW            150    /* AL Internet Browser */
#define KEY_MSDOS        151
#define KEY_COFFEE        152    /* AL Terminal Lock/Screensaver */
#define KEY_SCREENLOCK        KEY_COFFEE
//#define KEY_DIRECTION        153
#define KEY_CYCLEWINDOWS    154
#define KEY_MAIL        155
#define KEY_BOOKMARKS        156    /* AC Bookmarks */
#define KEY_COMPUTER        157
#define KEY_BACK        158    /* AC Back */
#define KEY_FORWARD        159    /* AC Forward */
#define KEY_CLOSECD        160
#define KEY_EJECTCD        161
#define KEY_EJECTCLOSECD    162
#define KEY_NEXTSONG        163
#define KEY_PLAYPAUSE        164
#define KEY_PREVIOUSSONG    165
#define KEY_STOPCD        166
#define KEY_RECORD        167
#define KEY_REWIND        168
#define KEY_PHONE        169    /* Media Select Telephone */
#define KEY_ISO            170
#define KEY_CONFIG        171    /* AL Consumer Control Configuration */
#define KEY_HOMEPAGE        172    /* AC Home */
#define KEY_REFRESH        173    /* AC Refresh */
#define KEY_EXIT        174    /* AC Exit */
#define KEY_MOVE        175
#define KEY_EDIT        176
#define KEY_SCROLLUP        177
#define KEY_SCROLLDOWN        178
#define KEY_KPLEFTPAREN        179
#define KEY_KPRIGHTPAREN    180
#define KEY_NEW            181    /* AC New */
#define KEY_REDO        182    /* AC Redo/Repeat */

#define KEY_F13            183
#define KEY_F14            184
#define KEY_F15            185
#define KEY_F16            186
#define KEY_F17            187
#define KEY_F18            188
#define KEY_F19            189
#define KEY_F20            190
#define KEY_F21            191
#define KEY_F22            192
#define KEY_F23            193
#define KEY_F24            194

#define KEY_PLAYCD        200
#define KEY_PAUSECD        201
#define KEY_PROG3        202
#define KEY_PROG4        203
#define KEY_DASHBOARD        204    /* AL Dashboard */
#define KEY_SUSPEND        205
#define KEY_CLOSE        206    /* AC Close */
#define KEY_PLAY        207
#define KEY_FASTFORWARD        208
#define KEY_BASSBOOST        209
#define KEY_PRINT        210    /* AC Print */
#define KEY_HP            211
#define KEY_CAMERA        212
#define KEY_SOUND        213
#define KEY_QUESTION        214
#define KEY_EMAIL        215
#define KEY_CHAT        216
#define KEY_SEARCH        217
#define KEY_CONNECT        218
#define KEY_FINANCE        219    /* AL Checkbook/Finance */
#define KEY_SPORT        220
#define KEY_SHOP        221
#define KEY_ALTERASE        222
#define KEY_CANCEL        223    /* AC Cancel */
#define KEY_BRIGHTNESSDOWN    224
#define KEY_BRIGHTNESSUP    225
#define KEY_MEDIA        226

#define KEY_SWITCHVIDEOMODE    227    /* Cycle between available video
                       outputs (Monitor/LCD/TV-out/etc) */
#define KEY_KBDILLUMTOGGLE    228
#define KEY_KBDILLUMDOWN    229
#define KEY_KBDILLUMUP        230

#define KEY_SEND        231    /* AC Send */
#define KEY_REPLY        232    /* AC Reply */
#define KEY_FORWARDMAIL        233    /* AC Forward Msg */
#define KEY_SAVE        234    /* AC Save */
#define KEY_DOCUMENTS        235

#define KEY_BATTERY        236

#define KEY_BLUETOOTH        237
#define KEY_WLAN        238
#define KEY_UWB            239

#define KEY_UNKNOWN        240

#define KEY_VIDEO_NEXT        241    /* drive next video source */
#define KEY_VIDEO_PREV        242    /* drive previous video source */
#define KEY_BRIGHTNESS_CYCLE    243    /* brightness up, after max is min */
//#define KEY_BRIGHTNESS_ZERO    244    /* brightness off, use ambient */
#define KEY_DISPLAY_OFF        245    /* display device to off state */

//#define KEY_WIMAX        246
#define KEY_RFKILL        247    /* Key that controls all radios */

#define KEY_MICMUTE        248    /* Mute / unmute the microphone */

/* Code 255 is reserved for special needs of AT keyboard driver */


#define outMaxLen  100
char outputFormal[outMaxLen];



int kbCapture(int eventId, char* output)
{
	int outCnt = 0;

	char dev[100];
	sprintf(dev, "/dev/input/event%d", eventId);
	int fd=open(dev, O_RDWR);
    if(fd <= 0){
        printf("Can not open keyboard input file\n");
        return 0;
     }

    //struct input_event *event;
    char buf[128]={0};
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd , &rfds);

    memset((void*)output, 0, outMaxLen);
    outCnt = 0;
    while (1) {
        int ret = select(fd +1, &rfds, NULL, NULL, NULL);
        if (ret < 0)
            return 0;
        if (FD_ISSET(fd , &rfds)) {
           int readn = read(fd , buf, sizeof(struct input_event));
            if (readn <= 0) {
                printf("uart read error %d\n", readn);
                return 0;
            }

            struct input_event *my_event=(struct input_event*)buf;
            if ((my_event->type==EV_KEY) && (my_event->value == 1))
            {
            	outCnt++;
            	if (outCnt >= outMaxLen){
            		memset(output, 0, outMaxLen);
            		outCnt = 0;
            	}
            	else if (my_event->code == KEY_ENTER){
            		//printf("Return string len = %d\n", strLen);
            		int resLen = outCnt-1;
            		outCnt = 0;
            		close(fd);
            		return resLen;
            	}
            	else{
            		if ((my_event->code >= KEY_1) && (my_event->code <= KEY_9)){
            			output[outCnt-1] = 0x30 + my_event->code - 1;
            		}
            		else if (my_event->code == KEY_0){
            			output[outCnt-1] = 0x30;
            		}
            		else{
            			//Give up
            		}
            	}
            }
        }
    }
}

char* kbCapture_return_string(int eventId)
{
	int outCnt = 0;

	char dev[100];
	sprintf(dev, "/dev/input/event%d", eventId);
	int fd=open(dev, O_RDWR);
    if(fd <= 0){
        printf("Can not open keyboard input file\n");
        return NULL;
     }

    //struct input_event *event;
    char buf[128]={0};
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd , &rfds);

    memset((void*)outputFormal, 0, outMaxLen);
    outCnt = 0;
    while (1) {
        int ret = select(fd +1, &rfds, NULL, NULL, NULL);
        if (ret < 0)
            continue;
        if (FD_ISSET(fd , &rfds)) {
           int readn = read(fd , buf, sizeof(struct input_event));
            if (readn <= 0) {
                printf("uart read error %d\n", readn);
                continue;
            }

            struct input_event *my_event=(struct input_event*)buf;
            if ((my_event->type==EV_KEY) && (my_event->value == 1)){
            	outCnt++;
            	if (outCnt >= outMaxLen){
            		memset(outputFormal, 0, outMaxLen);
            		outCnt = 0;
            	}
            	else if (my_event->code == KEY_ENTER){
            		outCnt = 0;
            		close(fd);
            		return outputFormal;
            		//printf("Output Buffer:%s\n", output);
            	}
            	else{
            		if ((my_event->code >= KEY_1) && (my_event->code <= KEY_9)){
            			outputFormal[outCnt-1] = 0x30 + my_event->code - 1;
            		}
            		else if (my_event->code == KEY_0){
            			outputFormal[outCnt-1] = 0x30;
            		}
            		else{
            			//Give up
            		}
            	}
            }
        }
    }
}

int main(){

	//memset((void*)outputFormal, 0, outMaxLen);
    //if (kbCapture(15, outputFormal) == 1)
    printf("Output Buffer:%s\n", kbCapture_return_string(15));
    return 0;
}







//虚拟键盘的完整翻译
//#include<stdio.h>
//#include<string.h>
//#include<stdlib.h>
//char *str[104]={
//"ESC",
//"F1",
//"F2",
//"F3",
//"F4",
//"F5",
//"F6",
//"F7",
//"F8",
//"F9",
//"F10",
//"F11",
//"F12",
//"` ~",
//"1 !",
//"2 @",
//"3 #",
//"4 $",
//"5 %",
//"6 ^",
//"7 &",
//"8 *",
//"9 (",
//"0 )",
//"- _",
//"+ =",
//"Backspace(退格键)",
//"Tab",
//"q Q",
//"w W",
//"e E",
//"r R",
//"t T",
//"Y y",
//"u U",
//"i I",
//"o O",
//"p P",
//"[ {",
//"] }",
//"Enter(回车键)",
//"CapsLock",
//"a A",
//"s S",
//"d D",
//"f F",
//"g G",
//"h H",
//"j J",
//"k K",
//"l L",
//"; :",
//"' \"",
//"\\ |",
//"Left_shift(左上档键)",
//"z Z",
//"x X",
//"c C",
//"v V",
//"b B",
//"n N",
//"m M",
//", <",
//". >",
//"/ ?",
//"Right_shift(右上档键)",
//"Left_ctrl(左Ctrl键)",
//"左Super键(windows键)",
//"Left_alt(左alt键)",
//"space(空格键)",
//"Right_alt(右alt键)",
//"右Super键(windows键)",
//"菜单键",
//"Right_ctrl(右Ctrl键)",
//"Print Screen键",
//"ScrollLock键",
//"PauseBreak键",
//"Insert",
//"Home",
//"PageUp",
//"Delete",
//"End",
//"PageDown",
//"方向键上",
//"方向键下",
//"方向键左",
//"方向键右",
//"NumLock",
//"/　(小键盘)",
//"*　(小键盘)",
//"-　(小键盘)",
//"+　(小键盘)",
//"Enter(小键盘)",
//". (小键盘)",
//"0 (小键盘)",
//"1 (小键盘)",
//"2 (小键盘)",
//"3 (小键盘)",
//"4 (小键盘)",
//"5 (小键盘)",
//"6 (小键盘)",
//"7 (小键盘)",
//"8(小键盘)",
//"9(小键盘)"
//};
//int num[] = {
//1,
//59,
//60,
//61,
//62,
//63,
//64,
//65,
//66,
//67,
//68,
//87,
//88,
//41,
//2,
//3,
//4,
//5,
//6,
//7,
//8,
//9,
//10,
//11,
//12,
//13,
//14,
//15,
//16,
//17,
//18,
//19,
//20,
//21,
//22,
//23,
//24,
//25,
//26,
//27,
//28,
//58,
//30,
//31,
//32,
//33,
//34,
//35,
//36,
//37,
//38,
//39,
//40,
//43,
//42,
//44,
//45,
//46,
//47,
//48,
//49,
//50,
//51,
//52,
//53,
//54,
//29,
//125,
//56,
//57,
//100,
//126,
//127,
//97,
//99,
//70,
//119,
//110,
//102,
//104,
//111,
//107,
//109,
//103,
//108,
//105,
//106,
//69,
//98,
//55,
//74,
//78,
//96,
//83,
//82,
//79,
//80,
//81,
//75,
//76,
//77,
//71,
//72,
//73,
//};
//int main()
//{
//	int i,j;
//	int code, value;
//	char *down = "按下";
//	char *up = "松开";
//	FILE *fp = NULL;
//	if((fp=fopen("key_record.txt","r"))==NULL)
//    {
//        printf("The file key_record.txt can not be opened!\n");
//        return 1;
//    }
//    while(!feof(fp))
//	{
//		fscanf(fp,"%d %d\n",&code,&value);
//		for(i=0;i<(sizeof(num)/sizeof(int));i++)
//		{
//			if(num[i]==code)
//			break;
//		}
//		if(value == 1)
//		fprintf(stdout,"%s %s\n",str[i],down);
//		//else
//		//fprintf(stdout,"%s %s\n",str[i,up]);
//	}
//	fclose(fp);
//}
