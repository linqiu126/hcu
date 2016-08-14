#ifdef TARGET_RASPBERRY_PI3B

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

#endif
