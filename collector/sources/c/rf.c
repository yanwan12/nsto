// rf.c
// 22/11/2013

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "rf.h"
#include "com.h"
#include "trace.h"
#include "rf_protocol.h"
#include "frame.h"
#include "tool.h"
#include "descriptor.h"
#include "storage.h"
#include "alarm.h"
#include "server.h"

static unsigned long 	ulTraceModule = TRC_MODULE_THREAD_RF;
static int		unDescAdd;
static int		unDescAddNb;
static int		unMaxDescAdd;
static char		cStateGetDesc 	= 0;
static char		cSendAlarm  	= 1;
static char 		acKeySn [LEN_FRAME_HEX_SERIAL_NB];

static void 	init 			(void);
static void 	end  			(void);
static void 	manage_reception	(pstFrame _pst_frame);
static void 	manage_get_desc		(pstFrame _pst_frame);
static void 	send_frame		(char * _pc_frame, int _n_frame_len);
static pstFrame wait_frame		(void);

/******************************************************************************/
/******************************************************************************/
void * rf_Thread 
(
	void * p_arg
)
{
	void * 		pv_return = NULL;
	char		b_end = 0;
	pstFrame	pst_frame;

	trcTrace0 (ulTraceModule, "Start of rf_Thread", 1, 1);

	if (p_arg != NULL)
	{
		pv_return = (void*)1;
		goto LABEL_EXIT;
	}

	init ();

	do
	{
		pst_frame = wait_frame ();

		if (pst_frame != NULL)
		{
			manage_reception (pst_frame);
		}
		else
		{
			usleep (10000);
		}

		//free (pst_frame);

		b_end = toolTestEndAppli();
	}
	while (!b_end);

	end ();

LABEL_EXIT :

	pthread_exit (pv_return);
}

/******************************************************************************/
/******************************************************************************/
static void init 
(
	void
)
{
	pstFrame pst_frame = NULL;

	comInit ();

	trcTrace0 (ulTraceModule, "Send get desciptor of the key", 1, 1);

	send_frame(acRfpCmdKeyGetDescriptor, sizeof(acRfpCmdKeyGetDescriptor));

	sleep(1);
	
	wait_frame ();

	trcTrace0 (ulTraceModule, "Start key", 1, 1);

	usleep (10000);

	send_frame  (acRfpCmdKeyStart, sizeof(acRfpCmdKeyStart));
	
	sleep(2);
	
	pst_frame = wait_frame ();

	memset (acKeySn, 0xFF, sizeof(acKeySn));

	if (pst_frame != NULL)
	{
		//comSetKeySn (&pst_frame->acPayload[7]);
//		memcpy (acKeySn, pst_frame->acDestSnHex, sizeof (acKeySn));
//		trcTrace2 (ulTraceModule, "KeySn : ", acKeySn, sizeof (acKeySn), 1, 1);
	}

// trcTrace2 (ulTraceModule, "KeySn : ", acKeySn, sizeof (acKeySn), 1, 1);
	trcTrace0 (ulTraceModule, "End of start key", 1, 1);
}

/******************************************************************************/
/******************************************************************************/
static void end 
(
	void
)
{
	trcTrace0(ulTraceModule, "End of thread rf", 1, 1);

	send_frame(acRfpCmdKeyStop, sizeof(acRfpCmdKeyStop));

}

/******************************************************************************/
/******************************************************************************/
static pstFrame wait_frame
(
	void
)
{
	pstFrame 	pst_return 	= NULL;
	int 		n_buffer_len 	= 0;
	int 		n_offset 	= 0;
	char 		ac_buffer [LEN_FRAME * 10];

	do
	{
		n_buffer_len = comRead (ac_buffer, sizeof(ac_buffer));
		if ((-1 == n_buffer_len) || (0x00 == n_buffer_len))
		{
			if (0 == n_buffer_len)
			{
				toolEndAppli();
				trcTrace0(ulTraceModule, "Remove key detection", 1, 1);
			}
			goto LABEL_EXIT;
		}
		
		n_offset = 0;

		do
		{
			pstFrame pst_new_frame = frmConstructor (&ac_buffer[n_offset], 0);
			//trcTraceFrame (ulTraceModule, pst_new_frame, 1, 1);
			//trcTrace1 (ulTraceModule, "key = %X", acKeySn [0], 1, 1);
			//trcTrace1 (ulTraceModule, "sn = %X", pst_new_frame->acDestSnHex[0], 1, 1);

			if ((0xFF == acKeySn [0]) && (pst_new_frame->acDestSnHex[0] != 0xFF))
			{
				memcpy (acKeySn, pst_new_frame->acDestSnHex, LEN_FRAME_HEX_SERIAL_NB);
				trcTrace2 (ulTraceModule, "**************KeySn : ", acKeySn, sizeof (acKeySn), 1, 1);

			}
			if (pst_new_frame->cTag != TAG_ACKNOWLEDGE_FROM_KEY)
			{
				trcTraceFrame (ulTraceModule, pst_new_frame, 1, 1);
				pst_return = pst_new_frame;
				goto LABEL_EXIT;
			}
			n_offset += LEN_FRAME;
			free (pst_new_frame);			
		}
		while (n_offset < n_buffer_len);
	}
	while (1);

LABEL_EXIT:

	return pst_return;
}

