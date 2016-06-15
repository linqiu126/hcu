/*
 * spsapi.c
 *
 *  Created on: 2016年6月15日
 *      Author: hitpony
 */

#include "../l1hwopt/spsapi.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

//Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表
/*
** Local static variables
*/
//static unsigned char traceFileName[] = __FILE__;


UINT32 spsapi_SerialPortOpen(UINT8 com_port_to_open, UINT16 *fd)
{
	/* Port Open Result */
	if (NULL == fd)
	{
		HcuErrorPrint("SPSAPI: Invalid fd, *fd is pointing to address NULL.\n");
		return FAILURE;
	}

	if (SPSAPI_COM_PORT_0 == com_port_to_open)
	{
		*fd = open(SPS_COM_PORT_PATH_0, O_RDWR|O_NOCTTY|O_NDELAY);

		if (HCU_INVALID_U16 == *fd)
		{
			HcuErrorPrint("SPSAPI: Can't Open Serial Port COM0\n");
			return FAILURE;
		}
	}
	else if(SPSAPI_COM_PORT_1 == com_port_to_open)
	{
		*fd = open(SPS_COM_PORT_PATH_1, O_RDWR|O_NOCTTY|O_NDELAY);

		if (HCU_INVALID_U16 == *fd)
		{
			HcuErrorPrint("SPSAPI: Can't Open Serial Port COM1\n");
			return FAILURE;
		}
	}
	else if(SPSAPI_COM_PORT_2 == com_port_to_open)
	{
		*fd = open(SPS_COM_PORT_PATH_2, O_RDWR|O_NOCTTY|O_NDELAY);

		if (HCU_INVALID_U16 == *fd)
		{
			HcuErrorPrint("SPSAPI: Can't Open Serial Port COM2\n");
			return FAILURE;
		}
	}

	else if(SPSAPI_COM_PORT_3 == com_port_to_open)
	{
		*fd = open(SPS_COM_PORT_PATH_3, O_RDWR|O_NOCTTY|O_NDELAY);

		if (HCU_INVALID_U16 == *fd)
		{
			HcuErrorPrint("SPSAPI: Can't Open Serial Port COM3\n");
			return FAILURE;
		}
	}

	else if(SPSAPI_COM_PORT_4 == com_port_to_open)
	{
		*fd = open(SPS_COM_PORT_PATH_4, O_RDWR|O_NOCTTY|O_NDELAY);

		if (HCU_INVALID_U16 == *fd)
		{
			HcuErrorPrint("SPSAPI: Can't Open Serial Port COM4\n");
			return FAILURE;
		}
	}
	else
	{
		HcuErrorPrint("SPSAPI: Input COM port ID = %d, should be between 0 and 4.\n", com_port_to_open);
		return FAILURE;
	}

	if(fcntl(*fd, F_SETFL, 0) < 0)
	{
		HcuErrorPrint("SPSAPI: fcntl failed!\n");
		return FAILURE;
	}

/*
	//non_blocking mode
	if(fcntl(*fd, F_SETFL, FNDELAY) < 0)
	{
		HcuErrorPrint("SPS485: fcntl failed!\n");
		return FAILURE;
	}
*/

	/*
	if(0 == isatty(STDIN_FILENO))
	{
		HcuErrorPrint("SPSAPI: Standard input is not a terminal device\n");
		return FAILURE;
	}
	*/

	/* Return the result */
	HcuDebugPrint("SPSAPI: COM %d has been openned\n", com_port_to_open);
	return SUCCESS;
}

UINT32 spsapi_SerialPortSet(SerialPortCom_t *sp)
{
	/* Local variable */
	struct termios newtio, oldtio;
	UINT32 cflag, iflag, oflag, lflag;

	if ( tcgetattr(sp->fd, &oldtio)  !=  SUCCESS)
	{
		HcuErrorPrint("SPSAPI: Com port control fd is not valid\n");
		return FAILURE;
	}

	/* Set all bit to Zero */
	bzero( &newtio, sizeof( newtio ) );

	newtio.c_cflag  |=  CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = sp->c_lflag;

	switch( sp->nBits )
	{
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |= CS8;
		break;
	default:
		HcuErrorPrint("SPSAPI: Invalid bit lenth set for com port, nBit should be 7 or 8, error return\n");
		return FAILURE;
	}

	switch( sp->nEvent )
	{
	case 'O':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E':
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N':
		newtio.c_cflag &= ~PARENB;
		break;
	default:
		HcuErrorPrint("SPSAPI: Invalid parity for com port, nEvent should be '0', 'E', or 'N', error return\n");
		return FAILURE;
	}

	switch( sp->nSpeed )
	{
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
		break;
	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	case 19200:
		cfsetispeed(&newtio, B19200);
		cfsetospeed(&newtio, B19200);
		break;
	case 38400:
		cfsetispeed(&newtio, B38400);
		cfsetospeed(&newtio, B38400);
		break;
	case 57600:
		cfsetispeed(&newtio, B57600);
		cfsetospeed(&newtio, B57600);
		break;
	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	case 230400:
		cfsetispeed(&newtio, B230400);
		cfsetospeed(&newtio, B230400);
		break;
	default:
		HcuErrorPrint("SPSAPI: Invalid speed set for com port, Error return\n");
		return FAILURE;
		break;
	}

	if( 1 == sp->nStop)
		newtio.c_cflag &=  ~CSTOPB;
	else if ( 2 == sp->nStop)
		newtio.c_cflag |=  CSTOPB;
	else
	{
		HcuErrorPrint("SPSAPI: Invalid stop bit for com port, Error return\n");
		return FAILURE;
	}

	/* default VTIME and VMIN */
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;  /* If there is one in the buffer, it will be return from block read, */

	/* Clean the buffer !!! before !!! the parameter active */
	tcflush(sp->fd, TCIFLUSH);

	/* Set parameters */
	if((tcsetattr(sp->fd,TCSANOW, &newtio))!=0)
	{
		HcuErrorPrint("SPS485: COM port set error\n");
		return FAILURE;
	}

	/* Save VTIME and VMIN */
	sp->vTime = 0;
	sp->vMin = 1;

	/* Read out the config paramter for debug */
	cflag = (UINT32)newtio.c_cflag;
	iflag = (UINT32)newtio.c_iflag;
	oflag = (UINT32)newtio.c_oflag;
	lflag = (UINT32)newtio.c_lflag;

	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("SPSAPI: COM port flags: c_cflag = 0x%X, c_iflag = 0x%X, c_oflag = 0x%X, c_lflag = 0x%X, VTIME = 0x%d, TMIN = 0x%d\n", cflag, iflag, oflag, lflag, newtio.c_cc[VTIME], newtio.c_cc[VMIN] = 1);
		HcuDebugPrint("SPSAPI: COM port set done!\n");
	}

	return SUCCESS;
}

