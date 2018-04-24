// storage.c
// 22/11/2013

#include <pthread.h>
#include <sys/stat.h>
#include <stdio.h>

#include "storage.h"
#include "tool.h"
#include "trace.h"
#include "path.h"

#define LEN_STORAGE	LEN_FRAME


static pthread_mutex_t	MutexStorage;
static FILE *		hFile;
static int		nMeasuresNb;
static unsigned long	ulModuleTrace = TRC_MODULE_STORAGE;

/*****************************************************************************/
/*****************************************************************************/
void stoInit 
(
	void
)
{
	trcTrace0 (ulModuleTrace, "StoInit", 1, 1);

	nMeasuresNb = 0;

	toolCopyFile (VAL_PATH_STORAGE_SAVE, VAL_PATH_STORAGE);

	hFile = fopen (VAL_PATH_STORAGE, "ab");
	if (hFile != NULL)
	{
		nMeasuresNb = ftell(hFile) / LEN_STORAGE;
	}

	fclose (hFile);

	pthread_mutex_init (&MutexStorage, NULL);
	
	trcTrace1 (ulModuleTrace, "Measure Nb = %d", nMeasuresNb, 1, 1);
}

/*****************************************************************************/
/*****************************************************************************/
void stoAdd 
(
	pstFrame _pc_frame
)
{
	toolMutexLock (&MutexStorage);

	hFile = fopen (VAL_PATH_STORAGE, "ab");
	if (hFile != NULL)
	{	
	    	fwrite (_pc_frame->acHexData, LEN_STORAGE, 1, hFile);
		fclose (hFile);
		nMeasuresNb++;
		//trcTrace1 (ulModuleTrace, "Measure Nb = %d", nMeasuresNb, 1, 1);
	}
	
	toolMutexUnlock (&MutexStorage);

}

/*****************************************************************************/
/*****************************************************************************/
int stoGetMeasures
(
	char * _pc_buffer,
	int  * _pn_buffer_len
)
{
	int n_read_nb = 0;

	toolMutexLock (&MutexStorage);

	hFile = fopen (VAL_PATH_STORAGE, "rb");
	if (hFile != NULL)
	{
		
		n_read_nb = fread (_pc_buffer, 1, *_pn_buffer_len, hFile);
		fclose (hFile);	
		//remove (VAL_PATH_STORAGE);
		//nMeasuresNb = 0;
	}

	toolMutexUnlock (&MutexStorage);

	return n_read_nb;
}

/*****************************************************************************/
/*****************************************************************************/
int stoGetMeasuresNb
(
	void
)
{
	int n_return = 0;

	toolMutexLock (&MutexStorage);

	n_return = nMeasuresNb;

	toolMutexUnlock (&MutexStorage);

	return n_return;
}



/*****************************************************************************/
/*****************************************************************************/
void stoRemoveMeasures
(
	int _n_measures_nb
)
{
	int n_char_nb;
	FILE * h_file_source 	= NULL;
	FILE * h_file_dest	= NULL;
	char ac_buffer [1000];

	toolMutexLock (&MutexStorage);

	//trcTrace1 (ulModuleTrace, "Remove %d Measures", _n_measures_nb / LEN_STORAGE, 1, 1);

	if (_n_measures_nb == nMeasuresNb)
	{
		nMeasuresNb = 0;
		remove (VAL_PATH_STORAGE);
		goto LABEL_EXIT;
	}

	remove (VAL_PATH_STORAGE_TMP);

	h_file_source = fopen (VAL_PATH_STORAGE, "rb");
	if (NULL == hFile)
	{
		trcTrace0 (ulModuleTrace, "Storage file error", 1, 1);
		goto LABEL_EXIT;
	}

	h_file_dest = fopen (VAL_PATH_STORAGE_TMP, "wb");
	if (NULL == h_file_dest)
	{
		trcTrace0 (ulModuleTrace, "Storage tmp file error", 1, 1);
		goto LABEL_EXIT;
	}

	fseek (h_file_source, _n_measures_nb, SEEK_SET);

	do
	{
		n_char_nb = fread (ac_buffer, 1, sizeof(ac_buffer), h_file_source);
		if (n_char_nb > 0)
			fwrite (ac_buffer, 1, n_char_nb, h_file_dest);
	}
	while (n_char_nb > 0);

	remove (VAL_PATH_STORAGE);

	rename (VAL_PATH_STORAGE_TMP, VAL_PATH_STORAGE);

	nMeasuresNb = (ftell (h_file_dest) / LEN_STORAGE);

LABEL_EXIT :

	//trcTrace1 (ulModuleTrace, "After stoRemoveMeasures, Measures nb = %d", nMeasuresNb, 1, 1);

	if (h_file_source != NULL)
	{
		fclose (h_file_source);
	}

	if (h_file_dest)
	{
		fclose (h_file_dest);
	}

	toolMutexUnlock (&MutexStorage);
	
}


/*****************************************************************************/
/*****************************************************************************/
void stoEnd
(
)
{

	trcTrace0 (ulModuleTrace, "stoEnd", 1, 1);
	int n_result;

	toolMutexLock (&MutexStorage);

	n_result = toolCopyFile (VAL_PATH_STORAGE, VAL_PATH_STORAGE_SAVE);

	if (0 == n_result)
	{
		trcTrace0 (ulModuleTrace, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", 1, 1);
		trcTrace0 (ulModuleTrace, "!  STORAGE COPY FILE ERROR  !", 1, 1);
		trcTrace0 (ulModuleTrace, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", 1, 1);

	}

	toolMutexUnlock (&MutexStorage);

}
// End of storage.c

