// server.c
// 22/11/2012

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h> 
#include <sys/time.h>

#include "server.h"
#include "trace.h"
#include "descriptor.h"
#include "tool.h"
#include "storage.h"
#include "alarm.h"

#define STR_VAL_SERVER_OK	"SERVER_OK"

#define TIME_S_PRESENCE		60

#define MAX_MEASURES_NB		10

static unsigned long 	ulModuleTrace 		= TRC_MODULE_THREAD_SERVER;
static int		nSocketDesc 		= -1;
static int		nConnectionTimeS 	= 60;
static struct timeval	stPresenceTime;
static struct timeval	stCurrentTime;
static struct timeval	stMeasuresTime;
//static char *		pcMessage;
static pthread_mutex_t  MutexServer;

struct ST_MESSAGE
{
	int	nMsg;
	char   	acParam [100];
	char * 	pcNext;
};
typedef struct ST_MESSAGE * pstMsg;

pstMsg pstPreviousMsg = NULL;
pstMsg pstCurrentMsg  = NULL;

static void init 		(void);
static void end  		(void);
static void connection		(void);
static int  send_script		(char * _pc_script, char * _pc_data, int _n_data_len);
static void manage_presence	(void);
static void manage_measures	(void);
static void manage_start	(void);
static void manage_alarms	(void);

/******************************************************************************/
/******************************************************************************/
void * server_Thread 
(
	void * arg
)
{
	void * pv_return = NULL;
	char   b_end;
	
	trcTrace0 (ulModuleTrace, "Start of server thread", 1, 1);

	if (arg != NULL)
	{
		pv_return = (void*)1;
		goto LABEL_EXIT;
	}

	init ();

	manage_start ();
	
	do
	{
		sleep (1);

		gettimeofday (&stCurrentTime, NULL);
		
		manage_presence ();

		manage_measures ();

		manage_alarms ();

		b_end = toolTestEndAppli();

	}
	while (!b_end);

	end ();

LABEL_EXIT :

	trcTrace0 (ulModuleTrace, "End of server thread", 1, 1);

	pthread_exit (pv_return);
}

/*****************************************************************************/
/*****************************************************************************/
static void manage_measures
(
	void
)
{
	char   	c_send_measures = 0;
	int    	n_measures_nb   = 0;
	int    	n_buffer_len	= 0;
	int	n_measures_len	= 0;
	char * 	pc_buffer	= NULL;
	int	n_return;

	n_measures_nb = stoGetMeasuresNb ();

	if (0 == n_measures_nb)
	{
		goto LABEL_EXIT;
	}

	if ((stCurrentTime.tv_sec - stMeasuresTime.tv_sec) > nConnectionTimeS)
	{
		//trcTrace0 (ulModuleTrace, "Send Measures by time", 1, 1);
		gettimeofday (&stMeasuresTime, NULL);
		c_send_measures = 1;
	}

	if (n_measures_nb >= MAX_MEASURES_NB)
	{
		c_send_measures = 1;
		//trcTrace0 (ulModuleTrace, "Send Measures by measures nb", 1, 1);
	}

	if (0 == c_send_measures)
	{
		goto LABEL_EXIT;
	}

	stMeasuresTime = stCurrentTime;

	toolCrcInit ();

	// Size of measure
	n_measures_len = n_measures_nb * LEN_FRAME;
	
	trcTrace1 (ulModuleTrace, "Send %d Measures", n_measures_nb, 1, 1);

	// Size of buffer = size of ASCII measures + CRC(1 byte) + EOF(1 byte) + measure nb(2 bytes) + COl SN (6 bytes)
	n_buffer_len = n_measures_len + 10;

	pc_buffer = malloc (n_buffer_len);

	if (NULL == pc_buffer)
	{
		goto LABEL_EXIT;
	}

	
	memset (pc_buffer, 0x00, n_buffer_len);

	pc_buffer [0] = n_measures_nb >> 8;
	pc_buffer [1] = n_measures_nb;
	
	memcpy (&pc_buffer [2], descGetHexSn(), LEN_FRAME_SERIAL_NB);
	n_measures_nb = stoGetMeasures (&pc_buffer[8], &n_measures_len);

	toolCrcUpdate (pc_buffer, n_measures_len + 8);

	pc_buffer [n_measures_len + 8] = toolCrcGet ();
	
	n_return = send_script ("col_store.php", pc_buffer, n_measures_len + 9);

	if (1 == n_return)
	{
		stoRemoveMeasures (n_measures_nb);
	}
	else
	{
		trcTrace0 (ulModuleTrace, "No remove measures", 1, 1);
	}

LABEL_EXIT:

	if (pc_buffer != NULL)
	{
		free (pc_buffer);
	}

	return;
}


