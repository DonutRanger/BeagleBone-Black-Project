/*
 * beagle_main.cpp
 *
 *  Created on: Oct 27, 2015
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

#include "BlackCore.h"
#include "BlackDef.h"
#include "BlackErr.h"
#include "BlackLib.h"

#include "BlackADC/BlackADC.h"
#include "BlackDirectory/BlackDirectory.h"
#include "BlackGPIO/BlackGPIO.h"
#include "BlackI2C/BlackI2C.h"
#include "BlackPWM/BlackPWM.h"
#include "BlackSPI/BlackSPI.h"
#include "BlackTime/BlackTime.h"
#include "BlackUART/BlackUART.h"

#include "atsmsgps.h"
#include "SocketClient.h"

using namespace std;
using namespace BlackLib;
//using namespace USM;
using namespace exploringBB;

int main() {
	GPSInfo *gpsInfo = new GPSInfo;   //GPS struct contain GPS Nmea Information.
	BlackTime myTime;
	BlackTimeElapsed et;

	ttySerialSet("ttyUSB2");
	gpsActive("ttyUSB2");
	wvdialActive();
	sleep(5);
	ttySerialSet("ttyUSB1");

	gpsInfo = gpsNMeaLocation("ttyUSB1");  //Retrieve GPS Nmea Data
	sendData(gpsInfo->latitude_deg, gpsInfo->longitude_deg);  //Send data to ThingSpeak
	myTime.start();
	for (;;) {
		et = myTime.elapsed();
		if (et.second >= 16) {
			gpsInfo = gpsNMeaLocation("ttyUSB1");  //Retrieve GPS Nmea Data
			sendData(gpsInfo->latitude_deg, gpsInfo->longitude_deg);  //Send data to ThingSpeak
			et = myTime.restart();
		}
	}
}

