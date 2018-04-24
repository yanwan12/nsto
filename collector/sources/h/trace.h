// trace.h
// 22/11/2013

#ifndef _TRACE_H_
#define _TRACE_H_

#include "frame.h"

#define TRC_MODULE_ALARM		0x00000100
#define TRC_MODULE_TOOL			0x00000080
#define TRC_MODULE_DESCRIPTOR		0x00000040
#define TRC_MODULE_FRAME		0x00000020
#define TRC_MODULE_STORAGE		0x00000010
#define TRC_MODULE_THREAD_SERVER	0x00000008
#define TRC_MODULE_THREAD_RF		0x00000004
#define TRC_MODULE_COM			0x00000002
#define TRC_MODULE_MAIN			0x00000001


void trcInit   (unsigned long _ul_mask_module);
void trcTrace0 (unsigned long _ul_module, char * _pc_trace, unsigned char _b_date, unsigned char _b_cr);
void trcTrace1 (unsigned long _ul_module, char * _pc_trace, int _n_param0, unsigned char _b_date, unsigned char _b_cr);
void trcTrace2 (unsigned long _ul_module, char * _pc_msg, char * _pc_buffer, int _n_buff_len, unsigned char _b_date, unsigned char _b_cr);
void trcTraceFrame (unsigned long _ul_module, pstFrame _pst_frame, unsigned char _b_date, unsigned char _b_cr);	

#endif //_TRACE_H_

// End of trace.h

