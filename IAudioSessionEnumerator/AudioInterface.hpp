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
#include "Session.hpp"
#include <AudioPolicy.h>
#include <Windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <iostream>
#include <Psapi.h>
#include <vector>

#define WAIT_TIME 50
#define DIM_LEVEL 0.02

//Safely release a COM ptr
template <class T> inline void safeReleaseAI(T **ppT) {
	if (*ppT) {
		(*ppT)->Release();
		*ppT = nullptr;
	}
}

//Safely release a vector of COM ptrs
template <class VTYPE> inline void vectorRelease(std::vector<VTYPE> *v) {
	for (std::vector<VTYPE>::size_type i = 0; i != v->size(); i++) {
		safeReleaseAI(&(*v)[i]);
	}
	v->clear();
}


class AudioInterface {
private:
	IAudioSessionManager2 * sessionManager;
	int numSessions;
	std::vector<Session>  sessions;
	float dimThreshold;

	//TODO:remove these to END
	std::vector<IAudioSessionControl *> sessionsControl;
	std::vector<IAudioSessionControl2 *> sessionsControl2;
	std::vector<ISimpleAudioVolume *> volumes;
	std::vector<IAudioMeterInformation *> meters;
	//END

	std::vector<Session *> muteKeyList;
	std::vector<Session *> muteDependents;
	std::vector<Session *> muteMasters;
	std::vector<double> originalVolume;

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
	void printSessions();

	//	Description: Work in progress... want to be able to find and print volumes of all sessions.
	void printVolumes();

	//	Description: Gets grouping GUID for sessions... hopefully to initialize ISimpleAudioVolume
	//	Parameters: pointer to GUID to be updated, and int specifying the index in sessionsControl vector
	// ###################### REMOVED UNTIL NEEDED #####################################
	//GUID getGroupingGUID(GUID * groupingGUID, int i);

	// Description: Work in progress, allows the user to change the volume of a specific program
	void changeVolume();

	//Description: Work in progress, currently allows user to press enter to get a peak meter value
	void monitorMeter();

	//Description: Work in progress, adds a programs to a list. All programs on mute list get muted on key press
	void addMuteKeyed();

	//Description: Adds program to list where programs on the dependent list will be muted when any program on the
	//	master list is producing significant volume.
	void addMuteDependent();

	//Description: Adds program to list where programs on the master list will be muted when any program on the
	//	master list is producing significant volume.
	void addMuteMaster();
	
	//Description: Work in progress, currently listens for a hard-coded button press to mute programs in
	//	the mute list.
	void beginMuteListen();


	//Description: Restores volumes to original on quit
	void restoreVolumes();
	
};



