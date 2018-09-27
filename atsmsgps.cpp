/*
 * atsmsgps.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: oto
 */

//C++ program
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <string>


#include "atsmsgps.h"

int ttyUSBSerial(uint8_t *atStr, uint8_t *atResponse, int sendlen, int readlen);

int ttyInit(void){
	 //setup ttyUSB2 for Sierra Wireless
	 system("stty -F /dev/ttyUSB2 -parity -echo -icanon");
	 return 0;
}

int sms(void){
	//Network checks
	ttyUSBSerial(atCopsQuery,atResponse, sizeof(atCopsQuery),1000);
	ttyUSBSerial(atCreg,atResponse, sizeof(atCreg),1000);

	//Read & Send SMS via Sierra Wireless modem
	ttyUSBSerial(atInitSMS,atResponse, sizeof(atInitSMS), 1000);
	ttyUSBSerial(atBand,atResponse, sizeof(atBand), 1000);
	ttyUSBSerial(atReadSMS,atResponse, sizeof(atReadSMS), 1000);

//or you may want to send sms
	 ttyUSBSerial(atSendSMSTo1,atResponse, sizeof(atSendSMSTo1), 0);
	 ttyUSBSerial(atMsgSMS, atResponse, sizeof(atMsgSMS), 0);
	 ttyUSBSerial(atCtrlZ, atResponse, sizeof(atCtrlZ), 1000);

//	 ttyUSBSerial(atSendSMSTo2,atResponse, sizeof(atSendSMSTo2), 0);
//	 ttyUSBSerial(atMsgSMS, atResponse, sizeof(atMsgSMS), 0);
//	 ttyUSBSerial(atCtrlZ, atResponse, sizeof(atCtrlZ), 1000);
// }
	return 0;
}

 int gps(int locCount){
	 //Read GPS via Sierra Wireless modem

	 ttyUSBSerial(atEnterCND,atResponse,sizeof(atEnterCND),1000);
	 ttyUSBSerial(atGPSEnable,atResponse,sizeof(atGPSEnable),1000);
//	 ttyUSBSerial(atGPSSel,atResponse,sizeof(atGPSSel),1000);
//	 ttyUSBSerial(atReset,atResponse,sizeof(atReset),1000);
//	 sleep(5);	//sleep 5 sec
//	 ttyInit();
	 ttyUSBSerial(atEnterCND,atResponse,sizeof(atEnterCND),1000);

	 ttyUSBSerial(atGPSAutoStart,atResponse, sizeof(atGPSAutoStart), 1000);
	 ttyUSBSerial(atGPSSatInfo,atResponse, sizeof(atGPSSatInfo), 1000);

	 ttyUSBSerial(atGPSTrack,atResponse, sizeof(atGPSTrack), 1000);
	 while(locCount){
		 locCount--;
		 ttyUSBSerial(atGPSStatus,atResponse, sizeof(atGPSStatus), 1000);
		 ttyUSBSerial(atGPSLoc,atResponse, sizeof(atGPSLoc), 1000);
	 }
	 return 0;
}
