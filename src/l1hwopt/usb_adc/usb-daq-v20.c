#ifndef TARGET_LINUX_ARM

#include <stdio.h>
#include <string.h>
#include <usb.h>

typedef unsigned char BYTE;
int OpenUsb(void);
int CloseUsb(void);
int WriteUsb(int dwPipeNum,char *pBuffer,int dwSize);
int ReadUsb(int dwPipeNum,char *pBuffer,int dwSize);
int DO_SOFT(BYTE chan,BYTE state);
BYTE DI_Soft(void);
int AD_single(int chan,float* adResult);
int AD_continu(int chan,int Num_Sample,int Rate_Sample,float *databuf);
int MAD_continu(int chan_first,int chan_last,int Num_Sample,int Rate_Sample,float  *mad_data);

int DA_sigle_out(int chan,int value);
int DA_DATA_SEND(int chan,int Num,int *databuf);
int DA_scan_out(int chan,int Freq,int scan_Num);
int PWM_Out(int chan,int Freq,float DutyCycle,int mod);
int PWM_In(int mod);
int COUNT(int mod);
int Read_PWM_In(float* Freq, int* DutyCycle);
int Read_COUNT(int* count);
usb_dev_handle *dev_h = NULL;  // the device handle
 
int DeviceCount;			 //Éè±žÊýÁ¿
 
int  opened=0;
#define pi 3.1415926
#define MY_VID        0x7812
#define MY_PID        0x55A9
#define TRANSFER_TIMEOUT 6000 /* in msecs */
#define MY_CONFIG 1
#define MY_INTF 0
int verbose = 0;

void print_endpoint(struct usb_endpoint_descriptor *endpoint)
{
  printf("      bEndpointAddress: %02xh\n", endpoint->bEndpointAddress);
  printf("      bmAttributes:     %02xh\n", endpoint->bmAttributes);
  printf("      wMaxPacketSize:   %d\n", endpoint->wMaxPacketSize);
  printf("      bInterval:        %d\n", endpoint->bInterval);
  printf("      bRefresh:         %d\n", endpoint->bRefresh);
  printf("      bSynchAddress:    %d\n", endpoint->bSynchAddress);
}

void print_altsetting(struct usb_interface_descriptor *interface)
{
  int i;

  printf("    bInterfaceNumber:   %d\n", interface->bInterfaceNumber);
  printf("    bAlternateSetting:  %d\n", interface->bAlternateSetting);
  printf("    bNumEndpoints:      %d\n", interface->bNumEndpoints);
  printf("    bInterfaceClass:    %d\n", interface->bInterfaceClass);
  printf("    bInterfaceSubClass: %d\n", interface->bInterfaceSubClass);
  printf("    bInterfaceProtocol: %d\n", interface->bInterfaceProtocol);
  printf("    iInterface:         %d\n", interface->iInterface);

  for (i = 0; i < interface->bNumEndpoints; i++)
    print_endpoint(&interface->endpoint[i]);
}

void print_interface(struct usb_interface *interface)
{
  int i;

  for (i = 0; i < interface->num_altsetting; i++)
    print_altsetting(&interface->altsetting[i]);
}

void print_configuration(struct usb_config_descriptor *config)
{
  int i;

  printf("  wTotalLength:         %d\n", config->wTotalLength);
  printf("  bNumInterfaces:       %d\n", config->bNumInterfaces);
  printf("  bConfigurationValue:  %d\n", config->bConfigurationValue);
  printf("  iConfiguration:       %d\n", config->iConfiguration);
  printf("  bmAttributes:         %02xh\n", config->bmAttributes);
  printf("  MaxPower:             %d\n", config->MaxPower);

  for (i = 0; i < config->bNumInterfaces; i++)
    print_interface(&config->interface[i]);
}

