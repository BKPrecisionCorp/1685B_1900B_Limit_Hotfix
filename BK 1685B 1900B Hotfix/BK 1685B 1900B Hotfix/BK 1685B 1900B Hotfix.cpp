// BK 1685B 1900B Hotfix.cpp : This file is a quick fix for a small issue customers have been experiencing where the upper voltage and current limits
// may suddenly get misconfigured. This cannot be reset manually and must be set via computer. This fix is for people with no programming experience.
// Preconfigure: Go to project properties and give paths to the Visa include directory, Visa link directory, and Visa32.lib/Visa64.lib or else this code won't run!
#include <iostream>
#include <visa.h>			
#include <Windows.h>

unsigned char operationFlag = 0;			// 0 if no choice, 1 if 1865B, 2 if 1687B, 3 if 1688B, 4 if 1900B, 5 if 1901B, 6 if 1902B
char comPort = -1;							// communication port. -1 if no choice, will accept 0-9.

int main() {
	std::cout << "This is a Hot Fix for the BK1865B Series and the BK1900B Series Power Supplies" << std::endl;
	std::cout << "This will reset the upper voltage and current limits of the models specified below!" << std::endl;

	while (operationFlag == 0)						// User chooses which model they have, accepts values 1 - 6
	{
		std::cout << "1) 1685B" << std::endl;
		std::cout << "2) 1687B" << std::endl;
		std::cout << "3) 1688B" << std::endl;
		std::cout << "4) 1900B" << std::endl;
		std::cout << "5) 1901B" << std::endl;
		std::cout << "6) 1902B" << std::endl;
		std::cout << "Which model do you have? ";
		unsigned char choiceOP = std::cin.get();
		if (choiceOP == 49)
		{
			operationFlag = 1;
		}
		else if (choiceOP == 50)
		{
			operationFlag = 2;
		}
		else if (choiceOP == 51)
		{
			operationFlag = 3;
		}
		else if (choiceOP == 52)
		{
			operationFlag = 4;
		}
		else if (choiceOP == 53)
		{
			operationFlag = 5;
		}
		else if (choiceOP == 54)
		{
			operationFlag = 6;
		}

		else
		{
			std::cout << "Invalid Option" << std::endl;
			std::cout << " " << std::endl;
			operationFlag = 0;
		}
	}

	std::cin.clear();
	std::cout << " " << std::endl;

	while (comPort == -1)									// User select communication port, Accepts 1 - 9;
	{
		std::cout << "Which COM port? ";
		std::cin >> comPort;
		if (comPort > 57 || comPort < 48)
		{
			std::cout << "\nInvalid Option" << std::endl;
			std::cout << " " << std::endl;
			comPort = -1;
		}
	}

	std::cout << " " << std::endl;


	ViStatus status;						// ViStatus, ViSession, and ViFindList are data types. We're essentially creating variables.
											// status will hold function return values, this is uses for error checking
	ViSession defaultRM, device;			// defaultRM will hold the resource manager address and device will hold the device address.
	ViUInt32  retCount;						// this will hold how many bytes we are returned from device

	status = viOpenDefaultRM(&defaultRM);   // we open the resource manager and use status to check for errors
	if (status < VI_SUCCESS) { std::cout << "Error Opening Resource Manager: "; std::cout << status << std::endl; } // if not successful, output error and code

	char comArray[13] = { 'A', 'S', 'R', 'L', comPort, ':', ':', 'I', 'N', 'S', 'T', 'R', 0};	// constructing serial VISA resource name. Ends in 0 for null terminator.
	status = viOpen(defaultRM, comArray, VI_NULL, VI_NULL, &device);							// Opens communication with selected port. VI_NULL is a null response. 
																								// we give null responses to the inputs for access mode and timeout as 
																								// they don't matter to use currently.
	if (status < VI_SUCCESS) { std::cout << "Error Opening Communication: "; std::cout << status << std::endl;}
	

	short InterfaceType;
	status = viGetAttribute(device, VI_ATTR_INTF_TYPE, &InterfaceType);
		
	if (InterfaceType == VI_INTF_ASRL) {														// we check the interace type to see if we successfully created a serial connection
		status = viSetAttribute(device, VI_ATTR_ASRL_BAUD, 9600);
		if (status < VI_SUCCESS) { std::cout << "Error Editing Communication Attributes: "; std::cout << status << std::endl; }

		status = viSetAttribute(device, VI_ATTR_ASRL_DATA_BITS, 8);
		if (status < VI_SUCCESS) { std::cout << "Error Editing Communication Attributes: "; std::cout << status << std::endl; }

		status = viSetAttribute(device, VI_ATTR_ASRL_PARITY, VI_ASRL_PAR_NONE);
		if (status < VI_SUCCESS) { std::cout << "Error Editing Communication Attributes: "; std::cout << status << std::endl; }

		status = viSetAttribute(device, VI_ATTR_ASRL_STOP_BITS, VI_ASRL_STOP_ONE);
		if (status < VI_SUCCESS) { std::cout << "Error Editing Communication Attributes: "; std::cout << status << std::endl; }

		status = viSetAttribute(device, VI_ATTR_TMO_VALUE, 3000);
		if (status < VI_SUCCESS) { std::cout << "Error Editing Communication Attributes: "; std::cout << status << std::endl; }

		std::cout << "CONNECTION SUCCESSFUL! SENDING COMMANDS, PLEASE WAIT!" << std::endl;				// we are communicating serially, we set serial parameters and tell User to wait.
		std::cout << "This process takes around 60 seconds. Do not touch the power supply until the program exits...";
	}
	else
	{
		std::cout << "CONNECTION UNSUCCESSFUL. EXITING" << std::endl;					// we are not communicating serially, we exit. Set operation flag to 0 in order to not
		operationFlag = 0;																// send any code
	}

	// SET MODEL SPECIFIC UPPER VOLTAGE AND CURRENT LIMITS

	if (operationFlag == 1)  //1685B
	{
		status = viWrite(device, (ViConstBuf)"VOLT010\r", 8, &retCount);									// Must set voltage to around 1 Volt, these PSU's cannot
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }		// go to 0 volts and will ignore request
		Sleep(35000);
		status = viWrite(device, (ViConstBuf)"CURR000\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(20000);
		status = viWrite(device, (ViConstBuf)"SOVP600\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl;}
		Sleep(1000);
		status = viWrite(device, (ViConstBuf)"SOCP500\r", 9, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }		//1685 has 2 decimal places!
	}
	else if (operationFlag == 2) //1687B
	{
		status = viWrite(device, (ViConstBuf)"VOLT010\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(35000);
		status = viWrite(device, (ViConstBuf)"CURR000\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(20000);
		status = viWrite(device, (ViConstBuf)"SOVP360\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl;}
		Sleep(1000);
		status = viWrite(device, (ViConstBuf)"SOCP100\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
	}

	else if (operationFlag == 3) //1688B
	{
		status = viWrite(device, (ViConstBuf)"VOLT010\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(35000);
		status = viWrite(device, (ViConstBuf)"CURR000\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(20000);
		status = viWrite(device, (ViConstBuf)"SOVP180\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(1000);
		status = viWrite(device, (ViConstBuf)"SOCP200\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
	}

	else if (operationFlag == 4) //1900B
	{
		status = viWrite(device, (ViConstBuf)"VOLT010\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(35000);
		status = viWrite(device, (ViConstBuf)"CURR000\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(20000);
		status = viWrite(device, (ViConstBuf)"SOVP160\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(1000);
		status = viWrite(device, (ViConstBuf)"SOCP600\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
	}

	else if (operationFlag == 5) //1901B
	{
		status = viWrite(device, (ViConstBuf)"VOLT010\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(35000);
		status = viWrite(device, (ViConstBuf)"CURR000\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(20000);
		status = viWrite(device, (ViConstBuf)"SOVP320\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(1000);
		status = viWrite(device, (ViConstBuf)"SOCP300\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
	}

	else if (operationFlag == 6) //1902B
	{
		status = viWrite(device, (ViConstBuf)"VOLT010\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(35000);
		status = viWrite(device, (ViConstBuf)"CURR000\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(20000);
		status = viWrite(device, (ViConstBuf)"SOVP600\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
		Sleep(1000);
		status = viWrite(device, (ViConstBuf)"SOCP150\r", 8, &retCount);
		if (status < VI_SUCCESS) { std::cout << "Error Writing: "; std::cout << status << std::endl; }
	}
	
	else                                             // If not communicating serially, Fatal Error.
	{
		std::cout << " " << std::endl;
		std::cout << "FATAL ERROR" << std::endl;
	}

	std::cout << "Goodbye";
	Sleep(1000);			// THIS DELAY IS ESSENTIAL OR COMMANDS WONT WORK.
	viClose(device);
	viClose(defaultRM);
}

