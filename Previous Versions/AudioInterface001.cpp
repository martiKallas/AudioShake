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

//	Title: enumSessions();
//	Description: Lists audio sessions to standard output stream
//	Preconditions: Initialize sessionManager
//	Postconditions: None
//  TODO: implement with a sessionManager
//  TODO: document function
//	TODO: update appropriate variables to class specific variables
HRESULT AudioInterface::enumSessions() {
	if (!this->sessionManager) {
		return E_INVALIDARG;
	}
	
	HRESULT hr = S_OK; //HRESULT is success/failure code (0 = success) of type long

	
	int sessionCount = 0;
	LPWSTR session = nullptr; //LPWSTR = 32-bit pointer to a string of 16-bit Unicode chars

	IAudioSessionEnumerator* sessionEnumerator = nullptr; //to point to a session enumerator object
	IAudioSessionControl* sessionControl = nullptr; //to point to a session object
	IAudioSessionControl2 * sessionControl2 = nullptr; 
	DWORD processID = -1;

	//Get the current list of sessions.
	checkHR(hr = this->sessionManager->GetSessionEnumerator(&sessionEnumerator)); //Create sessionEnumerator object at sessionEnumerator

	//Get the session count.
	checkHR(hr = sessionEnumerator->GetCount(&sessionCount)); //Populate session count\

	//TODO: Loop prints multiple instances? of Windows media player... check into why
	//Print the sessions
	for (int i = 0; i < sessionCount; i++) {
		CoTaskMemFree(session); //Releases a block of memory the was assigned by CoTaskMemAlloc or CoTaskMemRealloc (thes alloc/free commands are for use with COM pointers)
		safeRelease(&sessionControl); //first time just to be sure
		safeRelease(&sessionControl2);

		checkHR(hr = sessionEnumerator->GetSession(i, &sessionControl)); //get next session pointer
		checkHR(hr = sessionControl->GetDisplayName(&session)); //get next name string
		checkHR(hr = sessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&sessionControl2)); //get sessionControl2 interface
		
		//TODO: this doesn't seem to work when checking VLC player, maybe above doesn't work?
		//		code works with the below line commented. Check to see what this does
		//checkHR(hr = sessionControl2->IsSystemSoundsSession());

		sessionControl2->GetProcessId(&processID);

		TCHAR processName[_MAX_FNAME];

		//Prints wide-character string, it is a format specific printf
		//cout is generally preferably to printf due to being type-safe, less error prone and more extensible.
		//Printf is significantly faster and may be preferable in very specific cases
		//wprintf_s(L"Session Index %d: %s\n", i, session); //this tends to be empty
		
		wprintf_s(L"Process Identifier %d: %d\n", i, processID);
		hr = getProcessName(&processID, processName, _MAX_FNAME);
		if (hr = S_OK && processName) {
			printf_s("Process Exe %d: %s\n", i, processName);
		}

	}
//TODO: probably add this stuff to the deconstructor
//done:
	CoTaskMemFree(session);
	safeRelease(&sessionControl);
	safeRelease(&sessionControl2);
	safeRelease(&sessionEnumerator);

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
		TCHAR Buffer[_MAX_FNAME];
		if (GetModuleBaseName(processHandle, NULL, Buffer, _MAX_FNAME - 1))
			printf_s("Module Base Name: %s\n", Buffer);
	}
	else return E_FAIL;

	return S_OK;
}


//	Title: refreshSessions();
//	Description: Populates the sessions
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

HRESULT AudioInterface::printSessions() {
	HRESULT hr = S_OK;
	TCHAR processName[_MAX_FNAME];
	DWORD processID;
	int max = this->sessionsControl2.size();
	for (int i = 0; i < max; i++) {
		sessionsControl2[i]->GetProcessId(&processID);
		wprintf_s(L"Process #%d Identifier: %d\n", i, processID);
		hr = getProcessName(&processID, processName, _MAX_FNAME);
		if (hr = S_OK && processName) {
			printf_s("Process #%d Exe: %s\n", i, processName);
		}
	}
	hr = S_OK;
	return hr;
}