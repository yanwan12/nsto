// tool.c
// 22/11/2013


#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "tool.h"
#include "trace.h"
#include "frame.h"

#define VAL_STR_END_FILE_NAME  "end"

static unsigned long 	ulModuleTrace 	= TRC_MODULE_TOOL;
static char 		cCrc		= 0;
static pthread_mutex_t  MutexEndAppli;

/*****************************************************************************/
/****************************************************************************/
void toolInit
(
	void
)
{
	trcTrace0(ulModuleTrace, "toolInit", 1, 1);

	pthread_mutex_init (&MutexEndAppli, NULL);

	remove (VAL_STR_END_FILE_NAME);
}


/*****************************************************************************/
/****************************************************************************/
void toolEnd
(
	void
)
{
	trcTrace0 (ulModuleTrace, "toolEnd", 1, 1);
}

/*****************************************************************************/
/****************************************************************************/
void toolCrcInit
(
	void
)
{
	cCrc = 0x5A;
}


/*****************************************************************************/
/****************************************************************************/
void toolCrcUpdate
(
	char * 	_pc_buffer,
	int	_n_buffer_len
)
{
	int  n_i;

	for (n_i = 0; n_i < _n_buffer_len; n_i++)
	{
		//trcTrace1 (ulModuleTrace, "data=%2.2X", _pc_buffer[n_i], 1, 0);
		cCrc = cCrc ^_pc_buffer[n_i];
		//trcTrace1 (ulModuleTrace, " CRC=%2.2X", cCrc, 0, 1);
	}
}

/*****************************************************************************/
/****************************************************************************/
char toolCrcGet
(
	void
)
{
	return cCrc;
}



/*****************************************************************************/
/****************************************************************************/
void toolGetSerialNb 	
(
	char * _pc_buffer, 
	char * _pc_serial_nb
)
{
	memset (_pc_serial_nb, 0x00, LEN_FRAME_SERIAL_NB + 1);

	sprintf 
	(
		_pc_serial_nb, 
		"%c%c%c%2.2X%2.2X%2.2X",
		_pc_buffer[0],
		_pc_buffer[1],
		_pc_buffer[2],
		_pc_buffer[3],
		_pc_buffer[4],
		_pc_buffer[5]
	);

}

/*****************************************************************************/
/****************************************************************************/
void toolConvertAscii
(
        char *  _pc_hex_buffer,
        int     _n_hex_len,
        char *  _pc_ascii_buffer
)
{
        int i;

        for (i = 0; i < _n_hex_len; i++)
        {
                printf(&_pc_ascii_buffer[i], "c", _pc_hex_buffer[i]);
        }

        _pc_ascii_buffer[i] = 0;
}

//*****************************************************************************/
/*****************************************************************************/
void toolMutexLock
(
	pthread_mutex_t * _p_mutex
)
{
	pthread_mutex_lock (_p_mutex);
}


/*****************************************************************************/
/*****************************************************************************/
void toolMutexUnlock
(
	pthread_mutex_t * _p_mutex
)
{
	pthread_mutex_unlock (_p_mutex);
}


/*****************************************************************************/
/****************************************************************************/
char toolTestEndAppli
(
	void
)
{
	FILE * 	ph_file;
	int	n_return = 0;

	toolMutexLock (&MutexEndAppli);

	ph_file = fopen (VAL_STR_END_FILE_NAME, "r");
	if (ph_file != NULL)
	{
		fclose (ph_file);
		n_return = 1;
	}
	
	toolMutexUnlock (&MutexEndAppli);

	return n_return;
}

/*****************************************************************************/
/****************************************************************************/
void toolEndAppli
(
	void
)
{
        FILE *  ph_file;

        toolMutexLock (&MutexEndAppli);

	ph_file = fopen (VAL_STR_END_FILE_NAME, "w+");
	if (ph_file != NULL)
	{
		fclose (ph_file);
	}

	toolMutexUnlock (&MutexEndAppli);

}

