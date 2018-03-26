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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>



//settings defaults
#define REFRESH_TIME_DEFAULT 50
#define DIM_THRESHOLD_DEFAULT 0.02
#define DIM_MULT_DEFAULT 0.50
#define RAMP_TIME_DEFAULT 1000
#define MUTE_KEY_DEFAULT VK_OEM_PERIOD
#define QUIT_KEY_DEFAULT VK_F4

//settings limits
#define REFRESH_MIN 5
#define REFRESH_MAX 500
#define THRESHOLD_MIN 0.001
#define THRESHOLD_MAX 0.99
#define MULT_MIN 0.0
#define MULT_MAX 0.99
#define RAMP_MIN 1
#define RAMP_MAX 5000
#define KEY_MIN 0x01
#define KEY_MAX 0xFE




//other
#define NO_CHANGE -1

//Safely release a COM ptr
template <class T> inline void safeReleaseAI(T **ppT) {
	if (*ppT) {
		(*ppT)->Release();
		*ppT = nullptr;
	}
}

//Safely release a vector of COM ptrs
template <class VTYPE> inline void comVectorRelease(std::vector<VTYPE> *v) {
	for (std::vector<VTYPE>::size_type i = 0; i != v->size(); i++) {
		safeReleaseAI(&(*v)[i]);
	}
	v->clear();
}

//Release all pointers in a std::vector that were dynamically allocated
template <class VTYPE> inline void vectorClear(std::vector<VTYPE> *v) {
	for (auto it = v->begin(); it != v->end(); ++it) {
		delete (*it);
	}
	v->clear();
}

class AudioInterface {
private:
	IAudioSessionManager2 * sessionManager;
	int numSessions;
	std::vector<Session>  sessions;
	boost::property_tree::ptree settings;

	std::vector<Session *> muteKeyList;
	std::vector<Session *> muteDependents;
	std::vector<Session *> muteMasters;
	std::vector<double> originalVolume;
	void loadSettings();

public:
	//	Title: AudioInterface
	//	Description: Default constructor. Sets pointer variables = nullptr.
	AudioInterface();
	~AudioInterface();
	HRESULT initializeManager();

	//	Description: Releases all current COM pointers to session interfaces and re-establishes current list
	//		of audio sessions with the appropriate pointers in sessionsControl and sessionsControl2
	void refreshSessions(std::vector<sessionID> *sids);

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

	//Description: Work in progress, adds a programs to a list. All programs on mute list get muted on key press
	//	Returns 1 if session was added or 0 if session was not added
	int addMuteKeyed(sessionID* sid);

	//Description: Adds program to list where programs on the dependent list will be muted when any program on the
	//	master list is producing significant volume.
	//	Returns 1 if session was added or 0 if session was not added
	int addMuteDependent(sessionID* sid);

	//Description: Adds program to list where programs on the master list will be muted when any program on the
	//	master list is producing significant volume.
	//	Returns 1 if session was added or 0 if session was not added
	int addMuteMaster(sessionID* sid);
	
	//Description: Restores volumes to original on quit
	void restoreVolumes();
	
	//Checks if any of the session in MuteMasters have exceeded the threshold
	//	Returns 1 if this is the case.
	int dimMaxExceeded();

	//Calls smartVolume for muteKey Sessions. This should only be called if the
	//	state of the mute key has changed.
	int adjustKeyDependents(int mute, int dim);

	//Calls smartVolume for dim Sessions. This should only be called if the volumes
	//	are ramping up from being dimmed/muted or if the state of the Master program 
	//	list has changed (IE the programs in the master list have stopped or started 
	//	making noise above the dim threshold).
	int adjustDimDependents(int mute, int dim);
	
	//Clears muteKeyList, muteDependents and muteMasters lists. encapsulates functions
	//	necessary for session list refreshes and beginning loops.
	void clearLists();

	int getMuteKey();

	int getQuitKey();

	int getRefreshTime();

};



