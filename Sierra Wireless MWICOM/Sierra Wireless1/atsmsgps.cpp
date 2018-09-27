/*
 * atsmsgps.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: oto
 */

//C++ program
#include "atsmsgps.h"

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <string.h>

#include "minmea/minmea.h"
#include "SocketClient.h"

using namespace std;
using namespace exploringBB;

#define NMEA_MSG_LENGTH 1000
#define NMEADELIMSTRING "\n\r"

// minmea Location and Time Variables
struct minmea_sentence_rmc gpsRMCInfo;			// RMC includes Date and Time
struct minmea_sentence_gga gpsGGAInfo;			// GGA includes GLL info?
struct minmea_sentence_vtg gpsVTGInfo;// VTG is more relevant than RMC as it uses kph

GPSInfo gpsInfoItem;

//sierra wireless response buffer
uint8_t atResponse[2000];
uint8_t readBuffer[1000];

int usbdev = -1;
char *serialPort = "ttyUSB2";
bool isNMeaStart = false;

//CMUXing
uint8_t atCMuxQuery[]={"AT+CMUX?\r"};
uint8_t atCMuxSet[]={"AT+CMUX=0,0,7,31,5,3,15,5\r"};
uint8_t ctrlC[]={3};

//Get modem manufacturer, check access available and get all sms
//AT read must not exceed 2000

//Network AT commands
uint8_t atBand[]={"at!band?\r"};
uint8_t atCopsQuery[]={"at+cops?\r"};
uint8_t atCreg[]={"at+creg\r"};	//gsm registration
uint8_t atCgatt[]={"at+cgatt\r"};

//SMS AT commands
uint8_t atInitSMS[]={"at+cgmi;+CPIN?;+cmgf=1\r"}; //ascii \r=13 dec
uint8_t atReadSMS[]={"at+cmgl=\"all\"\r"};
uint8_t atSendSMSTo1[]={"at+cmgs=\"012xxxxxxx\"\r"}; //Phone number
uint8_t atSendSMSTo2[]={"at+cmgs=\"016xxxxxxx\"\r"}; //Phone number
uint8_t atCtrlZ[]={26};	//ascii ctrlz=26 dec
uint8_t atEnter[]={"\r"};
uint8_t atMsgSMS[]={"\"Testing Sierra Wireless. Hi This is Danny. Please reply\""};

//GPS AT commands
uint8_t atGPSAutoStart[] = { "at!gpsautostart=1,1,100,10,10\r" }; //1=Enable, 2=GPS standalone, 100=max time for fix, 10=max distance, 10=max time betw fixes.
uint8_t atGPSColdStart[] = { "at!gpscoldstart\r" };
uint8_t atGPSFix[] = { "at!gpsfix=1,100,10\r" }; //1=GPS Standalone, 100sec wait for fix, 10m within
uint8_t atGPSEnd[] = { "at!gpsend=0\r" };
uint8_t atGPSLoc[] = { "at!gpsloc?\r" };
uint8_t atEnterCND[] = { "at!entercnd=\"A710\"\r" };
uint8_t atGPSEnable[] = { "at!custom=\"gpsenable\",1\r" };
uint8_t atGPSSel[] = { "at!custom=\"gpssel\",0\r" };
uint8_t atReset[] = { "at!reset\r" };
uint8_t atGPSTrack[] = { "at!gpstrack=1,255,100,1000,20\r" };
uint8_t atGPSStatus[] = { "at!gpsstatus?\r" };
uint8_t atGPSSatInfo[] = { "at!gpssatinfo?\r" };
uint8_t atGPSOnlyQuery[] = { "at!gpsonly?\r" };
uint8_t atGPSOnlyEnabled[] = { "at!gpsonly=1\r" };
uint8_t startNMEA[] = { "$GPS_START" };

int ttyUSBSerial(uint8_t *atStr, uint8_t *atResponse, int sendlen, int readlen);

int ttyUSBSerial(int *usbdev, char *serialPort);

int ttyUSBSerialPort(int *usbdev, uint8_t *atStr, uint8_t *response,
		int sendlen, int readlen);

int ttyUSBStartNmea(int *usbDevNmea, uint8_t *atStr, uint sendlen);
int ttyUSBNmeaRead(int *usbDevNmea, uint8_t *response, int *readlen);

int ttyInit(void) {
	//setup ttyUSB2 for Sierra Wireless
	system("stty -F /dev/ttyUSB2 -parity -echo -icanon");
	return 0;
}

