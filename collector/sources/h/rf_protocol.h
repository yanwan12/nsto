// rf_protocl.h
// 22/11/2013

#ifndef _RF_PROTOCOL_H_
#define _RF_PROTOCOL_H_

#define MASK_FRAME_ALARM			0x10
#define MASK_SUBTAG_RESTITUTION		0X08

#define LEN_FRAME_SERIAL_NB			0x06
#define LEN_FRAME_DATE				0x07
#define LEN_FRAME				0x20
#define LEN_FRAME_PAYLOAD			0x10

#define TAG_ACKNOWLEDGE_FROM_KEY		0x4C 
#define TAG_MEASURE				0xA0
#define TAG_MEASURE_ACK				0xA1
#define TAG_ACKNOWLEDGE_FROM_DEVICE		0x0E
#define TAG_GET_DESCRIPTOR_VALUES_FOLLOWED	0x12
#define TAG_GET_DESCRIPTOR_VALUES_LAST		0x18

extern char cRfpTagFirstKeyProtocol;

extern char acRfpCmdKeyGetDescriptor 	[0x12];
extern char acRfpCmdKeyStart 		[0x12];
extern char acRfpCmdKeyStop 		[0x12];
extern char acRfpCmdLoggerGetDescStart	[0x12];
extern char acRfpCmdLoggerGetDescNext	[0x15];
extern char acRfpCmdLoggerGetDescLast	[0x15];

#endif // _RF_PROTOCOL_H_

// End of rf_protocol.h

