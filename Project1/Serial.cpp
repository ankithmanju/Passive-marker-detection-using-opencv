/** Serial.cpp
 *
 * A very simple serial port control class that does NOT require MFC/AFX.
 *
 * @author Hans de Ruiter
 *
 * @version 0.1 -- 28 October 2008
 */

#include <iostream>
#include <windows.h>
#define RX_BUFFSIZE 200 // big biffer size to read in all the data

using namespace std;

#include "Serial.h"

Serial::Serial(tstring &commPortName, int bitRate)
{
	commHandle = CreateFile(commPortName.c_str(), GENERIC_READ|GENERIC_WRITE, 0,NULL, OPEN_EXISTING, 
		0, NULL);

	if(commHandle == INVALID_HANDLE_VALUE) 
	{
		throw("ERROR: Could not open com port");
	}
	else 
	{
		// set timeouts
		COMMTIMEOUTS cto = { MAXDWORD, 0, 0, 0, 0};
		DCB dcb;
		if(!SetCommTimeouts(commHandle,&cto))
		{
			Serial::~Serial();
			throw("ERROR: Could not set com port time-outs");
		}

		// set DCB
		memset(&dcb,0,sizeof(dcb));
		dcb.DCBlength = sizeof(dcb);
		dcb.BaudRate = 19200;
		dcb.fBinary = 1;
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		dcb.ByteSize = 8;

		if(!SetCommState(commHandle,&dcb))
		{
			Serial::~Serial();
			throw("ERROR: Could not set com port parameters");
		}
	}
}

Serial::~Serial()
{
	CloseHandle(commHandle);
}

int Serial::write(const char *buffer)
{
	DWORD numWritten;
	WriteFile(commHandle, buffer, strlen(buffer), &numWritten, NULL); 

	return numWritten;
}

int Serial::write(const char *buffer, int buffLen)
{
	DWORD numWritten;
	WriteFile(commHandle, buffer, buffLen, &numWritten, NULL); 

	return numWritten;
}

int Serial::read(char *buffer, int buffLen, bool nullTerminate)
{
	DWORD numRead;
	if(nullTerminate)
	{
		--buffLen;
	}

	BOOL ret = ReadFile(commHandle, buffer, buffLen, &numRead, NULL);

	if(!ret)
	{
		return 0;
	}

	//if(nullTerminate)
	//{
		//buffer[numRead] = '\0';
	//}

	return numRead;
}

#define FLUSH_BUFFSIZE 10

void Serial::flush()
{
	char buffer[FLUSH_BUFFSIZE];
	int numBytes = read(buffer, FLUSH_BUFFSIZE, false);
	while(numBytes != 0)
	{
		numBytes = read(buffer, FLUSH_BUFFSIZE, false);
	}
}

int SerialDataRead(tstring Comportname)   //function which reads the data serially
{
	

		try
	{
		cout << "Opening com port"<< endl;
		tstring commPortName(Comportname);
		Serial serial(commPortName);
		cout << "Port opened" << endl;

		//cout << "writing something to the serial port" << endl;
		serial.flush();
		//char hi[] = "Hello";
		//int bytesWritten = serial.write(hi);
		//cout << bytesWritten << " bytes were written to the serial port" << endl;
		//if(bytesWritten != sizeof(hi) - 1)
		//{
		//	cout << "Writing to the serial port timed out" << endl;
		//}

		char buffer[RX_BUFFSIZE];

		cout << "Reading from the serial port: ";
		for(int i = 0; i < 10; i++)
		{
			int charsRead = serial.read(buffer, RX_BUFFSIZE);
			if(buffer[charsRead-2]=='\r' && buffer[charsRead-1]=='\n'  ) //(CharsRead ==55) is the total number of charecters in the standard bitstream.
				if(buffer[0]=='A' && buffer[1]=='u' && buffer[2]=='t')
			{
			//cout << buffer;
			string stdstring = buffer;
			std::size_t found = stdstring.find(",");

			 //erase the carecters till the first comma and store the same in stdstring
			stdstring.erase(0,found+1);

			//extract the x coordinates
			found = stdstring.find(",");
			std::string xcoordinate = stdstring.substr (0,found);
			stdstring.erase(0,found+1);
			cout<<xcoordinate<<endl; //Display x coordinate

			//extract the y coordinates
			found = stdstring.find(",");
			std::string ycoordinate = stdstring.substr (0,found);
			stdstring.erase(0,found+1);
			cout<<ycoordinate<<endl; //Display y coordinate

			//extract the z coordinates
			found = stdstring.find(",");
			std::string zcoordinate = stdstring.substr (0,found);
			stdstring.erase(0,found+1);
			cout<<zcoordinate<<endl; //Display z coordinate
			serial.flush();

			
			}
			Sleep(100);
		}
		//cout << endl;

	}catch(const char *msg)
	{
		cout << msg << endl;
	}

	cout << "press any key and enter to quit" << endl;
	char temp;
	cin >> temp;

	return 0;
}


	

