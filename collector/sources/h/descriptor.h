// descriptor.h
// 22/11/2013

#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_

struct ST_LOG_DESC_NBF 
{
	/* 0x1FF0 */
	char acFabCode [3];
	char cFwBuild;
	char acProductVersion[2];
	char cFwMaj;
	char cFwMin;
	char cFwMode;
	char acSerialNumber[6];
	char cFwOptions;
	/* 0x1FE0 */
	char acDescription[16];
	/* 0x1FD0 */
	char acAddMatch [4];
	char cDescVersion;
	char cProtVersion;
	char cRfu1FD6;
	char acCrc16 [2];
	char cFormulaNb;
	char cLinerNb;
	char cRecMemSize;
	char cRfu1FDC;
	char cRfBand;
	char cRfTxPwr;
	char cRfChannel;
	/* 0x1FC0 */
	char acRfu1FC0[14];
	char acUserId[2];
	/* 0x1FB0 */
	char cSalveNb;
	char cOpMode;
	char cDebLen;
	char cMemState;
	char acLastMeasureIndex[4];
	char acFirstMeasureIndex[4];
	char acNewMeasureIndex[4];
	/* 0x1FA0 */
	char acRfu1FA0[3];
	char acLivePeriod[2];
	char acRfu1FA5[3];
	char acRecordPeriod[2];
	char cBtldVersion;
	char cBtldBuild;
	char acRfu1FAA[4];
	/* 0x1F90 */
	char acRfu1F90[16];
	/* 0x1F80 */
	char acFormulaSetting[0xFFFF];
} stColDescNbf;

struct ST_LOG_DESC
{
       char acSerialNumber[9+1];
       char acHexSerialNumber[6];
       char acDescription[16+1];
       char acRfOptions[4];
       char acSetting[16];
       char acServerAdd[4*16];
       char acServerDb[16];
       char acServerUser[16];
       char acServerPwd[16];
       char acServerPort[16];
       int  nServerConnectionTimeS;
       char RFU;
}stColDesc;
						
		
void	descInit		(void);
void	descEnd			(void);
char * 	descGetSn 		(void);
char *  descGetHexSn		(void);
char * 	descGetDb 		(void);
char * 	descGetUser 		(void);
char * 	descGetPwd		(void);
char * 	descGetServerAdd	(void);

int 	descGetServerPort	(void);

int 	descGetConnectionTime	(void);

void    descRemove 		(pstFrame _pst_frame);
void    descSave		(pstFrame _pst_frame);

char 	descIsPresent 		(pstFrame _pst_frame);

#endif // _DESCRIPTOR_H_

// End of descriptor.h

