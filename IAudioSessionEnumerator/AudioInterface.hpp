/* Author: Marti Kallas
*  Date: 12/14/2016
*  Description: Simple practice for using Windows Core Audio APIs.
*	This particular example uses the IAudioSessionEnumerator to list
*	the current audio sessions. May be expanded in the future to be
*	used with more complex programs.
*/

#pragma once
#include "Session.hpp"
#include "Settings.hpp"
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
	Settings * settings;

	std::vector<Session *> muteKeyList;
	std::vector<Session *> muteDependents;
	std::vector<Session *> muteMasters;
	/* Description: */
	void loadSettings();

public:
	AudioInterface(Settings * settings);
	AudioInterface();
	~AudioInterface();
	//must have settings set before using this function
	HRESULT initializeManager();
	void setSettings(Settings * settings);

	/* Description: Releases all current COM pointers to session interfaces and re-establishes current list
		of audio sessions with the appropriate pointers in sessionsControl and sessionsControl2
	*/
	void refreshSessions(std::vector<sessionID> *sids);

	//	Description: Gets grouping GUID for sessions... hopefully to initialize ISimpleAudioVolume
	//	Parameters: pointer to GUID to be updated, and int specifying the index in sessionsControl vector
	// ###################### REMOVED UNTIL NEEDED #####################################
	//GUID getGroupingGUID(GUID * groupingGUID, int i);

	/* Description: Work in progress, adds a programs to a list. All programs on mute list get muted on key press
		Returns 1 if session was added or 0 if session was not added
	*/
	int addMuteKeyed(sessionID* sid);

	/* Description: Adds program to list where programs on the dependent list will be muted when any program on the
		master list is producing significant volume.
		Returns 1 if session was added or 0 if session was not added
	*/
	int addMuteDependent(sessionID* sid);

	/* Description: Adds program to list where programs on the master list will be muted when any program on the
		master list is producing significant volume.
		Returns 1 if session was added or 0 if session was not added
	*/
	int addMuteMaster(sessionID* sid);
	
	/* Description: Restores volumes to defaultVolume on quit
		Calls restoreVolume() for all Sessions.
	*/
	void restoreVolumes();
	
	/* Description: Checks if any of the session in MuteMasters have exceeded the threshold.
		Returns 1 if this is the case.
	*/
	int dimMaxExceeded();

	/* Description: Calls smartVolume for muteKey Sessions. This should only be called if the
		state of the mute key has changed.
	*/
	int adjustKeyDependents(int mute, int dim);

	/* Description: Calls smartVolume for dim Sessions. This should only be called if the volumes
		are ramping up from being dimmed/muted or if the state of the Master program 
		list has changed (IE the programs in the master list have stopped or started 
		making noise above the dim threshold).
	*/
	int adjustDimDependents(int mute, int dim);
	
	/* Description: Clears muteKeyList, muteDependents and muteMasters lists. encapsulates functions
		necessary for session list refreshes and beginning loops.
	*/
	void clearLists();

	int getMuteKey();

	int getQuitKey();

	int getRefreshTime();

};



