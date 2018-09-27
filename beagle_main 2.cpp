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


//function prototype
int ttyInit(void);
int sms(void);
int gps(int locCount);
int thingspeak(void);

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
/*
 *
UART setup
*
*/
 cout << "\n***UART TEST***" << endl;

 BlackLib::BlackUART  myUart(BlackLib::UART1,
                         BlackLib::Baud9600,
                         BlackLib::ParityNo,
                         BlackLib::StopOne,
                         BlackLib::Char8 );


myUart.open( BlackLib::ReadWrite | BlackLib::NonBlock  );
if( myUart.fail() )
{
    cout << "ERROR" << endl;
}
else
{
    cout << "OK" << endl;
}


/*
 *
SPI setup
SPI pin definition on BBB clarified:

 * P9-18 GPIO_5/SPI0_D1 (MOSI)
 * P9-21 GPIO_3/SPI0_D0 (MISO)
 * P9-22 GPIO_2/SPI0_CLK

SPI SETUP for BBB

*
*/
cout << "\n***SPI TEST***" << endl;
BlackSPI mySpi(SPI0_0, 8, SpiMode0, 200000);
mySpi.open(ReadWrite | NonBlock);
cout << "SPI Is open? " << boolalpha << mySpi.isOpen() << endl;
BlackSpiProperties currentProps = mySpi.getProperties();

cout << "First bits per word size: " << (int)currentProps.spiBitsPerWord << endl
<< "First mode              : " << (int)currentProps.spiMode << endl
<< "First speed             : " << dec << currentProps.spiSpeed << endl;//

cout << "Port path: " << mySpi.getPortName() << endl << endl << endl;

/*
 *
 ADC setup
*
*/
cout <<"\n***ADC SETUP***" << endl;
BlackADC myAdc(BlackLib::AIN0);
myAdc.getNumericValue();
if( myAdc.fail() )
{
    cout << "ERROR" << endl;
}
else
{
    cout << "OK" << endl;
    cout << "Analog mV Value: " << myAdc.getNumericValue() << " mV" << endl;


}


//I2C setup



//initializing tty for Sierra Wireless
//assume running from Desktop in BBB
	system("ttyU.sh");
	ttyInit();
//sms

//	sms();

//gps
	//gps(1000);


//thingspeak
//	thingspeak();



}