int print_device(struct usb_device *dev, int level)
{
  usb_dev_handle *udev;
  char description[256];
  char string[256];
  int ret, i;

  udev = usb_open(dev);
  if (udev) {
    if (dev->descriptor.iManufacturer) {
      ret = usb_get_string_simple(udev, dev->descriptor.iManufacturer, string, sizeof(string));
      if (ret > 0)
        snprintf(description, sizeof(description), "%s - ", string);
      else
        snprintf(description, sizeof(description), "%04X - ",
                 dev->descriptor.idVendor);
    } else
      snprintf(description, sizeof(description), "%04X - ",
               dev->descriptor.idVendor);

    if (dev->descriptor.iProduct) {
      ret = usb_get_string_simple(udev, dev->descriptor.iProduct, string, sizeof(string));
      if (ret > 0)
        snprintf(description + strlen(description), sizeof(description) -
                 strlen(description), "%s", string);
      else
        snprintf(description + strlen(description), sizeof(description) -
                 strlen(description), "%04X", dev->descriptor.idProduct);
    } else
      snprintf(description + strlen(description), sizeof(description) -
               strlen(description), "%04X", dev->descriptor.idProduct);

  } else
    snprintf(description, sizeof(description), "%04X - %04X",
             dev->descriptor.idVendor, dev->descriptor.idProduct);

  printf("%.*sDev #%d: %s\n", level * 2, "                    ", dev->devnum,
         description);

  if (udev && verbose) {
    if (dev->descriptor.iSerialNumber) {
      ret = usb_get_string_simple(udev, dev->descriptor.iSerialNumber, string, sizeof(string));
      if (ret > 0)
        printf("%.*s  - Serial Number: %s\n", level * 2,
               "                    ", string);
    }
  }

  if (udev)
    usb_close(udev);

  if (verbose) {
    if (!dev->config) {
      printf("  Couldn't retrieve descriptors\n");
      return 0;
    }

    for (i = 0; i < dev->descriptor.bNumConfigurations; i++)
      print_configuration(&dev->config[i]);
  } else {
    for (i = 0; i < dev->num_children; i++)
      print_device(dev->children[i], level + 1);
  }

  return 0;
}
struct usb_device *device[16];

int Reset_Usb_Device(void)
{
  return usb_reset(dev_h);
}
int OpenUsb(void)
{
	if(opened==1)
	{
		CloseUsb();
		opened=0;
	}
	usb_init(); //initialize the library 
	usb_find_busses(); // find all busses 
	usb_find_devices(); // find all connected devices
	struct usb_bus *bus;
	struct usb_device *dev;
	int i=0,j;
	for (bus = usb_busses; bus; bus = bus->next) 
	{
		for (dev = bus->devices; dev; dev = dev->next)
		{
			 
			if (dev->descriptor.idVendor == MY_VID&& dev->descriptor.idProduct == MY_PID)
			{
				device[i]=dev;
				print_device(dev, 0);
				i++;
				if(i>=16)
					break;	
			}
		}
	}
	DeviceCount=i;
	if(i>0)
	{
		for( j=0;j<i;j++)
		{
			dev_h=usb_open(device[j]);
			//opened=1;
                      //  return 0;
		 	if (usb_set_configuration(dev_h, MY_CONFIG) < 0)
			{
				printf("error setting config #%d: %s\n", MY_CONFIG, usb_strerror());
				dev=NULL;
				continue;
			}
                        else
			{
				printf("success: usb_set_configuration #%d\n", MY_CONFIG);
			}

			if (usb_claim_interface(dev_h, MY_INTF) < 0)
			{
				printf("error claiming interface #%d:\n%s\n", MY_INTF, usb_strerror());
				dev=NULL;
				continue;
			}
			else
			{
				 
				printf("success: claim_interface #%d\n", MY_INTF);
				opened=1;
				return 0;
			}
                    
		}
	}
	dev=NULL;
	return -1;

}
int CloseUsb(void)
{
  if(opened==1)
	{
		opened=0;
		if(dev_h==NULL)
		{
			return-1;
		}
		usb_release_interface(dev_h, 0);
		if (dev_h)
		{
			usb_close(dev_h);
		}
		printf("Done.\n");
		return 0;
	}
	return -2;
}
 
