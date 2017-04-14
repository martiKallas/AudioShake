/*
* Title: Enumerator
* Author: Marti Kallas
* Date: 12/14/2016
* Description: Simple practice for using Windows Core Audio APIs.
*	This particular example uses the IAudioSessionEnumerator to list
*	the current audio sessions. May be expanded in the future to be
*	used with more complex programs.
*/

#pragma once
#include <AudioPolicy.h>
#include <Windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <iostream>
#include <Psapi.h>
#include <vector>

//Safely release a COM ptr
template <class T> inline void safeRelease(T **ppT) {
	if (*ppT) {
		(*ppT)->Release();
		*ppT = nullptr;
	}
}

//Safely release a vector of COM ptrs
template <class VTYPE> inline void vectorRelease(std::vector<VTYPE> *v) {
	for (std::vector<VTYPE>::size_type i = 0;
	i != v->size(); i++) {
		safeRelease(&(*v)[i]);
	}
	v->clear();
}

//Simple HRESULT check function
//TODO: expand checkHR for better debugging
inline void checkHR(HRESULT hres) {
	if (hres != S_OK) exit(hres);
}

class AudioInterface {
private:
	IAudioSessionManager2 * sessionManager;
	int numSessions;
	std::vector<IAudioSessionControl *> sessionsControl;
	std::vector<IAudioSessionControl2 *> sessionsControl2;
	std::vector<ISimpleAudioVolume *> volumes;

	//	Title: getProcessName
	//	Description: Gets the .exe name for a given PID
	//	Parameters: pointer to processID, address of a TCHAR buffer, length of TCHAR buffer
	//	Postconditions: None
	HRESULT getProcessName(DWORD * processID, TCHAR *processName, int bufferLength);

public:
	//	Title: AudioInterface
	//	Description: Default constructor. Sets pointer variables = nullptr.
	AudioInterface();
	~AudioInterface();
	HRESULT initializeManager();

	//	Description: Releases all current COM pointers to session interfaces and re-establishes current list
	//		of audio sessions with the appropriate pointers in sessionsControl and sessionsControl2
	HRESULT refreshSessions();

	//	Description: Prints the PID and .exe name for all of the active sessions.
	HRESULT printSessions();

	//	Description: Work in progress... want to be able to find and print volumes of all sessions.
	HRESULT printVolumes();

	//	Description: Gets grouping GUID for sessions... hopefully to initialize ISimpleAudioVolume
	//	Parameters: pointer to GUID to be updated, and int specifying the index in sessionsControl vector
	HRESULT getGroupingGUID(GUID * groupingGUID, int i);

	//	Description: Work in progress, adds a volume control interface for each session. 
	HRESULT addVolumeControl();

	// Description: Work in progress, allows the user to change the volume of a specific program
	HRESULT changeVolume();
};



