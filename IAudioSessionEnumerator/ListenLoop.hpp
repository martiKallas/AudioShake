/* Author: Marti Kallas
*  Date: 3/26/2018
*  Description: ListenLoop for multi-threaded monitoring of audio Session
	parameters. Keeps GUI responsive while listening for key presses or
	volume changes.
*/
#pragma once
#include "AudioInterface.hpp"
#include "Settings.hpp"
#include "wx/wx.h"
#include <vector>

const int ID_THREAD_UPDATE = 100;


class ListenLoop : public wxThread {
private:
	wxFrame * pParent;
	AudioInterface * pAudioI;
	//loop variables to track
	int loopOn;
	int muteKey;
	int stopKey;
	int waitTime;
	int mutePress;
	int keyUp;
	wxCriticalSection loopCritSect;
public:
	/* Description: Thread-safe method to set running status of listen loop.
		Listen loop will exit if setLoopVar(0). All other values are running.
	*/
	void setLoopVar(int i);
	int getLoopVar();

	/* Preconditions: parent and auI must be valid pointers
	*/
	ListenLoop(wxFrame* parent, AudioInterface* auI);

	/* Description: Use this to start the listen loop from wxFrame.
	*/
	virtual wxThread::ExitCode Entry();

	//TODO: add EntryHoldKey option
	virtual wxThread::ExitCode EntryHoldKey();

	void setMuteKey(int virutalKey);
	void setStopKey(int virtualKey);
	void setRefreshTime(int waitTime);
	//TODO: consider revising functions after determine multi-loop operation
	void setMutePress(int i);
	int getMutePress();
};