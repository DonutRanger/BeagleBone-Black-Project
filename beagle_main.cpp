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

using namespace std;
using namespace BlackLib;


int main()
{
  cout << "Hello BeagleBone Black!\n\n";
  cout << "Sample codes for GPIO and I2C\n";

  /*BlackGPIO
   *
   */
  /*Two ways of declaring a new instance of GPIO control eg. 31 ie pin P9.13
  *1. pointer - *test
  *2. reference - myGPIO
  */


  //As pointer
  BlackGPIO *test=new BlackGPIO(GPIO_31, output);
  test->setValue(low);
  cout << "GPIO_31 value: " << test->getValue() << endl;
  test->setValue(high);
  cout << "GPIO_31 value: " << test->getValue() << endl;

  //As reference
  BlackGPIO  myGpio(GPIO_31, output, SecureMode);
  //Checks GPIO
 if (myGpio.setValue(high)==true) {
      cout << "Gpio31 value: " << myGpio.getValue() << endl;
  }
  if (myGpio.setValue(low)==true) {
	  cout << "Gpio31 value: " << myGpio.getValue() << endl;
  }
  //Send 100 pulses train to Gpio31: check on oscilloscope
  cout << "\nGPIO_31 with 100 pulses. Pulse train on scope at Beaglebone header pin P9.13\n\n";
 for (int n=0;n<100;n++){
	  myGpio.setValue(high);
	  usleep(1);
	  myGpio.setValue(low);
  }

 cout << endl << "***ADC TEST***" << endl;
 BlackLib::BlackADC myAdc(BlackLib::AIN0);
 std::string val1 = myAdc.getValue();
 std::cout << "Analog mV Value: " << val1 << " mV" << endl;



 cout << endl << "***PWM TEST***"<< endl;
 BlackLib::BlackPWM myPwm(BlackLib::P8_19);
 // if new period value is less than the current duty value, the new period value setting
 // operation couldn't execute. So firstly duty value is set to zero for safe steps.
 myPwm.setDutyPercent(100.0);
 myPwm.setPeriodTime(900000);
 myPwm.setDutyPercent(50.0);
 std::string val2 = myPwm.getDutyValue();
 std::cout << "Pwm duty time: " << val2 << " nanoseconds" << endl;


}





