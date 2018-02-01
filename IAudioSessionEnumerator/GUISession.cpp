#include "GUISession.hpp"

GUISession::GUISession(wxPanel* parent, sessionID sid) {
	pParent = parent;
	sID = sid;
	std::string name = sid.procName;
	pName = new wxStaticText(parent, wxID_ANY, name);
	pKeyBox = new wxCheckBox(parent, wxID_ANY, wxT(""));
	pMasterBox = new wxCheckBox(parent, wxID_ANY, wxT(""));
	pDependentBox = new wxCheckBox(parent, wxID_ANY, wxT(""));
}

GUISession::~GUISession() {
	pName->Destroy();
	pKeyBox->Destroy();
	pMasterBox->Destroy();
	pDependentBox->Destroy();
}

wxStaticText* GUISession::getTitle() {
	return pName;
}
wxCheckBox* GUISession::getKeyBox() {
	return pKeyBox;
}
wxCheckBox* GUISession::getMasterBox() {
	return pMasterBox;
}
wxCheckBox* GUISession::getDependentBox() {
	return pDependentBox;
}

bool GUISession::keyBoxChecked() {
	return pKeyBox->IsChecked();
}

bool GUISession::masterBoxChecked() {
	return pMasterBox->IsChecked();
}

bool GUISession::dependentBoxChecked() {
	return pDependentBox->IsChecked();
}

sessionID* GUISession::getSessionID() {
	return &sID;
}
