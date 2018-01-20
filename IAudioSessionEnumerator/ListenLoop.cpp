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
}

wxThread::ExitCode ListenLoop::Entry() {
	int waitTime = WAIT_TIME;
	//TODO: get settings from file
	int quit = 0;
	int isMuted = 0;
	int isDimmed = 0;
	int isRamping = 0;

	int dimOn = 0;
	int muteDown = 0;
	while (!quit) {
		dimOn = 0;
		muteDown = 0;
		if (pAudioI->dimMaxExceeded()) {
			dimOn = 1;
		}
		if (GetAsyncKeyState(VK_DOWN)) muteDown = 1;

		//if there is a change, update the volumes
		if ((muteDown != isMuted)) {
			isMuted = muteDown;
			isRamping = pAudioI->adjustKeyDependents(muteDown, -1);
		}

		if (dimOn != isDimmed) {
			isDimmed = dimOn;
			isRamping = pAudioI->adjustDimDependents(-1, dimOn);
		}
		//if the volumes are ramping, adjust the volumes until they aren't ramping
		else if (isRamping) {
			isRamping = pAudioI->adjustDimDependents(-1, dimOn);
			isRamping = (isRamping || pAudioI->adjustKeyDependents(-1, dimOn)) ? 1 : 0;
		}

		if (GetAsyncKeyState(VK_F4)) quit = 1;
		Sleep(waitTime);

	}
	pAudioI->restoreVolumes();
}