int ttySerialSet(char *serialPort) {

	if (usbdev != -1) {
		close(usbdev);
		usbdev = -1;
	}
	string commandString = string("stty -F /dev/") + serialPort
			+ " -parity -echo -icanon";
	const char * command = commandString.c_str();
	system(command);

	bool status = ttyUSBSerial(&usbdev, serialPort);
	if (status == true) {
		cout << "**********************************" << endl;
		cout << "Opening serial port " << serialPort << endl;
		cout << "**********************************" << endl;
	}

	return 0;
}

int ttySerialOff() {
	if (usbdev != -1) {
		close(usbdev);
		usbdev = 1;
	}
}

void sendData(float lat, float lng) {
	ostringstream head, data;

	SocketClient sc("api.thingspeak.com", 80);
	data << "field1=" << lat << "&field2=" << lng << endl;
	sc.connectToServer();
	head << "POST /update HTTP/1.1\n" << "Host: api.thingspeak.com\n"
			<< "X-THINGSPEAKAPIKEY: BJ5II60RYYWR83WG\n"
			<< "Content-Type: application/x-www-form-urlencoded\n"
			<< "Accept: text/plain\n" << "Content-Length: "
			<< string(data.str()).length() << "\n\n";
	sc.send(string(head.str()));
	sc.send(string(data.str()));
	string rec = sc.receive(1024);
	cout << "[" << rec << "]" << endl;
	sc.disconnectFromServer();
	//cout << "Send random data end" << endl;
}

void parseNMea(uint8_t *atResponse, unsigned int nMeaLen) {
	char *nmeaChar = reinterpret_cast<char*>(atResponse);
	char *nmeaSentence;

	char *ggaSentence = NULL;
	char *rmcSentence = NULL;
	char *vtgSentence = NULL;

	nmeaSentence = strtok(nmeaChar, NMEADELIMSTRING);
	while (nmeaSentence != NULL) {
		// Parse buffer, keep only most recent instance of NMEA Sentence
		// Sentences that fail checksum are still parsed (change minmea_sentence_id() second parameter to true for strict checking)
		//DEBUG("NMEA Sentence [" << nmeaSentence << "]" << endl);
		switch (minmea_sentence_id(nmeaSentence, true)) {
		case MINMEA_SENTENCE_RMC:
			rmcSentence = nmeaSentence;
			break;
		case MINMEA_SENTENCE_GGA:
			ggaSentence = nmeaSentence;
			break;
		case MINMEA_SENTENCE_VTG:
			vtgSentence = nmeaSentence;
			break;
		default:
			// Ignore sentence
			//DEBUG("Skipped" << endl);
			break;
		}
		nmeaSentence = strtok(NULL, NMEADELIMSTRING);
	}

	// Process Sentences
	if (ggaSentence) {
		//DEBUG("Found GGA Sentence" << endl);
		minmea_parse_gga(&gpsGGAInfo, ggaSentence);
		gpsInfoItem.longitude_deg = minmea_tocoord(&gpsGGAInfo.longitude);
		gpsInfoItem.latitude_deg = minmea_tocoord(&gpsGGAInfo.latitude);
		gpsInfoItem.altitude_m = minmea_tofloat(&gpsGGAInfo.altitude);
		gpsInfoItem.height_m = minmea_tofloat(&gpsGGAInfo.height);
		gpsInfoItem.gga_hdop = minmea_tofloat(&gpsGGAInfo.hdop);
		gpsInfoItem.gga_FixQuality = gpsGGAInfo.fix_quality;
	}

/*
	if (rmcSentence) {
		minmea_parse_rmc(&gpsRMCInfo, rmcSentence);
		// gpsInfoItem.latitude_deg = minmea_tocoord(&gpsRMCInfo.latitude);
		// gpsInfoItem.longitude_deg = minmea_tocoord(&gpsRMCInfo.longitude);
		// minmea_tofloat(&gpsRMCInfo.speed));				// Not used as it is measured in knots
		gpsInfoItem.utcDateTime.tm_sec = gpsRMCInfo.time.seconds;
		gpsInfoItem.utcDateTime.tm_min = gpsRMCInfo.time.minutes;
		gpsInfoItem.utcDateTime.tm_hour = gpsRMCInfo.time.hours;
		gpsInfoItem.utcDateTime.tm_mday = gpsRMCInfo.date.day;
		gpsInfoItem.utcDateTime.tm_mon = gpsRMCInfo.date.month;
		gpsInfoItem.utcDateTime.tm_year = gpsRMCInfo.date.year;
		gpsInfoItem.utcDateTime.tm_wday = gpsInfoItem.utcDateTime.tm_yday = -1;
		gpsInfoItem.utcDateTime.tm_gmtoff = 0;// UTC time only (Beaglebone glibc does not have gmtime())
	}
	if (vtgSentence) {
		minmea_parse_vtg(&gpsVTGInfo, vtgSentence);
		gpsInfoItem.trueTrack_deg = minmea_tocoord(
				&gpsVTGInfo.true_track_degrees);
		gpsInfoItem.magneticTrack_deg = minmea_tocoord(
				&gpsVTGInfo.magnetic_track_degrees);
		gpsInfoItem.groundSpeed_kph = minmea_tofloat(&gpsVTGInfo.speed_kph);
	}
*/
}