/*****************************************************************************/
/*****************************************************************************/
int toolCopyFile
(
	char * _pc_source,
	char * _pc_destination	
)
{
        FILE * 	ph_in;
	FILE *  ph_out;
	size_t 	n_chars;
        char 	ac_buffer [1000];
	int	n_return = 0;

	trcTrace0 (ulModuleTrace, "toolCopyFile", 1, 1);
	trcTrace0 (ulModuleTrace, _pc_source, 1, 1);
	trcTrace0 (ulModuleTrace, _pc_destination, 1, 1);

	
	if (NULL == (ph_in = fopen(_pc_source, "rb")))
	{
		trcTrace0 (ulModuleTrace, "No file to copy", 1, 1);
		n_return = 1;
	        goto LABEL_EXIT;
	}
	remove (_pc_destination);
	if (NULL == (ph_out = fopen(_pc_destination, "wb")))
	{									
		trcTrace0 (ulModuleTrace, "tooCopyFile create dest file error", 1, 1);
		goto LABEL_EXIT;
	}

        while ((n_chars = fread(ac_buffer, 1, sizeof (ac_buffer), ph_in)) > 0)
        {
		trcTrace1 (ulModuleTrace, "Read %d data", n_chars, 1, 1);

	        if (fwrite(ac_buffer, 1, n_chars, ph_out) != n_chars)
	        {
			trcTrace0 (ulModuleTrace, "toolCopyFile write error", 1, 1);
			goto LABEL_EXIT;
	        }
		trcTrace1 (ulModuleTrace, "Write %d data", n_chars, 1, 1);
	}
	
	trcTrace0 (ulModuleTrace, "End of write", 1, 1);

	if ((fclose (ph_in) != 0) || (fclose (ph_out) != 0))
	{
		trcTrace0 (ulModuleTrace, "toolCopyFile close files error", 1, 1);
		goto LABEL_EXIT;
	}
	
	n_return = 1;

LABEL_EXIT:

	return n_return;
}	

/*****************************************************************************/
/*****************************************************************************/
void toolGetDateTime
(
	char * _pc_buffer
)
{
	time_t      n_t  	= time (NULL);
	struct tm * st_now 	= localtime (&n_t);

	_pc_buffer [0] = toolHexToBcd (st_now->tm_sec);
	_pc_buffer [1] = toolHexToBcd (st_now->tm_min);
	_pc_buffer [2] = toolHexToBcd (st_now->tm_hour);
	_pc_buffer [3] = 0x00;
	_pc_buffer [4] = toolHexToBcd (st_now->tm_mday);
	_pc_buffer [5] = toolHexToBcd (st_now->tm_mon + 1) | 0x80;
	_pc_buffer [6] = toolHexToBcd ((st_now->tm_year + 1900) - 2000);
/*
	trcTrace1 (ulModuleTrace, "sec  = %d", st_now->tm_sec, 1, 1);
	trcTrace1 (ulModuleTrace, "min  = %d", st_now->tm_min, 1, 1);
	trcTrace1 (ulModuleTrace, "hour = %d", st_now->tm_hour, 1, 1);
	trcTrace1 (ulModuleTrace, "day  = %d", st_now->tm_mday, 1, 1);	
	trcTrace1 (ulModuleTrace, "mon  = %d", st_now->tm_mon, 1, 1);
	trcTrace1 (ulModuleTrace, "year = %d", st_now->tm_year, 1, 1);

	trcTrace2 (ulModuleTrace, "Date ", _pc_buffer, 7, 1, 1);
*/	
}

/*****************************************************************************/
/*****************************************************************************/
char toolHexToBcd
(
	char _c_value 
)
{
	char c_return = 0;
/*	

	if (0x00 == _c_value)
	{
		goto LABEL_EXIT;
	}

	c_return = (toolHexToBcd (_c_value >> 4) * 10) + (_c_value & 0x0F);

LABEL_EXIT:
*/
	c_return = ((_c_value / 10) << 4)  +  (_c_value % 10);

	return c_return;
}


/*****************************************************************************/
/*****************************************************************************/
void toolCopyDescBuffer
(
	char * _pc_source,
	char * _pc_destination
)
{
	unsigned int un_len;
	unsigned int un_offset;

	un_len = (unsigned char)_pc_source[0] * 0x100 +  _pc_source[1];
	for (un_offset = 0; un_offset < un_len; un_offset++)
	{
		_pc_destination[un_offset] = _pc_source[un_offset + 2];
	}
}

/*****************************************************************************/
/*****************************************************************************/
void toolCopySpecialDescBuffer
(
        char * _pc_source,
        char * _pc_destination
)
{
	memcpy (&_pc_destination[0x00], &_pc_source[0x32], 0x0E);
	memcpy (&_pc_destination[0x0E], &_pc_source[0x20], 0x10);
	memcpy (&_pc_destination[0x1E], &_pc_source[0x10], 0x10);
	memcpy (&_pc_destination[0x2E], &_pc_source[0x00], 0x10);
}

/*****************************************************************************/
/*****************************************************************************/
void toolGetTimeS
(
        char * _pc_input_time,
        int  * _pn_return_time_s
)
{
	int n_period_s = 0;
	int n_unit     = _pc_input_time[0] + 1;

	switch (_pc_input_time[1])
	{
		case 0x04:
			n_period_s = 1;
			break;
		case 0x05:
			n_period_s = 60;
			break;
		default :
			break;
	}

	*_pn_return_time_s = n_period_s * n_unit;
}


// End of tool.c

