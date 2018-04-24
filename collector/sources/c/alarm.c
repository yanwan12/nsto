// alarm.c
// 02/12/2013


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "alarm.h"
#include "rf_protocol.h"
#include "trace.h"
#include "path.h"
#include "tool.h"
#include "frame.h"

/******************************************************************************
 * DEFINES
 *****************************************************************************/

#define OFFSET_ALARM			(0x00)
#define OFFSET_ALARM_TYPE		(OFFSET_ALARM		+ 0x00)
#define OFFSET_ALARM_DATE		(OFFSET_ALARM_TYPE 	+ LEN_ALARM_TYPE)
#define OFFSET_ALARM_LOGGER_SN		(OFFSET_ALARM_DATE 	+ LEN_ALARM_DATE)
#define OFFSET_ALARM_FRAME		(OFFSET_ALARM_LOGGER_SN + LEN_ALARM_LOGGER_SN)

/******************************************************************************
 * VARIABLES
 *****************************************************************************/

static int nAlarmsNb	= 0x00;

static pthread_mutex_t	MutexAlarm;

static FILE *		hFile;

static unsigned long	ulModuleTrace = TRC_MODULE_ALARM;

/******************************************************************************
 * MACROS
 *****************************************************************************/

/******************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/

/*****************************************************************************
 * PROGRAM
 *****************************************************************************/


/*****************************************************************************
*****************************************************************************/
void arlInit (void)
{
	trcTrace0 (ulModuleTrace, "arlInit", 1, 1);

	nAlarmsNb 	= 0x00;

        toolCopyFile (VAL_PATH_ALARM_SAVE, VAL_PATH_ALARM_TMP);

	hFile = fopen (VAL_PATH_ALARM_TMP, "ab");
	if (hFile != NULL)
	{
		nAlarmsNb = ftell(hFile) / LEN_ALARM;
	}

	pthread_mutex_init (&MutexAlarm, NULL);
}


/*****************************************************************************
*****************************************************************************/
void arlEnd (void)
{ 
	int n_result;

        trcTrace0 (ulModuleTrace, "arlEnd", 1, 1);
        
	toolMutexLock (&MutexAlarm);
        
	n_result = toolCopyFile (VAL_PATH_ALARM_TMP, VAL_PATH_ALARM_SAVE);
        if (0 == n_result)
        {
		trcTrace0 (ulModuleTrace, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", 1, 1);
		trcTrace0 (ulModuleTrace, "!  ALARM COPY FILE ERROR  !", 1, 1);
		trcTrace0 (ulModuleTrace, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", 1, 1);
	}

	toolMutexUnlock (&MutexAlarm);


}

/*****************************************************************************
* 
*****************************************************************************/
void arlNewAlarm (
	EN_ALARM _en_alarm,
	pstFrame _pst_frame
)
{
	char * pc_alarm;

	trcTrace1 (ulModuleTrace, "New alarm : %d", _en_alarm, 1, 1);
	toolMutexLock (&MutexAlarm);
	 
	pc_alarm = malloc (LEN_ALARM);
	if (NULL == pc_alarm)
	{
		goto LABEL_EXIT;
	}

	pc_alarm [OFFSET_ALARM_TYPE + 0] = (char) (_en_alarm >> 8);
	pc_alarm [OFFSET_ALARM_TYPE + 1] = (char) _en_alarm;
	toolGetDateTime (&pc_alarm [OFFSET_ALARM_DATE]);
	memcpy (&pc_alarm [OFFSET_ALARM_LOGGER_SN], _pst_frame->acSourceSnHex, LEN_ALARM_LOGGER_SN);
	memcpy (&pc_alarm [OFFSET_ALARM_FRAME],     _pst_frame->acHexData, LEN_ALARM_FRAME);
	
	hFile = fopen (VAL_PATH_ALARM_TMP, "ab");
	if (hFile != NULL)
	{
		fwrite (pc_alarm, LEN_ALARM, 1, hFile);
		fclose (hFile);
		nAlarmsNb++;
	}
	toolMutexUnlock (&MutexAlarm);

LABEL_EXIT:

	return;
}

/*****************************************************************************
*****************************************************************************/
int arlGetAlarms (
	char * 	_pc_buffer,
	int  *	_pn_buffer_len
)
{

	int n_read_nb = 0;

	toolMutexLock (&MutexAlarm);
	
	hFile = fopen (VAL_PATH_ALARM_TMP, "rb");
	if (hFile != NULL)
	{
		n_read_nb = fread (_pc_buffer, *_pn_buffer_len, 1, hFile);
		fclose (hFile);
		remove (VAL_PATH_ALARM_TMP);
		nAlarmsNb = 0;
	}
	
	toolMutexUnlock (&MutexAlarm);
	
	return n_read_nb;
}


/*****************************************************************************
*****************************************************************************/
int arlGetAlarmsNb (
	void
)
{
	int n_return;

        toolMutexLock (&MutexAlarm);
	
	n_return = nAlarmsNb;

	toolMutexUnlock (&MutexAlarm);

	return n_return;
}

#if 0
/*****************************************************************************
*****************************************************************************/
void arlClearsAllAlarms (
	unsigned int _un_alarm_nb
)
{
	unAlarmsOffset += _un_alarm_nb;
	
	if (unTotalAlarmsNb == unAlarmsOffset)
	{
		unTotalAlarmsNb	= 0x00;
		unAlarmsOffset 	= 0x00;
	}
	
	if (unTotalAlarmsNb < unAlarmsOffset)
	{
		unAlarmsOffset = unTotalAlarmsNb;	
	}
}
#endif
 
// End of alarm.c

