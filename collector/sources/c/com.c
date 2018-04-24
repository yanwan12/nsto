// com.c
// 21/11/2013
 
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "com.h"
#include "trace.h"
#include "rf_protocol.h"
#include "tool.h"

#define VAL_COM_PORT	"/dev/ttyUSB0"
#define VAL_SPEED	460800 /*B115200*/  //B460800
#define VAL_PARITY	0

static unsigned long 	ulTraceModule = TRC_MODULE_COM;
static int 		nComHandle 	= -1;
static int		nCounter	= 0;

static int 	open_com 		(void);
static int	set_interface_attribs 	(void);

/****************************************************************************/
/*****************************************************************************/
void comInit 
(
	void
)
{
	int n_result;

	n_result = open_com ();

	if (-1 == n_result)
	{
		trcTrace0 (ulTraceModule, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", 1, 1);
		trcTrace0 (ulTraceModule, "!  KEY NOT PRESNT           !", 1, 1);
		trcTrace0 (ulTraceModule, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", 1, 1);

		goto LABEL_EXIT;
	}

	trcTrace1 (ulTraceModule, "COM handle = %d", nComHandle, 1, 1);

LABEL_EXIT:

	return;
}

/*****************************************************************************/
/*****************************************************************************/
int comRead 
(
	char * _pc_buffer,
	int    _n_buffer_len
)
{
	int n_return;

	n_return = read (nComHandle, _pc_buffer, _n_buffer_len);

	if (n_return > 0)
	{
//		trcTrace2 (ulTraceModule, "RX : ", _pc_buffer, n_return, 1, 1);
	}
	else
	{
		if (0x00 == (++nCounter % 10))
		{
//			trcTrace0 (ulTraceModule, ".", 0,0);
		}
	}

	return n_return;
}

/*****************************************************************************/
/*****************************************************************************/
int comWrite 
(
	char * _pc_buffer,
	int    _n_buffer_len
)
{
	int n_return;

	n_return = write (nComHandle, _pc_buffer, _n_buffer_len);

//	trcTrace2 (ulTraceModule, "TX : ", _pc_buffer, _n_buffer_len, 1, 1);

	return n_return;

}

/*****************************************************************************/
/*****************************************************************************/
static int open_com 
(
	void
)
{
	nComHandle = open (VAL_COM_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK /*| O_NDELAY*/);

	set_interface_attribs ();

	return nComHandle;	
}

/*****************************************************************************/
/*****************************************************************************/
static int set_interface_attribs 
(
	void
)
{
        struct termios tty;

        memset (&tty, 0, sizeof tty);

        if (tcgetattr (nComHandle, &tty) != 0)
        {
		trcTrace0 (ulTraceModule, "tcgetattr error", 1, 1);
                return -1;
        }
	
	/* Setting other Port Stuff */
	tty.c_cflag     &=  ~PARENB;        			// Make 8n1
	tty.c_cflag     &=  ~CSTOPB;
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     |=  CS8;
	tty.c_cflag     &=  ~CRTSCTS;       			// no flow control
	tty.c_lflag     =   0;          			// no signaling chars, no echo, no canonical processing
	tty.c_oflag     =   0;                  		// no remapping, no delays
	tty.c_cc[VMIN]  =   LEN_FRAME;          		// read doesn't block
	tty.c_cc[VTIME] =   1;                  		// 0.1 seconds read timeout

	tty.c_cflag     |=  CREAD | CLOCAL;     		// turn on READ & ignore ctrl lines
	tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);		// turn off s/w flow ctrl
	tty.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG); 	// make raw
	tty.c_oflag     &=  ~OPOST;              		// make raw

	/* Flush Port, then applies attributes */
	tcflush (nComHandle, TCIFLUSH );

        if (tcsetattr (nComHandle, TCSANOW, &tty) != 0)
        {
		trcTrace0 (ulTraceModule, "set interface error", 1, 1);
                return -1;
        }

        return 0;
}


// End of file
