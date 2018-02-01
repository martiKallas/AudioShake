#pragma once
#include <AudioPolicy.h>
#include <Windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <iostream>
#include <Psapi.h>
#include <vector>
#include <string>
#include <stdlib.h>

//Safely release a COM ptr
template <class T> inline void safeRelease(T *ppT) {
	if (ppT) {
		ppT->Release();
	}
}


//Simple HRESULT check function
//TODO: expand checkHR for better debugging
inline void checkHR(HRESULT hres, std::string location) {
	if (hres != S_OK) {
		std::cout << "###################################       ";
		std::cout << "Error in " << location;
		std::cout << "       ###################################" << std::endl;
		std::cout << "HRESULT: " << hres << std::endl;
	}
}

struct sessionID {
	DWORD procID;
	std::string procName;
};

class Session
{
public:
	Session();
	Session(IAudioSessionControl * sControl);
	~Session();
	std::string getProcessName();
	GUID getGroupingGUID();
	DWORD getProcessID();
	float getDefaultVolume();
	void setDefaultVolume(float vol);
	void setVolume(float vol);
	float getVolume();
	float getPeak();
	BOOL getMuted();
	void setDim(float mult);
	float getDim();
	const bool sessionIDCompare(sessionID* sid);

	//Expects both settings in milliseconds setRampTIme calculates the
	//	number of ramp steps necessary given the rampTime and refreshTime
	void setRampTime(int rampTime, int refreshTime);

	//Takes ints -1, 0, 1. -1 indicates no change. 0 = false, 1 = true
	//	Adjusts volume based on whether it is currently dimmed and/or muted
	//	Returns isRamping variable to indicate whether current session is
	//	ramping or not (1 for, 0 for not).
	int smartVolume(int mute, int dim);

	//Returns the sesssion to its stored default Volume
	void restoreVolume();

private:
	DWORD processID;
	std::string processName;
	float defaultVolume;
	float dimVolume;
	int isMuted;
	int isDimmed;
	int isRamping;
	GUID groupingID;
	float currentVol;
	float rampSteps;

	//Audio Interfaces:
	IAudioSessionControl * sControl;
	IAudioSessionControl2 * sControl2;
	ISimpleAudioVolume * sVolume;
	IAudioMeterInformation * sMeter;

	//Other
	HRESULT findProcessName(DWORD * processID, LPSTR processName, LPSTR fileName, PDWORD size);



};