/******************************************************************************/
/******************************************************************************/
static void manage_reception
(
	pstFrame _pst_frame
)
{	

	switch (_pst_frame->cTag)
	{
		case TAG_MEASURE:
		case TAG_MEASURE_ACK:
			cStateGetDesc = 0;
			//trcTrace1 (ulTraceModule, "Desc %d", descIsPresent (_pst_frame), 1, 1);
			if (_pst_frame->cPi0 != 0x0F)
			{
				stoAdd (_pst_frame);
			}
			if (0 == descIsPresent(_pst_frame))
			{
				manage_get_desc (_pst_frame);
			}
			else if ((_pst_frame->cPi0 & MASK_FRAME_ALARM) && (1 == cSendAlarm))
			{
				cSendAlarm = 0;
				arlNewAlarm (EN_ALARM_LOGGER_NEW_ALARM_LIVE, _pst_frame);
			}
			break;

		case TAG_ACKNOWLEDGE_FROM_DEVICE:
		case TAG_GET_DESCRIPTOR_VALUES_FOLLOWED:
		case TAG_GET_DESCRIPTOR_VALUES_LAST:
			manage_get_desc (_pst_frame);
			break;
		
		default:
			break;
	}

}

/******************************************************************************/
/******************************************************************************/
static void manage_get_desc
(
	pstFrame _pst_frame
)
{
	char * pc_cmd;
	
	switch(cStateGetDesc)
	{
		case 0:
			//trcTrace0 (ulTraceModule, "Desc start cmd", 1, 1);
			send_frame (acRfpCmdLoggerGetDescStart, sizeof(acRfpCmdLoggerGetDescStart));
			cStateGetDesc++;
			unDescAdd    = 0x1FF0;
			unDescAddNb  = 0;
			unMaxDescAdd = 0x1F00;
			break;

		case 1:
			//trcTrace0 (ulTraceModule, "Desc next cmd", 1, 1);
			if (unDescAdd != 0x1FF0)
			{
				descSave (_pst_frame);
			}
			pc_cmd = acRfpCmdLoggerGetDescNext;
			pc_cmd[0x13] = (char) (unDescAdd >> 8);
			pc_cmd[0x14] = (char) (unDescAdd);

                        if (0x1FC0 == unDescAdd)
                        {
       		                unMaxDescAdd = 6 + _pst_frame->acPayload[10];
                                trcTrace1 (ulTraceModule, "****************DESC MAX ADD = %d", unMaxDescAdd, 1, 1);
	                }

			if (++unDescAddNb >= unMaxDescAdd)
			{
				pc_cmd = acRfpCmdLoggerGetDescLast;
				pc_cmd[0x13] = (char) (unDescAdd >> 8);
				pc_cmd[0x14] = (char) (unDescAdd);
				cStateGetDesc++;
				send_frame (pc_cmd, sizeof(acRfpCmdLoggerGetDescLast));
			}
			else
			{
	                        pc_cmd = acRfpCmdLoggerGetDescNext;
		                pc_cmd[0x13] = (char) (unDescAdd >> 8);		
				pc_cmd[0x14] = (char) (unDescAdd);
				send_frame (pc_cmd, sizeof(acRfpCmdLoggerGetDescNext));
			}

			unDescAdd -= 0x10;
			break;	

		case 2:
			cStateGetDesc = 0;
			descSave (_pst_frame);
			svrSendDescriptor (_pst_frame);
			break;

		default:
			break;
	}


}

/******************************************************************************/
/******************************************************************************/
static void send_frame
(
	char * _pc_frame, 
	int   _n_frame_len
)
{

	memcpy (&_pc_frame[7 + 2], acKeySn, LEN_FRAME_HEX_SERIAL_NB);

	pstFrame pst_new_frame = frmConstructor (_pc_frame, 1);

	trcTraceFrame (ulTraceModule, pst_new_frame, 1, 1);

	comWrite (_pc_frame, _n_frame_len);

}


// End of rf.c

