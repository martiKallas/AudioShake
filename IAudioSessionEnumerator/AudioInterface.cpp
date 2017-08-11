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
}

//	Title: AudioInterface ~destructor
//	Description: Safely releases all interfaces, letting outside COM objects
//		know that AudioInterface is no longer using those objects.
AudioInterface::~AudioInterface() {
	vectorRelease(&this->sessionsControl);
	vectorRelease(&this->sessionsControl2);
	vectorRelease(&this->volumes);
	vectorRelease(&this->meters);
	safeRelease(&this->sessionManager);
}

//	TODO: update appropriate variables to class specific variables: candidates device, deviceEnumerator
HRESULT AudioInterface::initializeManager() {
	HRESULT hr = S_OK;

	//IMMDevice interface encapsulates generic featrues of a multimedia device resource. Represents an audio endpoint device.
	IMMDevice* device = nullptr;
	IMMDeviceEnumerator * deviceEnumerator = nullptr;

	//Create the device enumerator:
	//CoCreateInstance creates a single uninitialized object of the class associated with a specified CLSID.
	checkHR(hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator));
			//__uuidof gets the GUID ("Globally Unique Identifier") of the expression... Learn more
			//CLSCTX_ALL indicates all contexts of a class, others might be server, remote, exe... Learn more

	//Get the default audio device
	checkHR(hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device));

	//Get the session manager
	checkHR(hr = device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&this->sessionManager));

//done:
	safeRelease(&deviceEnumerator);
	safeRelease(&device);

	return hr;
}

//	Title: getProcessName
//	Description: Gets the .exe name for a given PID
//	Parameters: pointer to processID, address of a TCHAR buffer, length of TCHAR buffer
//	Postconditions: None
HRESULT AudioInterface::getProcessName(DWORD * processID, TCHAR * processName, int maxLength) {
	if (!processID || !processName || maxLength < 1) return E_INVALIDARG;
	HANDLE processHandle = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE,
		*processID
		);
	if (processHandle) {
		GetModuleBaseName(processHandle, NULL, processName, maxLength);
	}
	else return E_FAIL;

	return S_OK;
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
	IAudioSessionControl2 * sControl2 = nullptr;
	ISimpleAudioVolume * sVolume = nullptr;
	IAudioMeterInformation * sMeter = nullptr;
	DWORD processID = -1;

	vectorRelease(&this->sessionsControl);
	vectorRelease(&this->sessionsControl2);
	vectorRelease(&this->volumes);
	vectorRelease(&this->meters);
	//Get the current list of sessions.
	checkHR(hr = this->sessionManager->GetSessionEnumerator(&sessionEnumerator)); //Create sessionEnumerator object at sessionEnumerator

																				  //Get the session count.
	checkHR(hr = sessionEnumerator->GetCount(&sessionCount)); //Populate session count\

	for (int i = 0; i < sessionCount; i++) {
		sControl = nullptr;
		sControl2 = nullptr;
		sVolume = nullptr;
		sMeter = nullptr;

		checkHR(hr = sessionEnumerator->GetSession(i, &sControl)); //get next session pointer
		this->sessionsControl.push_back(sControl);
		checkHR(hr = sControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&sControl2)); //get sessionControl2 interface
		this->sessionsControl2.push_back(sControl2);
		checkHR(hr = sControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&sVolume)); //get volume control for the session
		this->volumes.push_back(sVolume);
		checkHR(hr = sControl2->QueryInterface(__uuidof(IAudioMeterInformation), (void**)&sMeter));
		this->meters.push_back(sMeter);

	}
	if (this->sessionsControl2.size() != this->sessionsControl.size()) std::cout << "Session controls not equal." << std::endl;
	//TODO: probably add this stuff to the deconstructor
	//done:
	safeRelease(&sessionEnumerator);

	this->numSessions = sessionCount;
	return hr;
}

//	Title: printSessions();
//	Description: Prints the sessions in vector sessionsControl2 to std output
//	Preconditions: Initialize sessionManager... should not print if sessionsControl2 not populated
//	Postconditions: None
HRESULT AudioInterface::printSessions() {
	HRESULT hr = S_OK;
	TCHAR processName[_MAX_FNAME];
	DWORD processID;
	int max = this->sessionsControl2.size();
	for (int i = 0; i < max; i++) {
		if (sessionsControl2[i]) {
			sessionsControl2[i]->GetProcessId(&processID);
			wprintf_s(L"Process #%d Identifier: %d\n", i, processID);
			hr = getProcessName(&processID, processName, _MAX_FNAME);
			if (hr == S_OK && processName) {
				printf_s("Process #%d Exe: %s\n", i, processName);
			}
			else {
				printf_s("Process #%d Exe: no exe - likely system default\n", i);
			}
		}//if sessionsControl2[i]
	}//for loop
	hr = S_OK;
	return hr;
}

