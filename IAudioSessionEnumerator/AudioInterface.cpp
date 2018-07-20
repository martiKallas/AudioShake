#include "AudioInterface.hpp"
#include <cwchar>
#include <string>



//	Title: AudioInterface
//	Description: Default constructor. Sets pointer variables = nullptr.
AudioInterface::AudioInterface() {
	this->sessionManager = nullptr;
	this->numSessions = 0;
	loadSettings();
}

//	Title: AudioInterface ~destructor
//	Description: Safely releases all interfaces, letting outside COM objects
//		know that AudioInterface is no longer using those objects.
AudioInterface::~AudioInterface() {
	safeReleaseAI(&this->sessionManager);
	sessions.clear();
}

//	TODO: update appropriate variables to class specific variables: candidates device, deviceEnumerator
HRESULT AudioInterface::initializeManager() {
	HRESULT hr = S_OK;

	//IMMDevice interface encapsulates generic featrues of a multimedia device resource. Represents an audio endpoint device.
	IMMDevice* device = nullptr;
	IMMDeviceEnumerator * deviceEnumerator = nullptr;

	//Create the device enumerator:
	//CoCreateInstance creates a single uninitialized object of the class associated with a specified CLSID.
	checkHR(hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator), "initializeMangager 1");
	//__uuidof gets the GUID ("Globally Unique Identifier") of the expression... Learn more
	//CLSCTX_ALL indicates all contexts of a class, others might be server, remote, exe... Learn more

//Get the default audio device
	checkHR(hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device), "initializeMangaer 2");

	//Get the session manager
	checkHR(hr = device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&this->sessionManager), "initializeManager 3");

	//done:
	safeReleaseAI(&deviceEnumerator);
	safeReleaseAI(&device);

	return hr;
}

/* Description: Populates the session vectors for AudioInterface
*  Preconditions: Initialize sessionManager
*/
void AudioInterface::refreshSessions(std::vector<sessionID>* sids) {

	HRESULT hr = S_OK; //HRESULT is success/failure code (0 = success) of type long
	int sessionCount = 0;
	LPWSTR session = nullptr; //LPWSTR = 32-bit pointer to a string of 16-bit Unicode chars
	IAudioSessionEnumerator* sessionEnumerator = nullptr; //to point to a session enumerator object
	IAudioSessionControl* sControl = nullptr; //to point to a session object

	sids->clear();
	sessions.clear();
	//Get the current list of sessions.
	checkHR(hr = this->sessionManager->GetSessionEnumerator(&sessionEnumerator), "refreshSessions"); //Create sessionEnumerator object at sessionEnumerator
	checkHR(hr = sessionEnumerator->GetCount(&sessionCount), "refreshSessions 2"); //Populate session count\

	for (int i = 0; i < sessionCount; i++) {
		sControl = nullptr;
		checkHR(hr = sessionEnumerator->GetSession(i, &sControl), "refreshSessions 3"); //get next session pointer
		this->sessions.push_back(Session(sControl));
		sessionID  newSID;
		newSID.procID = sessions[i].getProcessID();
		newSID.procName = sessions[i].getProcessName();
		sessions[i].setDim(settings.get("DimMult", DIM_MULT_DEFAULT));
		sessions[i].setRampTime(settings.get("RampTime", RAMP_TIME_DEFAULT), 
			settings.get("RefreshTime", REFRESH_TIME_DEFAULT));
		sids->push_back(newSID);

	}
	std::cout << "Number of items in sessions vector: " << sessions.size() << std::endl;
	safeReleaseAI(&sessionEnumerator);
}

/* Removed until needed
GUID AudioInterface::getGroupingGUID(GUID * groupingGUID, int i) {
	hr = sessionsControl[i]->GetGroupingParam(groupingGUID);
	return hr;
} */