int WriteUsb(int dwPipeNum,char *pBuffer,int dwSize)
{
int ret;
//printf("####write start####\n");
  ret = usb_bulk_write(dev_h, (int)dwPipeNum,(char *)pBuffer, dwSize,TRANSFER_TIMEOUT);
  if(ret>0)
	{
//	printf("####write end####\n");
	   return 0;
	}
  else
	  return 1;
	
}
int ReadUsb(int dwPipeNum,char *pBuffer,int dwSize)
{
	int ret;
//printf("####read start####\n");
  ret = usb_bulk_read(dev_h, (int)dwPipeNum,(char *)pBuffer, dwSize,TRANSFER_TIMEOUT);
  if(ret>0)
   {
//	printf("####read end####\n");
	   return 0;
	}
  else
	  return 1;
}

int AD_single(int chan,float* adResult)
{
    if( opened==0)
	return -1;
  BYTE buf[16],inbuf[16];
 // EnterCriticalSection( &g_cs );
 // printf("####AD_single start####\n");
  buf[0]=0;
  buf[1]=0;
 if(WriteUsb(0x1,buf,2))//设置模式 单次采样
 {
    printf("error: failed to set sample mode: single sampele\n");
   	return -1;
 }
  buf[0]=2;
  buf[1]=(BYTE)chan&0x0f;
  buf[2]=0;
 if(WriteUsb(0x1,buf,3)) //设置采样通道
 {
    printf("error: failed to set sample channel: %d\n", chan);
    	return -1;
 }


  buf[0]=1;
  buf[1]=1;
 if(WriteUsb(0x1,buf,2))//启动采集
  {
    printf("error: failed to start sample\n");
   	return -1;
  }
 // printf("####AD_single start to read####\n");
 if(ReadUsb(0x82,inbuf,2)) //读取结果
  {
    printf("error: failed to read single AD value\n");
   return -1;
  }
  float v_adResult;
 
   
   v_adResult=(float)(((unsigned int)inbuf[1]<<8)+inbuf[0]);
   // v_adResult=v_adResult*2.5/4096;
  //  v_adResult=(v_adResult-1.25)/vv+1.25;
   //v_adResult=(v_adResult-0.3)/vv+(0.3-1.25)/(vv+1.25);
  //  v_adResult= (1.25- v_adResult)*8.33333333;
 v_adResult=(float)(v_adResult*3.3/4096);
  *adResult=v_adResult;
   
 // LeaveCriticalSection( &g_cs );
 // printf("####AD_single end####\n");
  return 0;
}
int AD_continu(int chan,int Num_Sample,int Rate_Sample,float  *databuf)
{
    if( opened==0)
	return -1;
//	EnterCriticalSection( &g_cs );
 

	int num=0;
  BYTE buf[16],inbuf[1024];
  buf[0]=0;
  buf[1]=1;
 if(WriteUsb(0x1,buf,2))//设置模式 单次采样
 {
    printf("error: failed to set sample mode: continue sample\n");
   	return -1;
 }
  buf[0]=2;
  buf[1]=(BYTE)chan&0x0f;
  buf[2]=0;
 if(WriteUsb(0x1,buf,3)) //设置采样通道
 {
    printf("error: failed to set sample channel: %d\n", chan);
    	return -1;
 }
 

  buf[0]=3;
  buf[1]=Rate_Sample&0xff;
  buf[2]=(Rate_Sample>>8)&0xff;
  buf[3]=(Rate_Sample>>16)&0xff;
  buf[4]=(Rate_Sample>>24)&0xff;
  if(WriteUsb(0x1,buf,5)) //设置采样频率
  {
     printf("error: failed to set sample rate: %d\n", Rate_Sample);
   	return -1;
  }
  buf[0]=4;
  Num_Sample=Num_Sample-Num_Sample%32;
  if(Num_Sample<0)Num_Sample=0;
  buf[1]=Num_Sample&0xff;
  buf[2]=(Num_Sample>>8)&0xff;
  buf[3]=(Num_Sample>>16)&0xff;
  buf[4]=(Num_Sample>>24)&0xff;
  if(WriteUsb(0x1,buf,5)) //设置采样个数
  {
     printf("error: failed to set sample number: %d\n", Num_Sample);
    	return -1;
  }

#if 0
  buf[0]=1;
  buf[1]=1;
 // 
  if(WriteUsb(0x1,buf,2))//启动采集
  {
    printf("error: failed to start sample\n");
   	return -1;
  }
  
 ReadUsb(0x82,inbuf,2); //
  if(!((inbuf[0]==0x55)&&(inbuf[1]==0xaa)))
  {
   ReadUsb(0x82,inbuf,2);
   if(!((inbuf[0]==0x55)&&(inbuf[1]==0xaa)))
   {
	printf("warn: inbuf[0]=%d inbuf[1]=%d\n", inbuf[0], inbuf[1]);
   }
  }
#else
  // read start flag
  int ret = 0;
  int nTry = 0;
  while ((ret == 0) && (nTry++ < 40))
  {
    // send start capture command
    buf[0]=1;
    buf[1]=1;
    if(WriteUsb(0x1,buf,2))
    {
      printf("error: failed to start sample\n");
   	return -1;
    }

    // receive start flag
    inbuf[0]=inbuf[1]=0;
    ret = ReadUsb(0x82,inbuf,2);
    if((inbuf[0] == 0x55) && (inbuf[1]==0xaa))
      break;
  }
  if(!(inbuf[0]==0x55) && (inbuf[1]==0xaa))
  {
    printf("error: failed to find start flag: ret=%d nTry=%d inbuf[0]=%d inbuf[1]=%d\n",
           ret, nTry, inbuf[0], inbuf[1]);
    return -1;
  }
  else
    printf("success: find start flag: ret=%d nTry=%d\n", ret, nTry);
#endif
  float v_adResult;
  int j,i;
for(  j=0;j<Num_Sample/512;j++)
{
 if(ReadUsb(0x82,inbuf,1024)) //读取结果
  {
     printf("error: failted to read AD result\n");
  	return -1;
  }
  for( i=0;i<512;i++)
  {	 
	   
   v_adResult=(float)(((unsigned int)inbuf[i*2+1]<<8)+inbuf[i*2]);
   v_adResult=(float)(v_adResult*3.3/4096);
   *databuf=v_adResult;
   databuf++;
  }
}
//LeaveCriticalSection( &g_cs );
return 0;
 



}
int MAD_continu(int chan_first,int chan_last,int Num_Sample,int Rate_Sample,float  *mad_data)
{
    if( opened==0)
	return -1;
//	EnterCriticalSection( &g_cs );
  if((chan_last<chan_first)||(chan_first<0)||(chan_last<0)||(chan_first>15)||(chan_last>15))
    return -1;
  int num=0,i,j;
  BYTE buf[16],inbuf[1024];
  buf[0]=0;
  buf[1]=2;
 if(WriteUsb(0x1,buf,2))//设置模式 单次采样
   return -1;
  buf[0]=2;
  buf[1]=(BYTE)chan_first&0x0f;
  buf[2]=(BYTE)chan_last&0x0f;
 if(WriteUsb(0x1,buf,3)) //设置采样通道
   return -1;

  
 

  buf[0]=3;
  buf[1]=Rate_Sample&0xff;
  buf[2]=(Rate_Sample>>8)&0xff;
  buf[3]=(Rate_Sample>>16)&0xff;
  buf[4]=(Rate_Sample>>24)&0xff;
  if(WriteUsb(0x1,buf,5)) //设置采样频率
   return -1;
  buf[0]=4;
  Num_Sample=Num_Sample-Num_Sample%32;
  if(Num_Sample<0)Num_Sample=0;
  buf[1]=Num_Sample&0xff;
  buf[2]=(Num_Sample>>8)&0xff;
  buf[3]=(Num_Sample>>16)&0xff;
  buf[4]=(Num_Sample>>24)&0xff;
  if(WriteUsb(0x1,buf,5)) //设置采样个数
   return -1;

  buf[0]=1;
  buf[1]=1;
 if(WriteUsb(0x1,buf,2))//启动采集
  return -1;
 ReadUsb(0x82,inbuf,2); //
  if(!((inbuf[0]==0x55)&&(inbuf[1]==0xaa)))
  {
   ReadUsb(0x82,inbuf,2);
   if(!((inbuf[0]==0x55)&&(inbuf[1]==0xaa)))
   {

   }
  }
  float v_adResult;
  
for( j=0;j<Num_Sample/32;j++)
{
 if(ReadUsb(0x82,inbuf,64)) //读取结果
   return -1;
  for( i=0;i<32;i++)
  {	 
    v_adResult=(float)(((unsigned int)inbuf[i*2+1]<<8)+inbuf[i*2]);
   v_adResult=(float)(v_adResult*3.3/4096);
   *mad_data=v_adResult ;
   mad_data++;
  }
}
//LeaveCriticalSection( &g_cs );

return 0;
}