UINT32 spsapi_SerialPortClose(UINT16 fd)
{
	/* Local variable */
	struct termios oldtio;

	if ( tcgetattr(fd, &oldtio)  !=  SUCCESS)
	{
		HcuErrorPrint("SPSAPI: Com port control fd is not valid\n");
		return FAILURE;
	}

	/* Close the PORT */
	close(fd);
	return SUCCESS;
}

UINT32 spsapi_SerialPortSetVtimeVmin(SerialPortCom_t *sp, UINT8 vTime, UINT8 vMin)
{
	/* Local variable */
	struct termios oldtio;
	UINT8 fd = sp->fd;

	if ( tcgetattr(fd, &oldtio) !=  SUCCESS )
	{
		HcuErrorPrint("SPSAPI: Com port control fd is not valid\n");
		return FAILURE;
	}

	/* Set VTIME and VMIN */
	oldtio.c_cc[VTIME]  = vTime;
	oldtio.c_cc[VMIN] = vMin;  /* If there is one in the buffer, it will be return from block read, */

	/* Clean the buffer !!! before !!! the parameter active */
	tcflush(fd, TCIFLUSH);

	/* Set parameters */
	if((tcsetattr(fd, TCSANOW, &oldtio))!=0)
	{
		HcuErrorPrint("SPSAPI: COM port set error\n");
		return FAILURE;
	}

	/* Save VTIME and VMIN */
	sp->vTime = vTime;
	sp->vMin = vMin;

	return SUCCESS;
}


UINT32 hcu_spsapi_serial_init(SerialPortCom_t *sp)
{
	UINT32 ret = SUCCESS;

	//HcuDebugPrint("SPSAPI: InitSerialPort Start ...\n");
	ret = spsapi_SerialPortOpen(sp->id, (UINT16 *)(&(sp->fd)));
	if (FAILURE == ret)
	{
		HcuErrorPrint("SPSAPI: InitSerialPort serial port COM%d open failure.\n", sp->id);
		return ret;
	}
	HcuDebugPrint("SPSAPI: InitSerialPort serial port COM%d open successfully.\n", sp->id);

	ret = spsapi_SerialPortSet(sp);
	if (FAILURE == ret)
	{
		HcuErrorPrint("SPSAPI: Open Serial Port %d failure.\n", sp->id);
		return ret;
	}
	//HcuDebugPrint("SPSAPI: InitSerialPort serial port COM%d properties has been set successfully.\n", sp->id);
	//HcuDebugPrint("SPSAPI: InitSerialPort completed.\n");
	return ret;
}

UINT32 hcu_spsapi_serial_port_get(SerialPortCom_t *sp, UINT8 *rcv_buf, UINT32 Len)
{
	UINT16 fd = sp->fd;
	UINT32 ret = SUCCESS;

	struct timeval tv;
	tv.tv_sec = SPSAPI_TIMER_FOR_SERIAL_PORT_READ;
	tv.tv_usec = 0;
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	if(select(1+fd, &rfds, NULL, NULL, &tv) > 0)
	{
		if(FD_ISSET(fd, &rfds))
		{
			ret = read(fd, rcv_buf, Len);
			//HcuDebugPrint("SPSAPI: Len of read %d\n", ret);
			if (FAILURE == ret)
		   {
				HcuErrorPrint("SPSAPI: Read Data failure \n");
				return FAILURE;
			}
			else if(!ret)
			{
				 HcuErrorPrint("SPSAPI: Read return 0 byte \n");
				 return FAILURE;
			 }

			return ret;
		}

		else
		{
			HcuErrorPrint("SPSAPI: Read Data failure \n");
			return  FAILURE;
		}
	}

	else
	{
		HcuErrorPrint("SPSAPI: Read Data failure \n");
		return  FAILURE;

	}
}

UINT32 hcu_spsapi_serial_port_send(SerialPortCom_t *sp, UINT8 *send_buf, UINT32 Len)
{
     UINT16  fd = sp->fd;
     UINT32 ret = SUCCESS;

     ret = write(fd, send_buf, Len);
     if (FAILURE == ret)
     {
         HcuErrorPrint("SPSAPI: Write device error\n");
         return FAILURE;
     }
     return ret;
}