/*****************************************************************************/
/*****************************************************************************/
void svrSendDescriptor 
(
	pstFrame _pst_frame
)
{
	char * 	pc_buffer	= NULL;
	FILE *  h_file		= NULL;
	int	n_file_len      = 0;

	toolCrcInit ();

	trcTrace0 (ulModuleTrace, "Send descriptor :", 1, 0);
	trcTrace0 (ulModuleTrace, _pst_frame->acDescFile, 0, 1);

	h_file = fopen (_pst_frame->acDescFile, "rb");
	if (NULL == h_file)
	{
		goto LABEL_EXIT;
	}
	
	fseek (h_file, 0, SEEK_END);

	n_file_len = ftell (h_file);

	rewind (h_file);

	trcTrace1 (ulModuleTrace, "descriptor length = %d", n_file_len, 1, 1);
	
	pc_buffer = malloc (n_file_len + 2);

	if (NULL == pc_buffer)
	{
		goto LABEL_EXIT;
	}

	
	memset (pc_buffer, 0x00, n_file_len + 2);

	fread (pc_buffer, n_file_len, 1, h_file);

	toolCrcUpdate (pc_buffer, n_file_len);

	pc_buffer [n_file_len] = toolCrcGet ();
	
	send_script ("col_descriptor.php", pc_buffer, n_file_len + 1);

LABEL_EXIT:

	if (h_file != NULL)
	{
		fclose (h_file);
	}

	if (pc_buffer != NULL)
	{
		free (pc_buffer);
	}

	return;
}


/*****************************************************************************/
/*****************************************************************************/
void svrAddMessage
(
	int 	_n_msg_type,
	char * 	_pc_param
)
{
	toolMutexLock (&MutexServer);	
	
	pstMsg pst_new_msg = malloc (sizeof (struct ST_MESSAGE));
	if (NULL ==pst_new_msg)
	{
		goto LABEL_EXIT;
	}

	pst_new_msg->nMsg = _n_msg_type;
	memcpy (pst_new_msg->acParam, _pc_param, sizeof (pst_new_msg->acParam));
	pst_new_msg->pcNext = NULL;

	if (NULL == pstCurrentMsg)
	{
		pstCurrentMsg = pst_new_msg;
	}
	else
	{
		if (pstPreviousMsg != NULL)
		{
			pstPreviousMsg->pcNext = (char*)pst_new_msg;
			pstPreviousMsg = pst_new_msg;
		}
		else
		{
			pstPreviousMsg = pstCurrentMsg;
		}
	}
	if (NULL == pstCurrentMsg)
	{

	}

LABEL_EXIT:

	toolMutexUnlock (&MutexServer);
	
}

/*****************************************************************************/
/*****************************************************************************/
static void manage_alarms
(
	void
)
{
	int    	n_alarms_nb   	= 0;
	int    	n_buffer_len	= 0;
	int	n_alarms_len	= 0;
	char * 	pc_buffer	= NULL;

	n_alarms_nb = arlGetAlarmsNb ();

	if (0 == n_alarms_nb)
	{
		goto LABEL_EXIT;
	}

	toolCrcInit ();

	// Size of measure
	n_alarms_len = n_alarms_nb * LEN_ALARM;
	
	trcTrace1 (ulModuleTrace, "Send %d alarms", n_alarms_nb, 1, 1);

	// Size of buffer = size of ASCII measures + CRC(1 byte) + EOF(1 byte) + measure nb(2 bytes) + COl SN (6 bytes)
	n_buffer_len = n_alarms_len + 10;

	pc_buffer = malloc (n_buffer_len);

	if (NULL == pc_buffer)
	{
		goto LABEL_EXIT;
	}

	
	memset (pc_buffer, 0x00, n_buffer_len);

	pc_buffer [0] = n_alarms_nb >> 8;
	pc_buffer [1] = n_alarms_nb;
	
	memcpy (&pc_buffer [2], descGetHexSn(), LEN_FRAME_SERIAL_NB);
	arlGetAlarms (&pc_buffer[8], &n_alarms_len);

	toolCrcUpdate (pc_buffer, n_alarms_len + 8);

	pc_buffer [n_alarms_len + 8] = toolCrcGet ();
	
	send_script ("col_alarms.php", pc_buffer, n_alarms_len + 9);

LABEL_EXIT:

	if (pc_buffer != NULL)
	{
		free (pc_buffer);
	}

	return;
}



/*****************************************************************************/
/*****************************************************************************/
static void manage_start
(
	void
)
{
	char ac_buffer [9];

	memset (ac_buffer, 0x00, sizeof(ac_buffer));
	
	send_script ("col_start.php", NULL, 0);

}


/*****************************************************************************/
/*****************************************************************************/
static void manage_presence
(
	void
)
{
	if ((stCurrentTime.tv_sec - stPresenceTime.tv_sec) > TIME_S_PRESENCE)
	{
		gettimeofday (&stPresenceTime, NULL);
		send_script ("col_presence.php", NULL, 0);
	}
}

