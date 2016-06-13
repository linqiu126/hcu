/*
 * configxml.h
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#ifndef L0SERVICE_CONFIGXML_H_
#define L0SERVICE_CONFIGXML_H_

#include "../l0comvm/vmlayer.h"
#include "../l1hwopt/spsapi.h"

//包含L2FRAME的文件，层次结构稍微有点乱，后期需要调整这个东西

//extern char tolower(char c);
UINT32 htoi(char s[]);
UINT32 AnalysisOfXml(char *docv);
UINT32 parseDoc(xmlDocPtr doc, xmlNodePtr cur, char *FatherElt, char *ChildElt, char *out);
UINT32 parseDoc3(xmlDocPtr doc, xmlNodePtr cur, char *FatherElt, char *ChildElt, char *GrandchildrenElt, char *out);
void ApartAddress(char *temp, char *stringAdd1, char *stringAdd2);


/*
 * Extern Global Variables
 */
SerialPortCom_t gSerialPortZb;
SerialPortCom_t gSerialPortGprs;
SocketPort_t gSocketPort;
HcuMeasureBehavior_t gHcuMeasureBehavior;
WedSensors_t gZbSensors;
UINT8 gFcsEnable;
UINT8 gUseGprsOrNot;
INT32  outputOnCrt;
UINT8 TraceWillBeDisplayed;
MsgSeverDataSegHeader_t gMsgSeverDataSegHeader;

#endif /* L0SERVICE_CONFIGXML_H_ */
