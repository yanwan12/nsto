// rf_protocol.c
// 22/11/2013


char cRfpTagFirstKeyProtocol = 0x4B;

char acRfpCmdKeyGetDescriptor 	[0x12] = { 0x4B, 0x10, 0x14, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00};
char acRfpCmdKeyStart 		[0x12] = { 0x4B, 0x10, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0x00};
char acRfpCmdKeyStop 		[0x12] = { 0x4B, 0x10, 0x50, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0x00};
char acRfpCmdLoggerGetDescStart	[0x12] = { 0x4E, 0x10, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x0C, 0x00};
char acRfpCmdLoggerGetDescNext	[0x15] = { 0x4E, 0x13, 0x12, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x03, 0x1F, 0x00};
char acRfpCmdLoggerGetDescLast	[0x15] = { 0x4E, 0x13, 0x18, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x03, 0x1F, 0x00};



// End of rf_protocol.c

