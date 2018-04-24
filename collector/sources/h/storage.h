// storage.h
// 22/11/2013

#include "frame.h"

void 	stoInit 		(void);

void 	stoEnd 			(void);

void 	stoAdd 			(pstFrame _pc_frame);

int 	stoGetMeasuresNb 	(void);

void 	stoSave 		(void);

int 	stoGetMeasures 		(char * _pc_buffer, int * _pn_buffer_len);

void 	stoRemoveMeasures 	(int _n_measures_nb);

// End of storage.h

