// main.c
// 22/11/2013

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "trace.h"
#include "version.h"
#include "rf.h"
#include "server.h"
#include "descriptor.h"
#include "tool.h"
#include "frame.h"
#include "storage.h"
#include "alarm.h"

static unsigned long ulTraceModule = TRC_MODULE_MAIN;

static pthread_t hThreadRf;
static pthread_t hThreadServer;

static void manage_option 	(int _n_argc, char ** _ap_argv);
static void init 		(void);
static void end			(void);

/***************************************** main.c ***********************************************/
/****************************************************************************************/
int main 
(
	int _n_argc, 
	char ** _ap_argv
)
{

	void * ret;

/*
	FILE * h_file;
	int n_toto;
	char ac_toto [2000];
	h_file = fopen ("./descriptor/LOG0013CC.nbf", "rb");
	printf ("file hd = %X\n", (int)h_file);
	n_toto = ftell (h_file);
	printf("file len = %d\n", n_toto);
	n_toto = fread (ac_toto, 1, sizeof(ac_toto), h_file);
	printf ("File read %d\n", n_toto);
	fclose (h_file);
	exit (0);
*/

	init();


	if (_n_argc > 1)
	{
		manage_option (_n_argc, _ap_argv);
	}


	if (pthread_create (&hThreadRf, NULL, rf_Thread, NULL) < 0) 
	{
		trcTrace0 (ulTraceModule, "Rf thread creation error", 1, 1);
	}


	if (pthread_create (&hThreadServer, NULL, server_Thread, NULL) < 0) 
	{
		trcTrace0 (ulTraceModule, "Server thread creation error", 1, 1);
	}


   	(void)pthread_join (hThreadServer, &ret);

 	(void)pthread_join (hThreadRf, &ret);

	end ();

	trcTrace0 (ulTraceModule, "End of application", 1, 1);
	
	return 0;

}

/****************************************************************************************/
/****************************************************************************************/
static void init 
(
	void
)
{
	trcTrace0 (ulTraceModule, "*****************************************", 1, 1);
	trcTrace0 (ulTraceModule, "* collector ",  1, 0);
	trcTrace0 (ulTraceModule, VAL_STR_VERSION, 0, 1);
	trcTrace0 (ulTraceModule, "*****************************************", 1, 1);

	toolInit ();
	descInit ();
	frmInit  ();	
	stoInit  ();
	arlInit  ();
}

/****************************************************************************************/
/****************************************************************************************/
static void end
(
	void
)
{
	arlEnd 	();
	stoEnd 	();
	frmEnd 	();
	descEnd ();
	toolEnd ();
}

/****************************************************************************************/
/****************************************************************************************/
static void manage_option 
(
	int 	_n_argc, 
	char ** _ap_argv
)
{
	int i;

	for (i = 1; i < _n_argc; i++)
	{
		char * pc_param = _ap_argv[i];
		switch(pc_param[1])
		{
			case 'h':
				break;

			case 'l':		
				if (0x00 == pc_param[2])	
				{
					trcInit (0xFFFFFFFF);
				}
				else
				{
					trcInit(atoi(&pc_param[2]));
				}
				break;
		}
		if (0 == memcmp(_ap_argv[i], "-h", 2))
		{
			trcTrace0(ulTraceModule, "Collector options : ", 1, 1);
		}
	}
}


// End of main.c