/* preserved for potential future use
void AudioInterface::monitorMeter() {
	//TODO: may need to adjust meter based on Master Volume
	// Seems peak value is Master * Session Volume
	if (sessions.size() == 0) {
		printf_s("No session!");
		return;
	}
	AudioInterface::printSessions();
	printf_s("Please select which sessions meter you would like to monitor: ");
	int sesh = validate::getIntBetween(0, sessions.size() - 1);
	printf_s("The monitor will begin shortly, press space to exit. Please enter time between peak updates (ms).");
	DWORD timer = 50;
	timer = validate::getIntMin(1);
	int quit = 0;
	float peak = 0;
	char bar = '|';
	std::string level = "";
	while (quit == 0) {
		if (GetAsyncKeyState(VK_SPACE)) {
			quit = 1;
		}
		peak = sessions[sesh].getPeak();
		std::cout << peak;
		int i = (peak*20);
		level = std::string(i, bar) + '\n';
		printf_s(level.c_str());
		Sleep(timer);
	}
}
*/

int AudioInterface::addMuteKeyed(sessionID *sid) {
	int sessionAdded = 0;
	for (auto it = sessions.begin(); it != sessions.end() && !sessionAdded; ++it) {
		if (it->sessionIDCompare(sid)) {
			muteKeyList.push_back(&(*it));
			sessionAdded = 1;
			break;
		}
	}
	return sessionAdded;
}

 int AudioInterface::addMuteDependent(sessionID *sid) {
	int sessionAdded = 0;
	for (auto it = sessions.begin(); it != sessions.end() && !sessionAdded; ++it) {
		if (it->sessionIDCompare(sid)) {
			muteDependents.push_back(&(*it));
			sessionAdded = 1;
			break;
		}
	}
	return sessionAdded;
}

int AudioInterface::addMuteMaster(sessionID *sid) {
	int sessionAdded = 0;
	for (auto it = sessions.begin(); it != sessions.end() && !sessionAdded; ++it) {
		if (it->sessionIDCompare(sid)) {
			muteMasters.push_back(&(*it));
			sessionAdded = 1;
			break;
		}
	}
	return sessionAdded;
}
/* #### Currently saved for any reference needs
void AudioInterface::beginMuteListen() {
	int waitTime = WAIT_TIME;
	std::cout << "Once the listen loop is entered, use \\ to mute selected programs. Use F4 to exit the loop." << std::endl;
	std::cout << "Enter the sleep value in ms for the loop. Smaller is more responsive but requires more CPU. 50 is recommended." << std::endl;
	std::cout << "Enter - 1 to return to main menu." << std::endl;
	waitTime = validate::getIntBetween(-1, 100);
	if (waitTime == -1) return ;
	int quit = 0;
	int isMuted = 0;
	int isDimmed = 0;
	int isRamping = 0;
	
	int dimOn = 0;
	int muteDown = 0;
	while (!quit) {
		dimOn = 0;
		muteDown = 0;
		for (auto it = muteMasters.begin(); it != muteMasters.end(); ++it) {
			if ((*it)->getPeak() > dimThreshold) {
				dimOn = 1;
				break;
			}
		}
		if (GetAsyncKeyState(VK_DOWN)) muteDown = 1;

		//if there is a change, update the volumes
		if ((muteDown != isMuted)) {
			isMuted = muteDown;
			for(auto it = begin(this->muteKeyList); it != end(this->muteKeyList); ++it) {
				isRamping = (*it)->smartVolume(muteDown, -1);
			}
		}

		if (dimOn != isDimmed) {
			isDimmed = dimOn;
			for (auto it = muteDependents.begin(); it != muteDependents.end(); ++it) {
				isRamping = (*it)->smartVolume(-1, dimOn);
			}
		}
		else if (isRamping) {
			for (auto it = muteDependents.begin(); it != muteDependents.end(); ++it) {
				isRamping = (*it)->smartVolume(-1, dimOn);
			}
			for (auto it = begin(this->muteKeyList); it != end(this->muteKeyList); ++it) {
				//dimOn is passed in this case to continue ramping
				isRamping = (*it)->smartVolume(-1, dimOn);
			}
		}
		
		if (GetAsyncKeyState(VK_F4)) quit = 1;
		Sleep(waitTime);

	}
	restoreVolumes();
}
*/

