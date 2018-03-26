/* Author: Marti Kallas
*  Date: 3/26/2018
*  Description: Helper class that builds each row in the listen loop
	menu based on the supplied Session.
*/
#pragma once
#include "Session.hpp"
#include "wx/wx.h"

class GUISession {
private:
	//panel to display created boxes
	wxPanel* pParent;
	//boxes to display
	wxCheckBox* pKeyBox;
	wxCheckBox* pMasterBox;
	wxCheckBox* pDependentBox;
	//Session name
	wxStaticText* pName;
	sessionID sID;
public:
	/* Description: Builds text box for name and checkboxes to 
		help creation of menu for listen loops.
	*  Preconditions: parent must be a valid wxPanel	
	*  Postconditions: boxes and name are ready to be added to parent
	*/
	GUISession(wxPanel* parent, sessionID sid);
	~GUISession();
	//Functions check if particular box is checked
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
