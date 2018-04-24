// frame.c
// 22/11/2013

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "frame.h"
#include "tool.h"
#include "trace.h"
#include "path.h"

#define OFFSET_TAG	0

static pstFrame 	pCurrentFrame = NULL;
static pthread_mutex_t	MutexFrame;
static unsigned long    ulModuleTrace = TRC_MODULE_FRAME;

/*****************************************************************************/
/*****************************************************************************/
pstFrame frmConstructor 
(
	char * _pc_reception,
	char   _c_tx
)
{
	pstFrame pst_new_frame;
	char 	 c_offset = 0;

	if (1 == _c_tx)
	{
		c_offset = 2;
	}

	pst_new_frame = malloc (sizeof(struct ST_FRAME));
	if (pst_new_frame != NULL)
	{
		pst_new_frame->acProtocol [0] = 0x00;
		pst_new_frame->acProtocol [1] = 0x00;
		if (1 == _c_tx)
		{
			c_offset = 2;
			pst_new_frame->acProtocol [0] = _pc_reception[0];
			pst_new_frame->acProtocol [1] = _pc_reception[1];
		}
		memcpy(pst_new_frame->acHexData, _pc_reception, LEN_FRAME);
		pst_new_frame->cTag = _pc_reception[OFFSET_TAG];
		toolGetSerialNb(&_pc_reception[1 + c_offset], pst_new_frame->acDestSnStr);
		memcpy (pst_new_frame->acDestSnHex, &_pc_reception[1 + c_offset], LEN_FRAME_HEX_SERIAL_NB);
		toolGetSerialNb(&_pc_reception[7 + c_offset], pst_new_frame->acSourceSnStr);
		memcpy (pst_new_frame->acSourceSnHex, &_pc_reception[7 + c_offset], LEN_FRAME_HEX_SERIAL_NB);
		memcpy (pst_new_frame->acDestSnHex, &_pc_reception[1 + c_offset], LEN_FRAME_HEX_SERIAL_NB);
		pst_new_frame->cPayloadLen 	= _pc_reception [13 + c_offset];
		pst_new_frame->cPi0		= _pc_reception [14 + c_offset];
		pst_new_frame->cPi1		= _pc_reception [15 + c_offset];
		memcpy (pst_new_frame->acPayload, &_pc_reception[16 + c_offset], LEN_FRAME_PAYLOAD);
		sprintf(pst_new_frame->acDescFile, "%s/%s.nbf", VAL_PATH_DESCRIPTOR, pst_new_frame->acSourceSnStr);
		goto LABEL_EXIT;
	}

LABEL_EXIT:

	return pst_new_frame;
}

/*****************************************************************************/
/*****************************************************************************/
pstFrame frmGetCurrent
(
	void
)
{
	pstFrame p_return = NULL;

	if (NULL == pCurrentFrame)
	{
		goto LABEL_EXIT;
	}

	toolMutexLock (&MutexFrame);

	p_return = pCurrentFrame;

	pCurrentFrame = (pstFrame) pCurrentFrame->pvNextFrame;

	toolMutexUnlock (&MutexFrame);

LABEL_EXIT:

	return p_return;
}


/*****************************************************************************/
/*****************************************************************************/
void frmInit
(
	void
)
{
	trcTrace0 (ulModuleTrace, "frmInit", 1, 1);
	pthread_mutex_init (&MutexFrame, NULL);
}


/*****************************************************************************/
/*****************************************************************************/
void frmEnd
(
	void
)
{
	trcTrace0 (ulModuleTrace, "frmEnd", 1, 1);
}


// End of frame.c
