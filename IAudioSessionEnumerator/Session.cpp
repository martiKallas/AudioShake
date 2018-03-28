#include "Session.hpp"

Session::Session() {
	this->sControl = nullptr;
	this->sControl2 = nullptr;
	this->sVolume = nullptr;
	this->sMeter = nullptr;
	isMuted = 0;
	isDimmed = 0;
	isRamping = 0;
}

Session::Session(IAudioSessionControl * sControl) {

	HRESULT hr = S_OK;
	this->sControl = sControl;
	//WINAPI interfaces
	checkHR(hr = sControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&sControl2), "Session constructor 1"); //get sessionControl2 interface
	checkHR(hr = sControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&sVolume), "Session constructor 2"); //get volume control for the session
	checkHR(hr = sControl2->QueryInterface(__uuidof(IAudioMeterInformation), (void**)&sMeter), "Session constructor 3"); //get meter for the session

	//find process name and process id
	DWORD tempID;
	hr = sControl2->GetProcessId(&tempID);
	this->processID = tempID;
	findProcessName();

	//set default values
	sVolume->GetMasterVolume(&this->defaultVolume);
	float newVol = defaultVolume*0.50;
	if (newVol > 1.0) newVol = 1.0;
	else if (newVol < 0.0) newVol = 0.0;
	dimVolume = newVol;
	rampSteps = 20;
	isMuted = 0;
	isDimmed = 0;
	isRamping = 0;
}

Session::~Session(){
}

void Session::findProcessName() {
	char buffer[MAX_PATH];
	char fileName[_MAX_FNAME];
	DWORD size = MAX_PATH;

	//get process handle
	HANDLE processHandle = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE,
		this->processID
	);

	if (processHandle) {
		QueryFullProcessImageNameA(processHandle, 0, buffer, &size);
		GetModuleBaseNameA(processHandle, NULL, buffer, size);
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char ext[_MAX_EXT];
		_splitpath_s(buffer, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);

		//GetProcessImageFileNameA(processHandle, fileName, _MAX_FNAME);
	}
	//if no handle then it is the System process
	else { this->processName = "System"; return; }

	this->processName = std::string(fileName);

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

void Session::setDim(float dim) {
	float newVol = defaultVolume*dim;
	//check float within bounds
	if (newVol > 1.0) newVol = 1.0;
	else if (newVol < 0.0) newVol = 0.0;
	dimVolume = newVol;;
}

float Session::getDim() {
	return this->dimVolume;
}

int Session::smartVolume(int mute, int dim) {
	if (mute == 1 && !isMuted) {
		//std::cout << "Volume set to 0 in session" << std::endl;
		sVolume->SetMasterVolume(0.0, NULL);
		isMuted = 1;
		isRamping = 0;
	}
	else if (mute == 0 && isMuted) {
		isMuted = 0;
		//restore original volume
		if (!isDimmed || isRamping) { 
			//std::cout << "Volume restored from mute" << std::endl;
			currentVol = dimVolume;
			sVolume->SetMasterVolume(currentVol, NULL);
			isRamping = 1;
		}
		//restore dimmed volume
		else { 
			//std::cout << "Volume restored to dim level" << std::endl;
			sVolume->SetMasterVolume(currentVol, NULL);
			isRamping = 1;
		}
	}
	if (dim == 1 && (!isDimmed || isRamping)) {
		isDimmed = 1;
		isRamping = 0;
		if (!isMuted) {
			//std::cout << "Volume dimmed" << std::endl;
			currentVol = dimVolume;
			sVolume->SetMasterVolume(currentVol, NULL);
		}
	}

	else if (dim == 0 && (isDimmed || isRamping)) {
		if (!isMuted) {
			//calculate volume change per rampSteps
			currentVol += (defaultVolume - dimVolume) / rampSteps;
			isRamping = 1;
			isDimmed = 0;
			if (currentVol > defaultVolume) {
				isRamping = 0;
				currentVol = defaultVolume;
			}
			sVolume->SetMasterVolume(currentVol, NULL);
		}
	}
	return isRamping;
}

const bool Session::sessionIDCompare(sessionID* sid) {
	if (sid->procID == processID && !processName.compare(sid->procName)) return true;
	return false;
}

void Session::setRampTime(int rampTime, int refreshTime) {
	//number of ListenLoop refreshes to get to max volume
	rampSteps = rampTime / refreshTime;
}
