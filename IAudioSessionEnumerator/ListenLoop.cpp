#include "ListenLoop.hpp"

void ListenLoop::setLoopVar(int i) {
	wxCriticalSectionLocker locker(loopCritSect);
	if (i) this->loopOn = 1;
	else this->loopOn = 0;
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
	this->setLoopVar(1);
	int isMuted = 0;
	int isDimmed = 0;
	int isRamping = 0;
	int keyUp = 1;
	int keyState = 0;
	int dimOn = 0;
	//begin listen loop
	while (this->getLoopVar()) {
		//TODO: look into cleaning up conditionals for simpler logic - ideally with multi-loop
		dimOn = 0;
		if (pAudioI->dimMaxExceeded()) {
			dimOn = 1;
		}
		keyState = GetAsyncKeyState(muteKey);
		if (keyState && keyUp) {
			pParent->SetStatusText("Key Detected");
			setMutePress(1);
			keyUp = 0;
		}
		//detect when key is released
		else if (!keyState && !keyUp) keyUp = 1;

		//if there is a change to key state, update the volumes
		if (getMutePress()) {
			if (!isMuted) {
				isMuted = 1;
				pParent->SetStatusText("Muted");
			}
			else {
				isMuted = 0;
				pParent->SetStatusText("Listening...");
			}
			isRamping = pAudioI->adjustKeyDependents(isMuted, NO_CHANGE);
			setMutePress(0);
		}
		//if a "Listen To" program exceeded volume threshold, dim volume
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

wxThread::ExitCode ListenLoop::EntryHoldKey() {
	pParent->SetStatusText("Listening...");
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

void ListenLoop::setMutePress(int i) {
	wxCriticalSectionLocker locker(loopCritSect);
	mutePress = i;
}


int ListenLoop::getMutePress() {
	wxCriticalSectionLocker locker(loopCritSect);
	return mutePress;
}
