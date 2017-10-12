#include "AudioInterface.hpp"
#include <cwchar>
#include "Validation.hpp"
#include "Menu.hpp"
#include <string>



//	Title: AudioInterface
//	Description: Default constructor. Sets pointer variables = nullptr.
AudioInterface::AudioInterface() {
	this->sessionManager = nullptr;
	this->numSessions = 0;
	this->dimThreshold = DIM_LEVEL;
}

//	Title: AudioInterface ~destructor
//	Description: Safely releases all interfaces, letting outside COM objects
//		know that AudioInterface is no longer using those objects.
AudioInterface::~AudioInterface() {
	vectorRelease(&this->sessionsControl);
	vectorRelease(&this->sessionsControl2);
	vectorRelease(&this->volumes);
	vectorRelease(&this->meters);
	safeReleaseAI(&this->sessionManager);
	sessions.clear();
}

//	TODO: update appropriate variables to class specific variables: candidates device, deviceEnumerator
HRESULT AudioInterface::initializeManager() {
	HRESULT hr = S_OK;
	this->dimThreshold = DIM_LEVEL;

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

//	Title: refreshSessions();
//	Description: Populates the session vectors for AudioInterface
//	Preconditions: Initialize sessionManager
//	Postconditions: None
//  TODO: implement with a sessionManager
//  TODO: document function
//	TODO: update appropriate variables to class specific variables
HRESULT AudioInterface::refreshSessions() {
	if (!this->sessionManager) {
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK; //HRESULT is success/failure code (0 = success) of type long
	int sessionCount = 0;
	LPWSTR session = nullptr; //LPWSTR = 32-bit pointer to a string of 16-bit Unicode chars
	IAudioSessionEnumerator* sessionEnumerator = nullptr; //to point to a session enumerator object
	IAudioSessionControl* sControl = nullptr; //to point to a session object

	sessions.clear();
	//Get the current list of sessions.
	checkHR(hr = this->sessionManager->GetSessionEnumerator(&sessionEnumerator), "refreshSessions"); //Create sessionEnumerator object at sessionEnumerator
	checkHR(hr = sessionEnumerator->GetCount(&sessionCount), "refreshSessions 2"); //Populate session count\

	for (int i = 0; i < sessionCount; i++) {
		sControl = nullptr;
		checkHR(hr = sessionEnumerator->GetSession(i, &sControl), "refreshSessions 3"); //get next session pointer
		this->sessions.push_back(Session(sControl));

	}
	std::cout << "Number of items in sessions vector: " << sessions.size() << std::endl;
	safeReleaseAI(&sessionEnumerator);
	return hr;
}

//	Title: printSessions();
//	Description: Prints the sessions in vector sessionsControl2 to std output
//	Preconditions: Initialize sessionManager... should not print if sessionsControl2 not populated
//	Postconditions: None
void AudioInterface::printSessions() {
	HRESULT hr = S_OK;
	if (sessions.size() == 0) {
		std::cout << "There are no active sessions" << std::endl;
		return;
	}
	TCHAR processName[_MAX_FNAME];
	DWORD processID;
	int i = 0;
	for (auto it = sessions.begin(); it != sessions.end(); ++it) {
			processID = it->getProcessID();
			wprintf_s(L"Process #%d Identifier: %d\n", i, processID);
			std::string processName = it->getProcessName();
			if (hr == S_OK && processName.size() < _MAX_FNAME) {
				std::cout << "Process " << i << " Exe: " << processName << std::endl;
			}
			else {
				printf_s("Process #%d Exe: no exe - likely system default\n", i);
			}
			i++;
	}//for loop
}

void AudioInterface::printVolumes() {
	int max = this->volumes.size();
	float vol = 1.f;
	BOOL mute = FALSE;
	DWORD sessionIdentifier;
	int i = 0;
	for (auto it = this->sessions.begin(); it != this->sessions.end(); ++it){	
		vol = it->getVolume();
		mute = it->getMuted();
		sessionIdentifier = it->getProcessID();
		wprintf_s(L"Session #%d Identifier: %d\n", i, sessionIdentifier);
		printf_s("Current session volume: %f. Muted: %d\n", vol, mute);
		i++;
	}//for loop
}

/* Removed until needed
GUID AudioInterface::getGroupingGUID(GUID * groupingGUID, int i) {
	hr = sessionsControl[i]->GetGroupingParam(groupingGUID);
	return hr;
} */

void AudioInterface::changeVolume() {
	if (sessions.size() == 0) {
		printf_s("No sessions!");
		return;
	}
	AudioInterface::printSessions();
	printf_s("Please select which session volume you want to change: ");
	int sesh = validate::getIntBetween(0, sessions.size() - 1);
	printf_s("Enter a new volume between 0.0 and 1.0:");
	double newVolume = validate::getDoubleBetween(0.0, 1.0);
	sessions[sesh].setVolume(newVolume);
	printf_s("Volume was set to %.3g", newVolume);
}

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

void AudioInterface::addMuteKeyed() {
	muteKeyList.clear();
	int quit = 0;
	float origin = 1.f;
	while (true) {
		printSessions();
		std::cout << "Select the number of the program to add to the mute key list or enter -1 to return to main menu." << std::endl;
		quit = validate::getIntBetween(-1, sessions.size());
		if (quit == -1) {
			break;
		}
		else {
			muteKeyList.push_back(&sessions[quit]);
			menu::clearScreen();
			std::cout << "Program " << quit << " added to mute list." << std::endl;
		}
	}
}

void AudioInterface::addMuteDependent() {
	muteDependents.clear();
	int quit = 0;
	float origin = 1.f;
	while (true) {
		printSessions();
		std::cout << "Select the number of the program to add to the mute dependent list or enter -1 to return to main menu." << std::endl;
		quit = validate::getIntBetween(-1, sessions.size());
		if (quit == -1) {
			break;
		}
		else {
			muteDependents.push_back(&sessions[quit]);
			menu::clearScreen();
			std::cout << "Program " << quit << " added to mute list." << std::endl;
		}
	}
}

void AudioInterface::addMuteMaster() {
	muteMasters.clear();
	int quit = 0;
	float origin = 1.f;
	while (true) {
		printSessions();
		std::cout << "Select the number of the program to add to the mute master list or enter -1 to return to main menu." << std::endl;
		quit = validate::getIntBetween(-1, sessions.size());
		if (quit == -1) {
			break;
		}
		else {
			muteMasters.push_back(&sessions[quit]);
			menu::clearScreen();
			std::cout << "Program " << quit << " added to mute list." << std::endl;
		}
	}
}

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

void AudioInterface::restoreVolumes() {
	for (auto it = muteDependents.begin(); it != muteDependents.end(); ++it) {
		(*it)->restoreVolume();
	}
	for (auto it = muteKeyList.begin(); it != muteKeyList.end(); ++it) {
		(*it)->restoreVolume();
	}
}