HRESULT AudioInterface::printVolumes() {
	HRESULT hr = S_OK;
	LPWSTR sessionIdentifier = nullptr;
	int max = this->volumes.size();
	float vol = 1.f;
	BOOL mute = FALSE;
	int i = 0;
	for (auto it = this->volumes.begin(); it != end(this->volumes); ++it){	
		(*it)->GetMasterVolume(&vol);
		(*it)->GetMute(&mute);
		sessionsControl2[i]->GetSessionIdentifier(&sessionIdentifier);
		wprintf_s(L"Session #%d Identifier: %d\n", i, (void **)&sessionIdentifier);
		printf_s("Current session volume: %f. Muted: %d\n", vol, mute);
		i++;
	}//for loop
	CoTaskMemFree(sessionIdentifier);
	return hr;
}

HRESULT AudioInterface::getGroupingGUID(GUID * groupingGUID, int i) {
	HRESULT hr = S_OK;
	hr = sessionsControl[i]->GetGroupingParam(groupingGUID);
	return hr;
}

//TODO: delete this after commit to save  
HRESULT AudioInterface::addVolumeControl() {
	HRESULT hr = S_OK;
	if (this->sessionsControl2.size() == 0) {
		printf_s("sessionControl2 is empty at addVolumeControl call.");
		return hr; 
	}
	ISimpleAudioVolume* volume = NULL;
	for (auto it = begin(this->sessionsControl2); it != end(this->sessionsControl2); ++it) {
		checkHR(hr = (*it)->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&volume));
		this->volumes.push_back(volume);

	}
	return hr;
}

HRESULT AudioInterface::changeVolume() {
	HRESULT hr = S_OK;
	if (this->numSessions == 0) {
		printf_s("No sessions!");
		return hr;
	}
	AudioInterface::printSessions();
	printf_s("Please select which session volume you want to change: ");
	int sesh = validate::getIntBetween(0, this->numSessions - 1);
	printf_s("Enter a new volume between 0.0 and 1.0:");
	double newVolume = validate::getDoubleBetween(0.0, 1.0);
	hr = this->volumes[sesh]->SetMasterVolume(newVolume, NULL);
	if (hr == S_OK) {
		printf_s("Volume was set to %.3g", newVolume);
	}
	else {
		printf_s("Volume was not correctly changed in changeVolume()");
	}
	return hr;
}

HRESULT AudioInterface::monitorMeter() {
	HRESULT hr = S_OK;
	if (this->numSessions == 0) {
		printf_s("No session!");
		return hr;
	}
	AudioInterface::printSessions();
	printf_s("Please select which sessions meter you would like to monitor: ");
	int sesh = validate::getIntBetween(0, this->numSessions - 1);
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
		this->meters[sesh]->GetPeakValue(&peak);
		int i = (peak*20);
		level = std::string(i, bar) + '\n';
		printf_s(level.c_str());
		Sleep(timer);
	}
}

HRESULT AudioInterface::addMute() {
	HRESULT hr = S_OK;
	muteList.clear();
	originalVolume.clear();
	int quit = 0;
	float origin = 1.f;
	while (true) {
		printSessions();
		std::cout << "Select the number of the program to add to the mute list or enter -1 to return to main menu." << std::endl;
		quit = validate::getIntBetween(-1, this->numSessions);
		if (quit == -1) {
			break;
		}
		else {
			muteList.push_back(quit);
			(*(this->volumes[quit])).GetMasterVolume(&origin);
			std::cout << "Stored Volume: " << origin << std::endl;
			originalVolume.push_back(origin);
			menu::clearScreen();
			std::cout << "Program " << quit << " added to mute list." << std::endl;
		}
	}

	return hr;
}

HRESULT AudioInterface::beginMuteListen() {
	HRESULT hr = S_OK;
	int waitTime = WAIT_TIME;
	std::cout << "Once the listen loop is entered, use \\ to mute selected programs. Use F4 to exit the loop." << std::endl;
	std::cout << "Enter the sleep value in ms for the loop. Smaller is more responsive but requires more CPU. Enter -1 to return to main menu." << std::endl;
	waitTime = validate::getIntBetween(-1, 100);
	if (waitTime == -1) return hr;
	int quit = 0;
	int isMuted = false;
	short keyState = 0;
	int muteLength = muteList.size();
	while (!quit) {
		if (isMuted) {
			if (!GetAsyncKeyState(VK_DECIMAL)) {//VK_OEM_5 is \ : VK_DECIMAL is numpad '.' : VK_DOWN is down arrow
				isMuted = false;
				for (int i = 0; i < muteLength; i++) {
					std::cout << "i = " << i << std::endl;
					std::cout << "Volumes? : " << originalVolume[i] << std::endl;
					hr = this->volumes[muteList[i]]->SetMasterVolume(originalVolume[i], NULL);
				}
			}

		}
		else {
			if (GetAsyncKeyState(VK_DECIMAL)) {
				isMuted = true;
				for (auto it = begin(this->muteList); it != end(this->muteList); ++it) {
					hr = this->volumes[*it]->SetMasterVolume(0.f, NULL);
				}
			}
		}
		if (GetAsyncKeyState(VK_F4)) quit = 1;
		Sleep(waitTime);

	}
	return hr;
}