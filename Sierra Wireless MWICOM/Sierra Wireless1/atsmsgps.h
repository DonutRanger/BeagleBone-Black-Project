/*
 * at.h
 *
 *  Created on: Jan 8, 2016
 *      Author: oto
 */

#ifndef ATSMSGPS_CPP_
#define ATSMSGPS_CPP_

struct GPSInfo {
	int gga_FixQuality;
	float gga_hdop;
	float longitude_deg;
	float latitude_deg;
	float altitude_m;
	float height_m;
	float groundSpeed_kph;
	float trueTrack_deg;
	float magneticTrack_deg;
//	struct tm utcDateTime;
};

int ttyInit(void);
int ttyInitSerial(char *);
int gps(int);

int ttySerialSet(char *);
int gpsActive(char *);
GPSInfo *gpsNMeaLocation(char*);
void parseNMea(char *, unsigned int);
int wvdialActive(void);

void sendData(float, float);

////CMUXing
//uint8_t atCMuxQuery[]={"AT+CMUX?\r"};
//uint8_t atCMuxSet[]={"AT+CMUX=0,0,7,31,5,3,15,5\r"};
//uint8_t ctrlC[]={3};
//
////Get modem manufacturer, check access available and get all sms
////AT read must not exceed 2000
//
////Network AT commands
//uint8_t atBand[]={"at!band?\r"};
//uint8_t atCopsQuery[]={"at+cops?\r"};
//uint8_t atCreg[]={"at+creg\r"};	//gsm registration
//uint8_t atCgatt[]={"at+cgatt\r"};
//
////SMS AT commands
//uint8_t atInitSMS[]={"at+cgmi;+CPIN?;+cmgf=1\r"}; //ascii \r=13 dec
//uint8_t atReadSMS[]={"at+cmgl=\"all\"\r"};
//uint8_t atSendSMSTo1[]={"at+cmgs=\"012xxxxxxx\"\r"}; //Phone number
//uint8_t atSendSMSTo2[]={"at+cmgs=\"016xxxxxxx\"\r"}; //Phone number
//uint8_t atCtrlZ[]={26};	//ascii ctrlz=26 dec
//uint8_t atEnter[]={"\r"};
//uint8_t atMsgSMS[]={"\"Testing Sierra Wireless. Hi This is Danny. Please reply\""};
//
//
//GPS AT commands
//uint8_t atGPSAutoStart[]={"at!gpsautostart=1,1,100,10,10\r"}; //1=Enable, 2=GPS standalone, 100=max time for fix, 10=max distance, 10=max time betw fixes.
//uint8_t atGPSColdStart[]={"at!gpscoldstart\r"};
//uint8_t atGPSFix[]={"at!gpsfix=1,100,10\r"};	//1=GPS Standalone, 100sec wait for fix, 10m within
//uint8_t atGPSEnd[]={"at!gpsend=0\r"};
//uint8_t atGPSLoc[]={"at!gpsloc?\r"};
//uint8_t atEnterCND[]={"at!entercnd=\"A710\"\r"};
//uint8_t atGPSEnable[]={"at!custom=\"gpsenable\",1\r"};
//uint8_t atGPSSel[]={"at!custom=\"gpssel\",0\r"};
//uint8_t atReset[]={"at!reset\r"};
//uint8_t atGPSTrack[]={"at!gpstrack=1,255,100,1000,20\r"};
//uint8_t atGPSStatus[]={"at!gpsstatus?\r"};
//uint8_t atGPSSatInfo[]={"at!gpssatinfo?\r"};
//uint8_t atGPSOnlyQuery[]={"at!gpsonly?\r"};
//uint8_t atGPSOnlyEnabled[]={"at!gpsonly=1\r"};
//
////sierra wireless response buffer
//extern uint8_t atResponse[2000];

#endif /* ATSMSGPS_CPP_ */

