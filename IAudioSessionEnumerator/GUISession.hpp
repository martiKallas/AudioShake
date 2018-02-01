#pragma once
#include "Session.hpp"
#include "wx/wx.h"

class GUISession {
private:
	wxPanel* pParent;
	wxCheckBox* pKeyBox;
	wxCheckBox* pMasterBox;
	wxCheckBox* pDependentBox;
	wxStaticText* pName;
	sessionID sID;
public:
	GUISession(wxPanel* parent, sessionID sid);
	~GUISession();
	bool keyBoxChecked();
	bool masterBoxChecked();
	bool dependentBoxChecked();
	sessionID* getSessionID();
	wxStaticText* getTitle();
	wxCheckBox* getKeyBox();
	wxCheckBox* getMasterBox();
	wxCheckBox* getDependentBox();
	void disableButtons();
	void enableButtons();
};