int DA_sigle_out(int chan,int value)
{
   if( opened==0)
	return -1;
  BYTE buf[16];
  if(chan==1)
  {
  buf[0]=7;
  }
  else
  {
   buf[0]=8;
  }
  buf[1]=0;//da mod
  buf[2]=0;//da num
  buf[3]=0;

  buf[4]=0;//da freq
  buf[5]=0;
  buf[6]=0;
  buf[7]=0;

  buf[8]= value&0xff;  //da value
  buf[9]=(value>>8)&0xff;
   
  if(WriteUsb(0x1,buf,10)) //设置采样频率
   return -1;

	return 0;
}
void delay(int ii)
{
 
 while(1)
 {
  ii--;
 if(ii<1)break;
 }
}
int DA_DATA_SEND(int chan,int Num,int *databuf)
{
    if( opened==0)
	return -1;
    BYTE buf2[64];
  int aa=0,bb=0;
  int i,j;
  if(Num>512)Num=512;
  aa=Num/30;
  bb=Num%30;
  buf2[0]=32;
  buf2[1]=0;
 /*   for(j=2;j<10;j++)
 {
  buf2[j]=j;
   }
  if(WriteUsb(0x2,buf2,10))  
	   return -1;
 */
  buf2[0]=32;
  buf2[1]=0;
  if(aa>0)
  {
	  for(i=0;i<aa;i++)
	  {
		  if(chan==1)
		  {
	   buf2[2]=(i*30)&0xff;
	   buf2[3]=((i*30)>>8)&0xff;
		  }
		  else
		  {
		buf2[2]=(i*30+512)&0xff;
		buf2[3]=((i*30+512)>>8)&0xff;
		  }
	   for(j=0;j<30;j++)
	   {
		buf2[(j<<1)+4]=(*(databuf+i*30+j))&0xff;
		buf2[(j<<1)+5]=((*(databuf+i*30+j))>>8)&0xff;
	   }
	  if(WriteUsb(0x1,buf2,64)) 
	   return -1;
      // delay(1000000);
	  }
  }
  if(bb>0)
  {
     if(chan==1)
		  {
	   buf2[2]=(aa*30)&0xff;
	   buf2[3]=((aa*30)>>8)&0xff;
		  }
		  else
		  {
		buf2[2]=(aa*30+512)&0xff;
		buf2[3]=((aa*30+512)>>8)&0xff;
		  }
	   for(j=0;j<bb;j++)
	   {
		buf2[(j<<1)+4]=(*(databuf+aa*30+j))&0xff;
		buf2[(j<<1)+5]=((*(databuf+aa*30+j))>>8)&0xff;
	   }
	  if(WriteUsb(0x1,buf2,(bb<<1)+4)) 
	   return -1;
     // delay(1000000);
  }
  
  return 0;
}
int DA_scan_out(int chan,int Freq,int scan_Num)
{
    if( opened==0)
	return -1;
 

  BYTE buf[16];
  if(chan==1)
  {
  buf[0]=7;
  }
  else
  {
   buf[0]=8;
  }
  buf[1]=1;//da mod
  buf[2]=scan_Num&0xff;//da num
  buf[3]=(scan_Num>>8)&0xff;

  buf[4]=Freq&0xff;//da freq
  buf[5]=(Freq>>8)&0xff;
  buf[6]=(Freq>>16)&0xff;
  buf[7]=(Freq>>24)&0xff;

  buf[8]=0;  //da value
  buf[9]=0;
   
  if(WriteUsb(0x1,buf,10))  
   return -1;
	return 0;
}
int PWM_Out(int chan,int Freq,float DutyCycle,int mod)
{
  int DutyCycle_;
  DutyCycle_=DutyCycle*100;
    if( opened==0)
	return -1;
BYTE buf[16];
  if(chan==1)
  {
  buf[0]=9;
  }
  else
  {
   buf[0]=10;
  }
  buf[1]=mod;//pwm mod
  buf[2]=DutyCycle_&0xff;//PWM_Duty-- 二个bit
  buf[3]=(DutyCycle_>>8)&0xff;

  buf[4]=Freq&0xff;//PWM_Freq--四个bit
  buf[5]=(Freq>>8)&0xff;
  buf[6]=(Freq>>16)&0xff;
  buf[7]=(Freq>>24)&0xff;

  if(WriteUsb(0x1,buf,8))  
   return -1;
	return 0;
}
int PWM_In(int mod)
{

    if( opened==0)
	return -1;
 BYTE buf[2];
  buf[0]=11;
  buf[1]=mod;
 if(WriteUsb(0x1,buf,2))// 
   return -1;

	return 0;
}
int COUNT(int mod)
{

    if( opened==0)
	return -1;
  BYTE buf[2];
  buf[0]=12;
  buf[1]=mod;
 if(WriteUsb(0x1,buf,2))// 
   return -1;
	return 0;
}
int Read_PWM_In(float* Freq, int* DutyCycle)
{

    if( opened==0)
	return -1;
	BYTE inbuf[16];
   if(ReadUsb(0x81,inbuf,16)) //读取结果
   return -1;
   if(ReadUsb(0x81,inbuf,16)) //读取结果
   return -1;
   * DutyCycle=inbuf[1];
   * DutyCycle+=(unsigned int)inbuf[2]<<8;
   int freq1;
   freq1=inbuf[3];
   freq1+=(unsigned int)inbuf[4]<<8;
   freq1+=(unsigned int)inbuf[5]<<16;
   freq1+=(unsigned int)inbuf[6]<<24;
   * Freq=(float)freq1/10;
	return 0;
}
int Read_COUNT(int* count)
{

    if( opened==0)
	return -1;
	BYTE inbuf[16];
   if(ReadUsb(0x81,inbuf,16)) //读取结果
   return -1;
   if(ReadUsb(0x81,inbuf,16)) //读取结果
   return -1;
   
   * count=inbuf[7];
   * count+=(unsigned int)inbuf[8]<<8;
   * count+=(unsigned int)inbuf[9]<<16;
   * count+=(unsigned int)inbuf[10]<<24;
	return 0;
}
int DO_SOFT(BYTE chan,BYTE state)
{
   if( opened==0)
	return -1;
  BYTE buf[3];
  buf[0]=13;
  buf[1]=chan;
  buf[2]=state;
 if(WriteUsb(0x1,buf,3))// 
   return -1;
	return 0;
}
BYTE DI_Soft(void)
{
  if( opened==0)
	return -1;
	BYTE inbuf[16];
   if(ReadUsb(0x81,inbuf,16)) //读取结果
   return -1;
   if(ReadUsb(0x81,inbuf,16)) //读取结果
   return -1;
   
   return inbuf[0];
   
 
}

