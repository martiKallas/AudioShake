#include "AudioInterface.hpp"
#include <cwchar>


//	Title: AudioInterface
//	Description: Default constructor. Sets pointer variables = nullptr.
AudioInterface::AudioInterface() {
	this->sessionManager = nullptr;
}

AudioInterface::~AudioInterface() {
	vectorRelease(&this->sessionsControl);
	vectorRelease(&this->sessionsControl2);
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
	DWORD processID = -1;

	vectorRelease(&this->sessionsControl);
	vectorRelease(&this->sessionsControl2);
	//Get the current list of sessions.
	checkHR(hr = this->sessionManager->GetSessionEnumerator(&sessionEnumerator)); //Create sessionEnumerator object at sessionEnumerator

																				  //Get the session count.
	checkHR(hr = sessionEnumerator->GetCount(&sessionCount)); //Populate session count\

															  //TODO: Loop prints multiple instances? of Windows media player... check into why
															  //Print the sessions
	for (int i = 0; i < sessionCount; i++) {
		sControl = nullptr;
		sControl2 = nullptr;

		checkHR(hr = sessionEnumerator->GetSession(i, &sControl)); //get next session pointer
		this->sessionsControl.push_back(sControl);
		checkHR(hr = sControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&sControl2)); //get sessionControl2 interface
		this->sessionsControl2.push_back(sControl2);

	}
	if (this->sessionsControl2.size() != this->sessionsControl.size()) std::cout << "Session controls not equal." << std::endl;
	//TODO: probably add this stuff to the deconstructor
	//done:
	safeRelease(&sessionEnumerator);

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
	int max = this->sessionsControl2.size();
	for (int i = 0; i < max; i++) {
		if (sessionsControl2[i]) {
			sessionsControl2[i]->GetSessionIdentifier(&sessionIdentifier);
			wprintf_s(L"Session #%d Identifier: %d\n", i, (void **)&sessionIdentifier);
		}//if sessionsControl2[i]
	}//for loop
	CoTaskMemFree(sessionIdentifier);
	return hr;
}

HRESULT AudioInterface::getGroupingGUID(GUID * groupingGUID, int i) {
	HRESULT hr = S_OK;
	hr = sessionsControl[i]->GetGroupingParam(groupingGUID);
	return hr;
}