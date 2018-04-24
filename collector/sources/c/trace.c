// trace.c
// 22/11/2013

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "trace.h"
#include "frame.h"

unsigned long ulMaskModule = 0xFFFFFFFF;

unsigned char 	test_module 	(unsigned long _ul_module, unsigned char _uc_cr);
void			print_time		(void);
void            insert_cr		(unsigned char _b_cr);
void 			display_buffer 	(char * _pc_buffer, int _n_buff_len);

/*****************************************************************************/
/*****************************************************************************/
void trcInit 
(
	unsigned long _ul_mask_module
)
{
	ulMaskModule = _ul_mask_module;
}

//*****************************************************************************/
/*****************************************************************************/
void trcTrace0 
(
	unsigned long 	_ul_module, 
	char * 		_pc_trace,
	unsigned char   _b_date, 
	unsigned char 	_b_cr
)
{

	if (0 == test_module(_ul_module, _b_date))
	{
		goto LABEL_EXIT;
	}

	printf (_pc_trace);

	insert_cr (_b_cr);

LABEL_EXIT:

	return;	
}

/*****************************************************************************/
/*****************************************************************************/
void trcTraceFrame 
(
	unsigned long 	_ul_module, 
	pstFrame	_pst_frame,
	unsigned char   _b_date, 
	unsigned char 	_b_cr
)
{
	char 	ac_trace [100];
	int	n_i;
	int	n_max;

	if (0 == test_module(_ul_module, _b_date))
	{
		goto LABEL_EXIT;
	}

	if (_pst_frame->acProtocol[0] != 0x00)
	{
		sprintf 
		(
			ac_trace,
			"TX : %2.2X %2.2X ",
			_pst_frame->acProtocol[0],
			_pst_frame->acProtocol[1]
		);		
	}
	else
	{
		sprintf (ac_trace, "RX :       ");
	}
	
	printf (ac_trace);

	sprintf
	(
		ac_trace,
		"%2.2X %s %s %2.2X %2.2X%2.2X ",
		_pst_frame->cTag,
		_pst_frame->acDestSnStr,
		_pst_frame->acSourceSnStr,
		_pst_frame->cPayloadLen,
		_pst_frame->cPi0,
		_pst_frame->cPi1
	);

	printf (ac_trace);

	n_max = LEN_FRAME_PAYLOAD;

	if (_pst_frame->cPayloadLen < LEN_FRAME_PAYLOAD)
	{
		n_max = _pst_frame->cPayloadLen;
	}

	for (n_i = 0; n_i < n_max; n_i++)
	{
		printf("%2.2X", _pst_frame->acPayload[n_i]);
	}
	
	insert_cr (_b_cr);

LABEL_EXIT:

	return;	
}

/*****************************************************************************/
/*****************************************************************************/
void trcTrace1 
(
	unsigned long 	_ul_module, 
	char * 		_pc_trace, 
	int 		_n_param0, 
	unsigned char 	_b_date,
	unsigned char 	_b_cr
)
{
	
	if (0 == test_module(_ul_module, _b_date))
	{
		goto LABEL_EXIT;
	}

	printf (_pc_trace, _n_param0);

	insert_cr (_b_cr);	

LABEL_EXIT:

	return;
}

/*****************************************************************************/
/*****************************************************************************/
void trcTrace2 
(
	unsigned long 	_ul_module, 
	char * 		_pc_msg, 
	char * 		_pc_buffer,
	int		_n_buff_len, 
	unsigned char   _b_date,
	unsigned char 	_b_cr
)
{
	
	if (0 == test_module(_ul_module, _b_date))
	{
		goto LABEL_EXIT;
	}

	printf (_pc_msg);

	display_buffer (_pc_buffer, _n_buff_len);

	insert_cr (_b_cr);	

LABEL_EXIT:

	return;
}



/*****************************************************************************/
/*****************************************************************************/
void display_buffer 
(
	char * 		_pc_buffer,
	int		_n_buff_len 
)
{
	int i;
	
	for (i = 0; i < _n_buff_len; i++)
	{

		if (0x00 == (i % 0x20))
		{
			printf("\n");
		}

		printf("%2.2X", _pc_buffer[i]);
	}

}


/*****************************************************************************/
/*****************************************************************************/
void insert_cr 
(
	unsigned char _b_cr
)
{		
	if (_b_cr != 0)
	{
		printf("\n");
	}

	fflush (stdout);

}

/*****************************************************************************/
/*****************************************************************************/
void print_time 
(
	void
)
{
	//time_t 	  t  = time (NULL);
	//struct tm tm = *localtime (&t);
	//printf ("%2.2d:%2.2d:%2.2d: ", tm.tm_hour, tm.tm_min, tm.tm_sec);
	
/*
	time_duration td = now.time_of_day();

	const long l_hours      = td.hours();
	const long l_minutes    = td.minutes();
	const long l_seconds    = td.seconds();
	const long l_msecond 	= td.total_milliseconds() - ((hours * 3600 + minutes * 60 + seconds) * 1000);


	printf("%2.2d:%2.2d:%2.2d:%3.3d ", l_hours, l_minutes, l_second, l_mssecond);
*/
  struct timeval tv;
  struct tm* ptm;
  char time_string[40];
  long milliseconds;

  /* Obtain the time of day, and convert it to a tm struct.  */
  gettimeofday (&tv, NULL);
  ptm = localtime (&tv.tv_sec);
  /* Format the date and time, down to a single second.  */
  strftime (time_string, sizeof (time_string), "%H:%M:%S", ptm);
  /* Compute milliseconds from microseconds.  */
  milliseconds = tv.tv_usec / 1000;
  /* Print the formatted time, in seconds, followed by a decimal point
     and the milliseconds.  */
  printf ("%s.%03ld : ", time_string, milliseconds);

}


/*****************************************************************************/
/*****************************************************************************/
unsigned char test_module 
(
	unsigned long _ul_module,
	unsigned char _uc_date
)
{
	int n_return = 0;
 
	if ((_ul_module & ulMaskModule) != 0x00)
	{
		n_return = 1;
	}

	if ((1 == n_return) && (1 == _uc_date))
	{
		print_time();
	}

	return n_return;
}

// End of trace.c 