/*****************************************************************************/
/*****************************************************************************/
static void connection 
(
	void
)
{    
   	struct sockaddr_in 	server;
	struct hostent * 	p_he;
	struct in_addr **	ap_addr_list;
	char   			ac_ip[100];

    	nSocketDesc = socket (AF_INET , SOCK_STREAM , 0);
    	if (nSocketDesc == -1)
   	{
        	trcTrace0 (ulModuleTrace, "Could not create socket" ,1, 1);
		goto LABEL_EXIT;
   	}
         
	p_he = gethostbyname (descGetServerAdd());
	if (NULL == p_he)
	{
		trcTrace0 (ulModuleTrace, "gethostbyname error", 1, 1);
		goto LABEL_EXIT;
	}


	ap_addr_list = (struct in_addr **) p_he->h_addr_list;
	if ((NULL == ap_addr_list) || (NULL == ap_addr_list[0]))
	{
		trcTrace0 (ulModuleTrace, "ap_add_list error", 1, 1);
		goto LABEL_EXIT;
	}
	
	strcpy (ac_ip, inet_ntoa(*ap_addr_list[0]));
	server.sin_addr.s_addr 	= inet_addr(ac_ip);
    	server.sin_family 	= AF_INET;
   	server.sin_port 	= htons (descGetServerPort());
    	
	//Connect to remote server
    	if (connect(nSocketDesc, (struct sockaddr *)&server , sizeof(server)) < 0)
    	{
        	trcTrace0 (ulModuleTrace, "connect error", 1, 1);
        	goto LABEL_EXIT;
    	}
     
LABEL_EXIT:

	return;
}

/******************************************************************************/
/******************************************************************************/
static void init 
(
	void
)
{
	nConnectionTimeS = descGetConnectionTime ();
	nConnectionTimeS = descGetConnectionTime();
	gettimeofday (&stCurrentTime,  NULL);
	gettimeofday (&stPresenceTime, NULL);
	gettimeofday (&stMeasuresTime, NULL);

	pthread_mutex_init (&MutexServer, NULL);

}

/******************************************************************************/
/*****************************************************************************/
static int send_script
(
	char * _pc_script,
	char * _pc_data,
	int    _n_data_len
)
{
	char * 	pc_buffer = NULL;
	char 	ac_reception [1000];
	int	n_buffer_len;
	int     n_result;
	int	n_reception_len;
	int 	n_return = -1;
	char *  pc_server_ok = NULL;
	char 	ac_trace [50];
	int	n_i;
	char    c_data;

        connection();

        if (-1 == nSocketDesc)
        {
                goto LABEL_EXIT;
        }

	n_buffer_len  = 200;
	n_buffer_len += _n_data_len * 2;

	pc_buffer = malloc (n_buffer_len);
	if (NULL == pc_buffer)
	{
		trcTrace0 (ulModuleTrace, "Malloc error", 1, 1);
		goto LABEL_EXIT;
	}

	memset (pc_buffer, 0x00, n_buffer_len);

	sprintf
	(
		pc_buffer, 
		"POST /_10/%s?d=%s&c=%s&v=_10&u=%s&p=%s&h=",
		_pc_script,
		descGetDb(),
		descGetSn(),
		descGetUser(),
		descGetPwd()
	);
	
	n_buffer_len = strlen (pc_buffer);
	
	for (n_i = 0; n_i < _n_data_len; n_i++)
	{
		sprintf(&pc_buffer[n_i * 2 + n_buffer_len], "%2.2X", _pc_data[n_i]);
	}

	c_data = 0;
	for (; n_i < _n_data_len + 11; n_i++)
	{
		//sprintf(&pc_buffer[n_i * 2 + n_buffer_len], "%2.2X", n_i - _n_data_len);
		sprintf(&pc_buffer[n_i * 2 + n_buffer_len], "%2.2X", c_data);
	}

	n_buffer_len = strlen (pc_buffer);

	strcpy(&pc_buffer[n_buffer_len], "\nHost: %s\r\n\r\n");

        n_result = send(nSocketDesc , pc_buffer , strlen(pc_buffer) , 0);
        if (n_result < 0)
        {
                trcTrace0 (ulModuleTrace, "Send server error", 1, 1);
                goto LABEL_EXIT;
        }

        n_reception_len = recv (nSocketDesc, ac_reception, sizeof(ac_reception), 0);

        if (n_reception_len > 0)
        {
//                trcTrace0 (ulModuleTrace, ac_reception, 1, 1);
        }


	pc_server_ok = strstr (ac_reception, STR_VAL_SERVER_OK);	

LABEL_EXIT:

	if (pc_buffer != NULL)
	{
		free(pc_buffer);
	}

//	trcTrace0 (ulModuleTrace, _pc_script, 1, 1);
	if (pc_server_ok != NULL)
	{
		sprintf (ac_trace, "%s : SERVER OK", _pc_script);
		n_return = 1;
	}
	else
	{
		sprintf (ac_trace, "%s : SERVER KO", _pc_script);
	}

	trcTrace0 (ulModuleTrace, ac_trace, 1, 1);
	return n_return;	
	
}

/******************************************************************************/
/******************************************************************************/
static void end 
(
	void
)
{
	trcTrace0 (ulModuleTrace, "End of thread server", 1, 1);
}

// End of server.c

