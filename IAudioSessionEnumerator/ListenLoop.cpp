#include "ListenLoop.hpp"

void ListenLoop::setLoopVar(int i) {
	wxCriticalSectionLocker locker(loopCritSect);
	loopOn = i;
}

int ListenLoop::getLoopVar() {
	wxCriticalSectionLocker locker(loopCritSect);
	return loopOn;
}

ListenLoop::ListenLoop(wxFrame* parent, AudioInterface* auI) {
	this->pParent = parent;
	this->pAudioI = auI;
	waitTime = REFRESH_TIME_DEFAULT;
	muteKey = MUTE_KEY_DEFAULT;
	quitKey = QUIT_KEY_DEFAULT;
}

wxThread::ExitCode ListenLoop::Entry() {
	pParent->SetStatusText("Listening...");
	//TODO: get settings from file
	this->setLoopVar(1);
	int isMuted = 0;
	int isDimmed = 0;
	int isRamping = 0;

	int dimOn = 0;
	int muteDown = 0;
	while (this->getLoopVar()) {
		dimOn = 0;
		muteDown = 0;
		if (pAudioI->dimMaxExceeded()) {
			dimOn = 1;
		}
		if (GetAsyncKeyState(VK_DOWN)) {
			pParent->SetStatusText("Key Detected");
			muteDown = 1;
		}

		//if there is a change, update the volumes
		if ((muteDown != isMuted)) {
			isMuted = muteDown;
			isRamping = pAudioI->adjustKeyDependents(muteDown, NO_CHANGE);
			if (!isMuted) {
				pParent->SetStatusText("Listening...");
			}
		}

		if (dimOn != isDimmed) {
			isDimmed = dimOn;
			isRamping = pAudioI->adjustDimDependents(NO_CHANGE, dimOn);
			if (isDimmed) {
				pParent->SetStatusText("Dimmed");
			}
		}
		//if the volumes are ramping, adjust the volumes until they aren't ramping
		else if (isRamping) {
			isRamping = pAudioI->adjustDimDependents(NO_CHANGE, dimOn);
			isRamping = (isRamping || pAudioI->adjustKeyDependents(NO_CHANGE, dimOn)) ? 1 : 0;
			if (!isRamping) {
				pParent->SetStatusText("Listening...");
			}
		}

		if (GetAsyncKeyState(VK_F4)) this->setLoopVar(0);
		Sleep(waitTime);

	}
	pAudioI->restoreVolumes();
	pParent->SetStatusText("Loop Stopped");

	return (wxThread::ExitCode)0;
}

void ListenLoop::setMuteKey(int key) {
	muteKey = key;
	if (muteKey < KEY_MIN || muteKey > KEY_MAX) muteKey = MUTE_KEY_DEFAULT;
}

void ListenLoop::setQuitKey(int key) {
	quitKey = key;

	if (quitKey < KEY_MIN || quitKey > KEY_MAX) quitKey = MUTE_KEY_DEFAULT;
}

void ListenLoop::setRefreshTime(int time) {
	waitTime = time;
	if (waitTime < REFRESH_MIN || waitTime > REFRESH_MAX) waitTime = REFRESH_TIME_DEFAULT;
}

