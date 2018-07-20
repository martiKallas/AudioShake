/* Author: Marti Kallas
*  Date: 3/28/2018
*  Description: Module for saving, loading and manipulating settings.
*  Sources:
*		wxWidgets keyboard sample: https://github.com/wxWidgets/wxWidgets/blob/v3.0.2/samples/keyboard/keyboard.cpp#L346-L485
*/
#pragma once

//TODO: Remove dependency on AudioInterface.hpp - bring all settings here
#include "AudioInterface.hpp"
#include <wx/preferences.h>
#include <wx/wx.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <string>

#define MAX_KEY_NAME 32

class Settings {
private:
	boost::property_tree::ptree settings;
	std::string settingsFile;
	wxScopedPtr<wxPreferencesEditor> settingsEditor;
public:
	Settings();
	/* Description: Loads settings from settingsFile (settingsFile defaults to "settings.json")
		If the file does not exist, it is created with default values.
	*/
	void loadSettings();
	// Saves settings to settingsFile. Writes defaults if setting is not in settings
	void saveSettings();
	//#### Set Functions Return -1 if outside of pre-defined bounds ####//
	int getMuteKey();
	int setMuteKey(int key);
	int getStopKey();
	int setStopKey(int key);
	int getRefreshTime();
	int setRefreshTime(int ms);
	int getRampTime();
	int setRampTime(int ms);
	float getDimThreshold();
	int setDimThreshold(float pct);
	float getDimMult();
	int setDimMult(float pct);
	std::string getSettingsFile();
	// TODO: figure out if this is how new files should be made/set
	//		What is the process for loading/saving from one file to next?
	int setSettingsFile(std::string file);
	void showSettings(wxWindow* parent);
};

class NoMoveWindow : public wxWindow {
public:
	NoMoveWindow(wxWindow* parent, wxWindowID);
	virtual void SetFocusFromKbd() {};
};

//TODO: consider moving this to the Settings module or another
class SettingsPagePanel : public wxPanel {
public:
	SettingsPagePanel(wxWindow *parent, Settings *settings);
	virtual void SetFocusFromKbd() {}
private:
	Settings * settings;
	wxTextCtrl* muteKeyEntry;
	wxStaticText* muteKeyText;
	wxStaticBoxSizer* muteEntry;
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnNavKey(wxKeyEvent& event);
	//TODO: add functions for value change, check value and apply
	void loadSettingsToWindow() {
		//TODO: all of this
	}
	void saveSettingsFromWindow() {
		//TODO: all of this
	}
};

//TODO: consider customizing this
class SettingsPageGeneral : public wxStockPreferencesPage {
public:
	SettingsPageGeneral(Settings* settings) : wxStockPreferencesPage(Kind_General) {
		this->settings = settings;
	}

	//only let this be called after this->settings has been activated
	virtual wxWindow* CreateWindow(wxWindow *parent) {
		return new SettingsPagePanel(parent, this->settings);
	}
	virtual void SetFocusFromkbd() {}
	void setSettings(Settings* settings);
private:
	Settings * settings;
};

//Functions from wxWidgets keyboard sample:
const char* GetVirtualKeyCodeName(int keycode);
wxString GetKeyName(const wxKeyEvent &event);

//Converts wxKeyCode to MSW virtual key number
//	As far as I can tell, virtual keys with GetAsyncKey are the only reliable
//	way to get key presses when the program is in the background while in listen
//	loop. wxWidgets key controls don't seem to be made to work in the background.
int WXtoVK(wxKeyCode i);

/* Description: Converts a virtual key code into a text string
*  Sources: https://stackoverflow.com/questions/38100667/windows-virtual-key-codes
*/
void vkToText(int vk, char name[MAX_KEY_NAME]);