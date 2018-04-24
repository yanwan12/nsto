// descriptor.c
// 22/11/2013

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdlib.h>

#include "frame.h"
#include "path.h"
#include "trace.h"
#include "tool.h"
#include "rf_protocol.h"
#include "descriptor.h"

#define VAL_PATH_DESCRIPTOR_NBF		VAL_PATH_DESCRIPTOR "/COL.nbf"
#define VAL_PATH_DESCRIPTOR_NBF_REF     VAL_PATH_DESCRIPTOR "/ref/COL.nbf"

static pthread_mutex_t	MutexDescriptor;
static unsigned long 	ulModuleTrace = TRC_MODULE_DESCRIPTOR;

/*****************************************************************************/
/*****************************************************************************/
void descInit
(
	void
)
{
	struct stat   		buffer;
	int			n_result;
	FILE *			ph_desc;

	trcTrace0 (ulModuleTrace, "descInit", 1, 1);

	pthread_mutex_init (&MutexDescriptor, NULL);
/*
	mkdir (VAL_PATH_DESCRIPTOR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
*/


	memset (&stColDescNbf, 0x00, sizeof(struct ST_LOG_DESC_NBF));
        memset (&stColDesc,    0x00, sizeof(struct ST_LOG_DESC));

	n_result = stat (VAL_PATH_DESCRIPTOR_NBF, &buffer);
	if (n_result != 0)
	{

		toolCopyFile (VAL_PATH_DESCRIPTOR_NBF_REF, VAL_PATH_DESCRIPTOR_NBF);
	}

	ph_desc =  fopen(VAL_PATH_DESCRIPTOR_NBF, "rb");
	if (NULL == ph_desc)
	{
		goto LABEL_EXIT;
	}

	fread (&stColDescNbf, 1, sizeof (struct ST_LOG_DESC_NBF), ph_desc);

	
	toolGetSerialNb (stColDescNbf.acSerialNumber, stColDesc.acSerialNumber);
	memcpy (stColDesc.acHexSerialNumber, stColDescNbf.acSerialNumber, sizeof(stColDescNbf.acSerialNumber));
	memcpy (stColDesc.acRfOptions,   &stColDescNbf.cRfu1FDC, 	sizeof(stColDesc.acRfOptions));	
	memcpy (stColDesc.acSetting,     &stColDescNbf.acFormulaSetting[3], sizeof(stColDesc.acSetting));
	memcpy (stColDesc.acDescription, stColDescNbf.acDescription, 	sizeof(stColDesc.acDescription)-1);

	toolCopySpecialDescBuffer (&stColDescNbf.acFormulaSetting[0x10], stColDesc.acServerAdd);
        toolCopyDescBuffer 	  (&stColDescNbf.acFormulaSetting[0x50], stColDesc.acServerDb);
	toolCopyDescBuffer 	  (&stColDescNbf.acFormulaSetting[0x60], stColDesc.acServerDb);
        toolCopyDescBuffer 	  (&stColDescNbf.acFormulaSetting[0x70], stColDesc.acServerUser);
        toolCopyDescBuffer 	  (&stColDescNbf.acFormulaSetting[0x80], stColDesc.acServerPwd);
        toolCopyDescBuffer  	  (&stColDescNbf.acFormulaSetting[0x90], stColDesc.acServerPort);
	
	toolGetTimeS		  (stColDescNbf.acLivePeriod, 		 &stColDesc.nServerConnectionTimeS);

LABEL_EXIT :

        trcTrace0 (ulModuleTrace, stColDesc.acSerialNumber, 	1,  	1);
	trcTrace0 (ulModuleTrace, stColDesc.acDescription, 	1,	1);
	trcTrace0 (ulModuleTrace, stColDesc.acSetting, 		1, 	1);
	trcTrace0 (ulModuleTrace, stColDesc.acServerAdd,        1, 	1);
	trcTrace0 (ulModuleTrace, stColDesc.acServerDb,         1, 	1);
	trcTrace0 (ulModuleTrace, stColDesc.acServerUser,       1, 	1);
	trcTrace0 (ulModuleTrace, stColDesc.acServerPwd,        1, 	1);
	trcTrace1 (ulModuleTrace, "%d",  atoi(stColDesc.acServerPort),     1, 1);
	trcTrace1 (ulModuleTrace, "%ds", stColDesc.nServerConnectionTimeS, 1, 1);
}

/*****************************************************************************/
/*****************************************************************************/
void descEnd
(
	void
)
{
	trcTrace0 (ulModuleTrace, "descEnd", 1, 1);
}

/*****************************************************************************/
/*****************************************************************************/
int descGetServerPort 
(
	void
)
{
	return atoi(stColDesc.acServerPort);
}

/*****************************************************************************/
/*****************************************************************************/
char * descGetHexSn 
(
	void
)
{
	return stColDesc.acHexSerialNumber;
}

/*****************************************************************************/
/*****************************************************************************/
int descGetConnectionTime
(
	void
)
{
	return stColDesc.nServerConnectionTimeS;
}


/*****************************************************************************/
/*****************************************************************************/
char descIsPresent
(
	pstFrame _pst_frame
)
{
	char   c_return = 0;
    FILE * h_file;

    toolMutexLock (&MutexDescriptor);

    h_file = fopen (_pst_frame->acDescFile, "rb");
    if (NULL == h_file)
    {
            goto LABEL_EXIT;
    }

	fclose (h_file);

	c_return = 1;

LABEL_EXIT:

        toolMutexUnlock (&MutexDescriptor);

        return c_return;
}


/*****************************************************************************/
/*****************************************************************************/
void descRemove
(
	pstFrame _pst_frame
)
{
	toolMutexLock (&MutexDescriptor);

	remove (_pst_frame->acDescFile);

	toolMutexUnlock (&MutexDescriptor);
}



/*****************************************************************************/
/*****************************************************************************/
void descSave 
(
	pstFrame _pst_frame
)
{
	FILE * h_file;
	int    n_len;

	toolMutexLock (&MutexDescriptor);

	h_file = fopen (_pst_frame->acDescFile, "ab");
	if (NULL == h_file)
	{
		goto LABEL_EXIT;
	}

	n_len = ftell (h_file);

	trcTrace0 (ulModuleTrace, _pst_frame->acDescFile, 1, 0);
	trcTrace1 (ulModuleTrace, " => File size = 0x%2.2X", n_len, 0, 1);

	fwrite (_pst_frame->acPayload, LEN_FRAME_PAYLOAD, 1, h_file);

	fclose (h_file);

LABEL_EXIT:

	toolMutexUnlock (&MutexDescriptor);

	return;
}


/*****************************************************************************/
/*****************************************************************************/
char * descGetServerAdd 
(
	void
)
{
	return stColDesc.acServerAdd;
}

/*****************************************************************************/
/*****************************************************************************/
char * descGetSn 
(
	void
)
{
	return stColDesc.acSerialNumber;
}

/*****************************************************************************/
/*****************************************************************************/
char * descGetDb 	
(
	void
)
{
	return stColDesc.acServerDb;
}

/*****************************************************************************/
/*****************************************************************************/
char * descGetUser 	
(
	void
)
{
	return stColDesc.acServerUser;
}

/*****************************************************************************/
/*****************************************************************************/
char * descGetPwd	
(
)
{
	return stColDesc.acServerPwd;
}



// End of descriptor.c

