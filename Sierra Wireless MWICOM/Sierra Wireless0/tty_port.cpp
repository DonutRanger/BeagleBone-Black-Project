#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>

using namespace std;

int ttyUSBSerial(uint8_t *atStr, uint8_t *response, int sendlen, int readlen) {
	int usbdev;
	uint8_t tmp[1];
	int status_flag;

	//NOTE:
	//\" is used to to indicate the next char is a legitimate text
	//ie the quotation in "all" are valid characters to be sent
	//\r is same as <cr>

	usbdev = open("/dev/ttyUSB2", O_RDWR | O_NOCTTY | O_NDELAY);
	if (usbdev == -1) {
		cout << "{ttyUSB2" << " open error" << endl;
	} else {
		cout << "Handle: " << usbdev << endl;
	}
	/*****AT command-response************************/

//	memset(response,0xff,sizeof(response));
	write(usbdev, atStr, sendlen);
	if (readlen == 0) {
		return 1;	//no response required expected.
	}
	for (int n = 0; n < readlen; n++) {
		usleep(10000);	//need time delay between reads
		read(usbdev, tmp, 1);
		response[n] = tmp[0];

		if (tmp[0] == 'O') {
			n++;
			read(usbdev, tmp, 1);
			response[n] = tmp[0];
			if (tmp[0] == 'K') {
				response[n + 1] = (char) NULL;	//terminate string
				status_flag = 0;
				break;
			}
		}
		if (tmp[0] == 'E') {
			n++;
			read(usbdev, tmp, 1);
			response[n] = tmp[0];
			if (tmp[0] == 'R') {
				n++;
				read(usbdev, tmp, 1);
				response[n] = tmp[0];
			}
			if (tmp[0] == 'R') {
				n++;
				read(usbdev, tmp, 1);
				response[n] = tmp[0];
			}
			if (tmp[0] == 'O') {
				n++;
				read(usbdev, tmp, 1);
				response[n] = tmp[0];
			}
			if (tmp[0] == 'R') {
				n++;
				read(usbdev, tmp, 1);
				response[n] = tmp[0];
				response[n + 1] = (char) NULL;
				status_flag = -1;
				break;
			}
		}
	}
	cout << "Reply: " << response << endl;
	close(usbdev);
	return status_flag;

}

int ttyUSBSerial(int *usbdev, char *serialPort) {
	string a = "/dev/";
	string path = a + serialPort;

	*usbdev = open(path.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (*usbdev == -1) {
		cout << serialPort << " open error" << endl;
		return 0;
	} else {
		cout << "Handle: " << *usbdev << endl;
		return 1;
	}
}

int ttyUSBSerialPort(int *usbdev, uint8_t *atStr, uint8_t *response,
		int sendlen, int readlen) {
	uint8_t tmp[1];
	int status_flag;

	write(*usbdev, atStr, sendlen);
	if (readlen == 0) {
		return 1;	//no response required expected.
	}
	for (int n = 0; n < readlen; n++) {
		usleep(10000);	//need time delay between reads
		read(*usbdev, tmp, 1);
		response[n] = tmp[0];

		if (tmp[0] == 'O') {
			n++;
			read(*usbdev, tmp, 1);
			response[n] = tmp[0];
			if (tmp[0] == 'K') {
				response[n + 1] = (char) NULL;	//terminate string
				status_flag = 0;
				break;
			}
		}
		if (tmp[0] == 'E') {
			n++;
			read(*usbdev, tmp, 1);
			response[n] = tmp[0];
			if (tmp[0] == 'R') {
				n++;
				read(*usbdev, tmp, 1);
				response[n] = tmp[0];
			}
			if (tmp[0] == 'R') {
				n++;
				read(*usbdev, tmp, 1);
				response[n] = tmp[0];
			}
			if (tmp[0] == 'O') {
				n++;
				read(*usbdev, tmp, 1);
				response[n] = tmp[0];
			}
			if (tmp[0] == 'R') {
				n++;
				read(*usbdev, tmp, 1);
				response[n] = tmp[0];
				response[n + 1] = (char) NULL;
				status_flag = -1;
				break;
			}
		}
	}
	cout << "Reply: " << response << endl;

	return status_flag;
}

int ttyUSBStartNmea(int *usbDevNmea, uint8_t *atStr, uint sendlen) {
	write(*usbDevNmea, atStr, sendlen);
}

int ttyUSBNmeaRead(int *usbDevNmea, uint8_t *response, int *readlen) {
	bool status_flag = true;
	if (*usbDevNmea > 0) {
		int numRead = 0;
		int tempRead = 0;
		sleep(1);	//need time delay between reads

		numRead = read(*usbDevNmea, response, *readlen);
		usleep(1000);

		*readlen = numRead;
		if (numRead > 0) {
			status_flag = true;
		} else {
			status_flag = false;
		}
	} else {
		return false;
	}
	return status_flag;
}