int gpsActive(char *serialPort) {
	ttyUSBSerialPort(&usbdev, atEnterCND, atResponse, sizeof(atEnterCND), 1000);
	ttyUSBSerialPort(&usbdev, atGPSEnable, atResponse, sizeof(atGPSEnable),
			1000);
	//	 ttyUSBSerial(atGPSSel,atResponse,sizeof(atGPSSel),1000);
	//ttyUSBSerialPort(&usbdev, atReset, atResponse, sizeof(atReset), 1000);
	ttyUSBSerialPort(&usbdev, atEnterCND, atResponse, sizeof(atEnterCND), 1000);

	ttyUSBSerialPort(&usbdev, atGPSAutoStart, atResponse,
			sizeof(atGPSAutoStart), 1000);
	ttyUSBSerialPort(&usbdev, atGPSSatInfo, atResponse, sizeof(atGPSSatInfo),
			1000);

	//ttyUSBSerialPort(serialPort, atGPSTrack, atResponse, sizeof(atGPSTrack), 1000);

	return 0;
}

GPSInfo *gpsNMeaLocation(char* serialPort) {
	if (isNMeaStart == false) {
		ttyUSBStartNmea(&usbdev, startNMEA, sizeof(startNMEA));
		isNMeaStart = true;
	}
	//ttyUSBSerialPort(serialPort, atGPSStatus, atResponse, sizeof(atGPSStatus), 1000);
	//ttyUSBSerialPort(serialPort, atGPSLoc, atResponse, sizeof(atGPSLoc), 1000);
	int nMeaLen = NMEA_MSG_LENGTH;
	uint8_t nmeaDataBuf[NMEA_MSG_LENGTH];
	bool readStatus = true;
	readStatus = ttyUSBNmeaRead(&usbdev, nmeaDataBuf, &nMeaLen);
	//cout << "Reply: " << nmeaDataBuf << endl;

	if (readStatus == true) {
		parseNMea(nmeaDataBuf, nMeaLen);
		cout << "Latitude: " << gpsInfoItem.latitude_deg << endl;
		cout << "Longitude: " << gpsInfoItem.longitude_deg << endl;
		cout << endl;

		GPSInfo *gpsInfo = new GPSInfo;
		gpsInfo = &gpsInfoItem;
		return gpsInfo;
	} else {
		cout << "NoNmea" << endl;
		return NULL;
	}
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

int gps(int locCount) {
//Read GPS via Sierra Wireless modem
	ttyUSBSerial(atEnterCND, atResponse, sizeof(atEnterCND), 1000);
	ttyUSBSerial(atGPSEnable, atResponse, sizeof(atGPSEnable), 1000);
//	 ttyUSBSerial(atGPSSel,atResponse,sizeof(atGPSSel),1000);
	ttyUSBSerial(atReset, atResponse, sizeof(atReset), 1000);
	ttyUSBSerial(atEnterCND, atResponse, sizeof(atEnterCND), 1000);

	ttyUSBSerial(atGPSAutoStart, atResponse, sizeof(atGPSAutoStart), 1000);
	ttyUSBSerial(atGPSSatInfo, atResponse, sizeof(atGPSSatInfo), 1000);

	ttyUSBSerial(atGPSTrack, atResponse, sizeof(atGPSTrack), 1000);
	while (locCount) {
		cout << "count: " << locCount << endl;
		locCount--;
		ttyUSBSerial(atGPSStatus, atResponse, sizeof(atGPSStatus), 1000);
		ttyUSBSerial(atGPSLoc, atResponse, sizeof(atGPSLoc), 1000);
	}
}

int wvdialActive(void) {
	cout << "*******************************" << endl;
	cout << "Active WVDIAL" << endl;
	cout << "*******************************" << endl;
	system("sudo wvdial &");

}

