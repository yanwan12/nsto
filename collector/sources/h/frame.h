// frame.h
// 22/11/2013

#include "rf_protocol.h"

#ifndef _FRAME_H_
#define _FRAME_H_

#define LEN_FRAME_TX_PROTOCOL		2
#define LEN_FRAME_STR_SERIAL_NB		10
#define LEN_FRAME_HEX_SERIAL_NB		6
#define LEN_FRAME_PAYLOAD		0x10

struct ST_FRAME
{
	void * 	pvNextFrame;
	char 	acHexData   	[LEN_FRAME + LEN_FRAME_TX_PROTOCOL]; // 2 BYTE for TX protocol
	char 	acProtocol  	[LEN_FRAME_TX_PROTOCOL];
	char 	cTag;
	char    acSourceSnHex 	[LEN_FRAME_HEX_SERIAL_NB];
	char 	acSourceSnStr  	[LEN_FRAME_STR_SERIAL_NB];
	char    acDestSnHex	[LEN_FRAME_HEX_SERIAL_NB];
	char 	acDestSnStr    	[LEN_FRAME_STR_SERIAL_NB];
	char 	cPayloadLen;
	char 	cPi0;
	char 	cPi1;
	char 	acPayload   	[LEN_FRAME_PAYLOAD];
	char 	cCrc;	
	char    acDescFile  [30];
};

typedef struct ST_FRAME *	pstFrame;

pstFrame  	frmConstructor 	(char * _pc_reception, char _c_tx);
pstFrame 	frmGetCurrent 	(void);
void 		frmInit 	(void);
void 		frmEnd		(void);

#endif // _FRAME_H_

// End of frame.h

