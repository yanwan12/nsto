// alarm.h
// 02/12/2013

#ifndef _ALARMS_H_  
#define  _ALARMS_H_

//#include "newsteo.h"
//#include "typedef.h"
#include "frame.h"

/******************************************************************************
 * DEFINES
 *****************************************************************************/

typedef enum
{
	EN_ALARM_LOGGER_DETECTED_WITHOUT_ALARM,
	EN_ALARM_LOGGER_DETECTED_WITH_ALARM,
	EN_ALARM_LOGGER_LOST,
	EN_ALARM_LOGGER_NEW_ALARM_LIVE,
	EN_ALARM_LOGGER_NEW_ALARM_RESTITUTION,
	EN_ALARM_LOGGER_END_ALARM_LIVE,
	EN_ALARM_LOGGER_END_ALARM_RESTITUTION,
	EN_ALARM_LOGGER_DATE_TIME,
	EN_ALARM_UNDEF
} EN_ALARM;

#define LEN_ALARM_TYPE                  0x02
#define LEN_ALARM_DATE                  LEN_FRAME_DATE
#define LEN_ALARM_LOGGER_SN             LEN_FRAME_HEX_SERIAL_NB
#define LEN_ALARM_FRAME                 LEN_FRAME

#define LEN_ALARM (LEN_ALARM_TYPE + LEN_ALARM_DATE + LEN_ALARM_LOGGER_SN + LEN_ALARM_FRAME)	

/******************************************************************************
 * VARIABLES
 *****************************************************************************/

/******************************************************************************
 * MACROS
 *****************************************************************************/

/******************************************************************************
 * PUBLIC FUNCTIONS
 *****************************************************************************/

void arlInit (
	void
);

void arlEnd (
	void
);

// void arlSetConfig (
// 	unsigned char _puc_alarm_buff
// );

void arlNewAlarm (
	EN_ALARM _en_alarm,
	pstFrame _pst_frame 
);

int arlGetAlarms (
	char * 	_pc_buffer,
	int  * 	_pn_buffer_le
);

int arlGetAlarmsNb (
	void
);

/*
void arlClearsAllAlarms (
	unsigned int _un_alarm_nb
);
*/
#endif /*_ALARMS_H_*/

// End alarm.h

