/*
 ============================================================================
 Name        : hcu.c
 Author      : ZHANG
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "hcu.h"

//#include "../l0service/configxml.h"
#include "../l0service/mmc.h"
#include "../l0service/sysinfo.h"
#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l0service/mqtt.h"


#include "../l1com/l1comdef.h"
#include "../l1com/hwinv.h"


//Global variables to be shared with VM

//Main Entry, Father process
int main(int argc, char* argv[]) {

	if(argv[1] != 0)
	{
		if (!strcmp(argv[1],"version")){
			printf("%d\n", HCU_CURRENT_SW_DELIVERY);
			return EXIT_SUCCESS;
		}
	}else{
		hcu_vm_main_entry();
	}
	return EXIT_SUCCESS;
}