#endif

/*

int main()
{  
 	OpenUsb();

	
     int i;
      printf("####usb  device open ok  ####\n");
      for(i=0;i<8;i++)
	{
	 DO_SOFT(i,1);
	}
        printf("####DusbEMO  device write ok  ####\n");
	unsigned char DI=0;
	DI=DI_Soft();
	 printf("####IOin = %x\n",DI);

	int daResult,m_freq1,m_num1;
	 m_freq1=5000;
	m_num1=512;			
	 daResult=(int)(8*4096/10);
	 int *sine_wave; 
	//double phase = 0.0; 
	sine_wave = (int *)malloc(m_num1*sizeof(int));
	 for(i=0;i<m_num1;i++)
	 {
	// *(sine_wave+i)=(int)(sin(2*pi*i/m_num1)*daResult/2 +daResult/2);
	 }
	 DA_DATA_SEND(1,m_num1,sine_wave);
	DA_scan_out(1,m_freq1,m_num1);
	// DA_sigle_out(1,3048);

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
 
	char str[10];
	float adResult1;
	for(i=0;i<12;i++)
	{
	
      if(AD_single(i,&adResult1)==0)
	{
         printf("read ad %d =%f\n",i,adResult1);
	  
	}
	else
	{
	 printf("read ad %d fail!\n",i);
	}
	//scanf("%s",str);
	} 
    
    CloseUsb();
    

}
*/
