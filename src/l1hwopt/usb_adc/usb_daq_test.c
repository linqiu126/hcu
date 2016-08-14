#ifdef TARGET_RASPBERRY_PI3B

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "../usb_adc/usb_dll_v20.h"

#define pi 3.1415926
#define DA_FULLSCALE 4096
#define DA_FLOAT2INT(x) (x*4096/3.3)

//#define DIGITAL_TEST
//#define ADCONTINUE_TEST 
#define ADSINGLE_TEST 

/* average filter: average by 'times' for 'channel' and save the AD sample result in 'out' */
int AD_average(int channel, int times, float *out)
{
  float outValue = 0, adResult1;
  int nRead = 0, i;
  for(i=0;i<times;i++)
  {
    if(AD_single(channel,&adResult1)==0)
    {
      outValue += adResult1;
      nRead ++;
    }
    else
    {
      printf("read ad %d fail!\n",i);
    }
  } 

  if (nRead > 0)
  {
     outValue = outValue/nRead;
     *out = outValue;
  }

  return nRead;
}

int usb_test_main(int argc, char* argv[])
{  
      int i;  
     if(-1==OpenUsb())
	{
	  printf("####usb  device open fail ####\n");
	 return -1;
	}
	else
	{
       printf("####usb  device open ok  ####\n");
	}
      /* puhuix: reset usb to avoid usb read error, why? */
      Reset_Usb_Device();

#ifdef DIGITAL_TEST
      for(i=0;i<8;i++)
	{
	 DO_SOFT(i,1);
	}
        printf("####DusbEMO  device write Dout put 1  ####\n");
	unsigned char DI=0;
	DI=DI_Soft();
	 printf("####Din = %x\n",DI);
#endif

#ifdef ADCONTINUE_TEST 
	int daResult,m_freq1,m_num1;
	 m_freq1=5000;
	m_num1=512;			
	 daResult=(int)(2*4096/3.3);
	 int *sine_wave; 
	//double phase = 0.0; 
	sine_wave = (int *)malloc(m_num1*sizeof(int));
	 for(i=0;i<m_num1;i++)
	 {
	  *(sine_wave+i)=(int)(sin(2*pi*i/m_num1)*daResult/2 +daResult/2);
	 }
	 DA_DATA_SEND(1,m_num1,sine_wave);
	DA_scan_out(1,m_freq1,m_num1);
       printf("####DusbEMO  device DA1 output sin wave  ####\n");

	float ad[1024];
	if(AD_continu(1,1024,10000,ad)==0)
	{
	for(i=0;i<50;i++)
	{
	  printf("ad[%d]=%f\n",i*20,ad[i*20]);
	}

	}
	else
	{
	 printf("read ad fail!\n");
	}
#endif
 
#ifdef ADSINGLE_TEST 
        float daFloatValue = 0;
        int daIntValue  = 0;
	float adResult1;

        printf("\n####DA1 output is connected to AD2 input  ####\n");
        for(i=0; i<10; i++)
        {
          daFloatValue = 0.3*i;
          daIntValue  = DA_FLOAT2INT(daFloatValue);
          DA_sigle_out(1, daIntValue);
          AD_average(2, 3, &adResult1);
          printf("####DA1 output %fV, AD2 input value: %f\n", daFloatValue, adResult1);
        }
        
        float sensorValue = 0;
        printf("\n####AD0 input is connected to MQ3 sensor ####\n");
        AD_average(0, 5, &sensorValue);
        printf("####MQ3 sensor input is %f V\n", sensorValue);

        DA_sigle_out(1, 0);
#endif
    
    CloseUsb();
    
 return 0;
}

#endif

