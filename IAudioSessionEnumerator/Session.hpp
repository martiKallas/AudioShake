/* Author: Marti Kallas
*  Date: 3/26/2018
*  Description: Facade class to track and interact with audio sessions
	from the Windows Core Audio API.
*/
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

//Session identifier for outside modules
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
	//not currently used
	GUID getGroupingGUID();
	DWORD getProcessID();
	float getDefaultVolume();
	void setDefaultVolume(float vol);
	void setVolume(float vol);
	//Current volume:
	float getVolume();
	//Max volume since last check:
	float getPeak();
	//Returns value of isMuted:
	BOOL getMuted();
	//Listen volume reduction multiplier:
	void setDim(float mult);
	float getDim();
	/* Description: Returns true if provided sessionID procID == this->processID
		and sessionID procName is same as this->processName
	*  Preconditions: sid must be pointer to valid sessionID.
	*/
	const bool sessionIDCompare(sessionID* sid);

	/* Description: rampTime is milliseconds from dim volume to full volume.
		refreshTime is milliseconds between ListenLoop updates.
	*  Postconditions: this->rampSteps is set to be number of ListenLoop refreshes
		to reach full volume.
	*/
	void setRampTime(int rampTime, int refreshTime);

	/* Description: Takes ints -1, 0, 1. -1 indicates no change. 0 = false, 1 = true
		Adjusts volume based on whether it is currently dimmed and/or muted
		Returns isRamping variable to indicate whether current session is
		ramping or not (1 for, 0 for not).
	*/
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
	/* Description: Populates proces name field with .exe name of process.
		Should only be called after PID is determined.
	*/
	void findProcessName();



};

