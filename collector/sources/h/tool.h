// tool.h
// 22/11/2013

#include <pthread.h>

void toolInit		(void);

void toolEnd		(void);

void toolConvertAscii 	(char * _pc_hex_buffer,	int n_hex_len, char * _pc_ascii_buffer);

void toolMutexLock 	(pthread_mutex_t * _p_mutex);

void toolMutexUnlock 	(pthread_mutex_t * _p_mutex);

char toolTestEndAppli	(void);

void toolEndAppli 	(void);

void toolGetSerialNb 	(char * _pc_buffer, char * _pc_serial_nb);

int toolCopyFile	(char * _pc_source, char * _pc_destination);

void toolCrcInit	(void);

void toolCrcUpdate	(char * _pc_buffer, int _n_buffer_len);

char toolCrcGet		(void);

void toolGetDateTime	(char * _pc_buffer);

char toolHexToBcd 	(char _n_value);

void toolCopyDescBuffer (char * _pc_source, char * _pc_destination);

void toolCopySpecialDescBuffer (char * _pc_source, char * _pc_destination);

void toolGetTimeS	(char * _pc_input_time, int  * _pn_return_time_s);


// Ed of tool.h