void AudioInterface::restoreVolumes() {
	for (auto it = muteDependents.begin(); it != muteDependents.end(); ++it) {
		(*it)->restoreVolume();
	}
	for (auto it = muteKeyList.begin(); it != muteKeyList.end(); ++it) {
		(*it)->restoreVolume();
	}
}

int AudioInterface::dimMaxExceeded() {
	for (auto it = muteMasters.begin(); it != muteMasters.end(); ++it) {
		if ((*it)->getPeak() > settings.get("DimThreshold", DIM_THRESHOLD_DEFAULT)) {
			return 1;
		}
	}
	return 0;
}

int AudioInterface::adjustKeyDependents(int mute, int dim) {
	int isRamping = 0;
	for (auto it = begin(this->muteKeyList); it != end(this->muteKeyList); ++it) {
		isRamping = ( isRamping || (*it)->smartVolume(mute, dim)) ? 1 : 0;
	}
	return isRamping;
}


int AudioInterface::adjustDimDependents(int mute, int dim) {
	int isRamping = 0;
	for (auto it = muteDependents.begin(); it != muteDependents.end(); ++it) {
		isRamping = (isRamping || (*it)->smartVolume(mute, dim)) ? 1 : 0;
	}
	return isRamping;
}

void AudioInterface::clearLists() {
	muteDependents.clear();
	muteMasters.clear();
	muteKeyList.clear();
}


void AudioInterface::loadSettings() {
	if (!boost::filesystem::exists("settings.json")) {
		settings.put("RefreshTime", REFRESH_TIME_DEFAULT);
		settings.put("DimThreshold", DIM_THRESHOLD_DEFAULT);
		settings.put("DimMult", DIM_MULT_DEFAULT);
		settings.put("RampTime", RAMP_TIME_DEFAULT);
		settings.put("MuteKey", MUTE_KEY_DEFAULT);
		settings.put("StopKey", STOP_KEY_DEFAULT);
		boost::property_tree::write_json("settings.json", settings);
		return;
	}
	else {
		int changed = 0;
		boost::property_tree::read_json("settings.json", settings);
		int refreshTime = settings.get("RefreshTime", REFRESH_TIME_DEFAULT);
		if (refreshTime < REFRESH_MIN || refreshTime > REFRESH_MAX) {
			settings.put("RefreshTime", REFRESH_TIME_DEFAULT);
			changed = 1;
		}
		float dimThreshold = settings.get("DimThreshold", DIM_THRESHOLD_DEFAULT);
		if (dimThreshold < THRESHOLD_MIN || dimThreshold > THRESHOLD_MAX) {
			settings.put("DimThreshold", DIM_THRESHOLD_DEFAULT);
			changed = 1;
		}
		float dimMult = settings.get("DimMult", DIM_MULT_DEFAULT);
		if (dimMult < MULT_MIN || dimMult > MULT_MAX) {
			settings.put("DimMult", DIM_MULT_DEFAULT);
			changed = 1;
		}
		int ramp = settings.get("RampTime", RAMP_TIME_DEFAULT);
		if (ramp < RAMP_MIN || ramp > RAMP_MAX) {
			settings.put("RampTime", RAMP_TIME_DEFAULT);
			changed = 1;
		}
		int muteKey = settings.get("MuteKey", MUTE_KEY_DEFAULT);
		if (muteKey < KEY_MIN || muteKey > KEY_MAX) {
			settings.put("MuteKey", MUTE_KEY_DEFAULT);
			changed = 1;
		}
		int quitKey = settings.get("StopKey", STOP_KEY_DEFAULT);
		if (quitKey < KEY_MIN || quitKey > KEY_MAX) {
			settings.put("StopKey", STOP_KEY_DEFAULT);
			changed = 1;
		}
		if (changed) {
			boost::property_tree::write_json("settings.json", settings);
		}
	}

}

int AudioInterface::getMuteKey() {
	return settings.get("MuteKey", MUTE_KEY_DEFAULT);
}

int AudioInterface::getQuitKey() {
	return settings.get("StopKey", STOP_KEY_DEFAULT);
}

int AudioInterface::getRefreshTime() {
	return settings.get("RefreshTime", REFRESH_TIME_DEFAULT);
}
