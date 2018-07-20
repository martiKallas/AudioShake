#pragma once
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/propgrid/propgrid.h>
#include <wx/preferences.h>
#include "AudioInterface.hpp"
#include "ListenLoop.hpp"
#include "GUISession.hpp"
#include "Settings.hpp"

// Should be defined in virtually every app
class MyApp : public wxApp {
public:
	virtual bool OnInit();
	void ShowSettings(wxWindow* parent);
	Settings* getSettings() { return &settings; }
private:
	wxScopedPtr<wxPreferencesEditor> m_prefEditor;
	Settings settings;
};

DECLARE_APP(MyApp)
