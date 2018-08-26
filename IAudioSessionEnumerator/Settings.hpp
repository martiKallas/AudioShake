/* Author: Marti Kallas
*  Date: 3/28/2018
*  Description: Module for saving, loading and manipulating settings.
*  Sources:
*		wxWidgets keyboard sample: https://github.com/wxWidgets/wxWidgets/blob/v3.0.2/samples/keyboard/keyboard.cpp#L346-L485
*/
#pragma once

//TODO: Remove dependency on AudioInterface.hpp - bring all settings here
#include <wx/preferences.h>
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <string>


//settings defaults
#define REFRESH_TIME_DEFAULT 50
#define DIM_THRESHOLD_DEFAULT 0.02
#define DIM_MULT_DEFAULT 0.50
#define RAMP_TIME_DEFAULT 1000
#define MUTE_KEY_DEFAULT VK_OEM_PERIOD
#define STOP_KEY_DEFAULT VK_F4

//settings limits
#define REFRESH_MIN 5
#define REFRESH_MAX 500
#define THRESHOLD_MIN 0.001
#define THRESHOLD_MAX 0.99
#define MULT_MIN 0.0
#define MULT_MAX 0.99
#define RAMP_MIN 1
#define RAMP_MAX 5000
#define KEY_MIN 0x01
#define KEY_MAX 0xFE

#define MAX_KEY_NAME 32

/**************** Settings Units and Descriptions: ***************
	# Refresh Time: (int) milliseconds that loop will sleep before checking for new actions
	# Dim Threshold: (float) float between 0 and 1 that determines how loud a source must
		be to trigger the dimming action.
	# Dim Mult: (float) during dimming action, volume will be set to this value times the
		default volume. Should be a float between 0 and 1.
	# Ramp Time: (int) milliseconds from when dimming action ends to when volume is restored
		to default volume. Volume will gradually increase over this duration to make
		volume restoration less jarring.
	# Mute Key: (SHORT) Microsoft VirtualKey that is a SHORT. When this key is pressed, any 
		programs assigned to the mute key will be muted. Overrides dimming.
	# Stop Key: (SHORT) Microsoft VirtualKey that is a SHORT. Stops the listen loop on key press.

*/

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
	SHORT getMuteKey();
	int setMuteKey(SHORT key);
	SHORT getStopKey();
	int setStopKey(SHORT key);
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

//TODO: consider moving this to the Settings module or another
class SettingsPagePanel : public wxPanel {
public:
	SettingsPagePanel(wxWindow *parent, Settings *settings);
	virtual void SetFocusFromKbd() {}
	virtual bool TransferDataFromWindow();
private:
	Settings * settings;
	wxTextCtrl* muteKeyEntry;
	wxTextCtrl* stopKeyEntry;
	wxSpinCtrl* dimPctCtrl;
	wxSpinCtrl* dimThreshCtrl;
	wxSpinCtrl* rampTimeCtrl;
	wxSpinCtrl* refreshTimeCtrl;
	//note: muteKeyValue and stopKeyValue may be different from stored settings
	SHORT muteKeyValue;
	SHORT stopKeyValue;
	wxStaticText* muteKeyText;
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
	/*
	//The below function stops navigation keys from moving from setting to setting
	//Was used for hotkey entry
	virtual void SetFocusFromkbd() {}
	*/
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
SHORT WXtoVK(wxKeyCode i);

/* Description: Converts a virtual key code into a text string
*  Sources: https://stackoverflow.com/questions/38100667/windows-virtual-key-codes
*/
void vkToText(SHORT vk, char name[MAX_KEY_NAME]);