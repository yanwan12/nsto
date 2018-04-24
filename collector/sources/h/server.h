// erverh
// 21/11/2013

#ifndef _SERVER_H_
#define _SERVER_H_

#include "frame.h"

void * server_Thread 	(void * arg);

void svrSendDescriptor 	(pstFrame _pst_frame);

void svrAddMessage 	(int _n_msg_type, char * _pc_param);

#endif // _SERVER_H_

// END OF server.h
