#include "Session.hpp"

Session::Session() {
	this->sControl = nullptr;
	this->sControl2 = nullptr;
	this->sVolume = nullptr;
	this->sMeter = nullptr;
	isMuted = 0;
	isDimmed = 0;
}

Session::Session(IAudioSessionControl * sControl){
	
	HRESULT hr = S_OK;
	this->sControl = sControl;
	//WINAPI interfaces
	checkHR(hr = sControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&sControl2), "Session constructor 1"); //get sessionControl2 interface
	checkHR(hr = sControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&sVolume), "Session constructor 2"); //get volume control for the session
	checkHR(hr = sControl2->QueryInterface(__uuidof(IAudioMeterInformation), (void**)&sMeter), "Session constructor 3"); //get meter for the session
	//Other useful information
	DWORD tempID;
	hr = sControl2->GetProcessId(&tempID);
	this->processID = tempID;
	char buffer[MAX_PATH];
	char fileName[_MAX_FNAME];
	hr = findProcessName(&this->processID, buffer, fileName);
	this->processName = std::string(fileName);
	sVolume->GetMasterVolume(&this->defaultVolume);
	float newVol = defaultVolume*DIM_MULT;
	if (newVol > 1.0) newVol = 1.0;
	else if (newVol < 0.0) newVol = 0.0;
	dimVolume = newVol;
	isMuted = 0;
	isDimmed = 0;
}

Session::~Session(){
}

 HRESULT Session::findProcessName(DWORD * processID, LPSTR path, LPSTR fileName) {
	if (!processID || !path) return E_INVALIDARG;
	HANDLE processHandle = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE,
		*processID
	);
	if (processHandle) {
		GetModuleBaseName(processHandle, NULL, path, MAX_PATH);
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char ext[_MAX_EXT];
		_splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);
	}
	else return E_FAIL;

	return S_OK;
}

DWORD Session::getProcessID() {
	return processID;
}

std::string Session::getProcessName() {
	return processName;
}

float Session::getVolume() {
	float vol = 0.0;
	checkHR(sVolume->GetMasterVolume(&vol), "getVolume");
	return vol;
}

BOOL Session::getMuted() {
	BOOL mute = FALSE;
	checkHR(sVolume->GetMute(&mute), "getMuted");
	return mute;
}

GUID Session::getGroupingGUID() {
	GUID groupingGUID;
	checkHR(sControl->GetGroupingParam(&groupingGUID), "getGroupingGuid");
	return groupingGUID;
}

void Session::setVolume(float vol) {
	checkHR(sVolume->SetMasterVolume(vol, NULL), "setVolume");
}

float Session::getPeak() {
	float peak = 0.0;
	sMeter->GetPeakValue(&peak);
	return peak;
}

void Session::restoreVolume() {
	checkHR(sVolume->SetMasterVolume(this->defaultVolume, NULL), "restoreVolume");
}

void Session::setDim(float dim = DIM_MULT) {
	float newVol = defaultVolume*dim;
	if (newVol > 1.0) newVol = 1.0;
	else if (newVol < 0.0) newVol = 0.0;
	dimVolume = newVol;;
}

float Session::getDim() {
	return this->dimVolume;
}

void Session::smartVolume(int mute, int dim) {
	if (mute == 1 && !isMuted) {
		std::cout << "Volume set to 0 in session" << std::endl;
		sVolume->SetMasterVolume(0.0, NULL);
		isMuted = 1;
	}
	else if (mute == 0 && isMuted) {
		isMuted = 0;
		if (!isDimmed) { //restore original volume
			std::cout << "Volume restored to full from mute in session" << std::endl;
			restoreVolume();
		}
		else { //restore dimmed volume
			std::cout << "Volume restored to dim level in session" << std::endl;
			sVolume->SetMasterVolume(dimVolume, NULL);
		}
	}
	if (dim == 1 && !isDimmed) {
		isDimmed = 1;
		if (!isMuted) {
			std::cout << "Volume dimmed in session" << std::endl;
			sVolume->SetMasterVolume(dimVolume, NULL);
		}
	}

	else if (dim == 0 && isDimmed) {
		isDimmed = 0;
		if (!isMuted) {
			std::cout << "Volume restored to full from  dim in session" << std::endl;
			restoreVolume();
		}
	}
	
}