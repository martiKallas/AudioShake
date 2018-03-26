#pragma once
#include "AudioInterface.hpp"
#include "wx/wx.h"
#include <vector>

const int ID_THREAD_UPDATE = 100;


class ListenLoop : public wxThread {
private:
	wxFrame * pParent;
	AudioInterface * pAudioI;
	int loopOn;
	int muteKey;
	int quitKey;
	int waitTime;
	int mutePress;
	int keyUp;
	wxCriticalSection loopCritSect;
public:
	void setLoopVar(int i);
	int getLoopVar();
	ListenLoop(wxFrame* parent, AudioInterface* auI);
	virtual wxThread::ExitCode Entry();
	virtual wxThread::ExitCode EntryHoldKey();
	void setMuteKey(int virutalKey);
	void setQuitKey(int virtualKey);
	void setRefreshTime(int waitTime);
	void setMutePress(int i);
	int getMutePress();
};