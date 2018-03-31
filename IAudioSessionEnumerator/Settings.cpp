#include "Settings.hpp"

Settings::Settings() {
	settingsFile = "settings.json";
	loadSettings();
}

void Settings::loadSettings() {
	if (!boost::filesystem::exists(settingsFile)) {
		settings.put("RefreshTime", REFRESH_TIME_DEFAULT);
		settings.put("DimThreshold", DIM_THRESHOLD_DEFAULT);
		settings.put("DimMult", DIM_MULT_DEFAULT);
		settings.put("RampTime", RAMP_TIME_DEFAULT);
		settings.put("MuteKey", MUTE_KEY_DEFAULT);
		settings.put("QuitKey", QUIT_KEY_DEFAULT);
		boost::property_tree::write_json("settings.json", settings);
		return;
	}
	else {
		int changed = 0;
		boost::property_tree::read_json(settingsFile, settings);
		int refreshTime = settings.get("RefreshTime", REFRESH_TIME_DEFAULT);
		if (refreshTime < REFRESH_MIN || refreshTime > REFRESH_MAX) {
			settings.put("RefreshTime", REFRESH_TIME_DEFAULT);
			changed = 1;
		}
		float dimThreshold = settings.get("DimThreshold", DIM_THRESHOLD_DEFAULT);
		if (dimThreshold < THRESHOLD_MIN || dimThreshold > THRESHOLD_MAX) {
			settings.put("DimThreshold", DIM_THRESHOLD_DEFAULT);
			changed = 1;
		}
		float dimMult = settings.get("DimMult", DIM_MULT_DEFAULT);
		if (dimMult < MULT_MIN || dimMult > MULT_MAX) {
			settings.put("DimMult", DIM_MULT_DEFAULT);
			changed = 1;
		}
		int ramp = settings.get("RampTime", RAMP_TIME_DEFAULT);
		if (ramp < RAMP_MIN || ramp > RAMP_MAX) {
			settings.put("RampTime", RAMP_TIME_DEFAULT);
			changed = 1;
		}
		int muteKey = settings.get("MuteKey", MUTE_KEY_DEFAULT);
		if (muteKey < KEY_MIN || muteKey > KEY_MAX) {
			settings.put("MuteKey", MUTE_KEY_DEFAULT);
			changed = 1;
		}
		int quitKey = settings.get("QuitKey", QUIT_KEY_DEFAULT);
		if (quitKey < KEY_MIN || quitKey > KEY_MAX) {
			settings.put("QuitKey", QUIT_KEY_DEFAULT);
			changed = 1;
		}
		if (changed) {
			boost::property_tree::write_json(settingsFile, settings);
		}
	}

}

void Settings::saveSettings() {
	int refreshTime = settings.get("RefreshTime", REFRESH_TIME_DEFAULT);
	if (refreshTime < REFRESH_MIN || refreshTime > REFRESH_MAX) {
		settings.put("RefreshTime", REFRESH_TIME_DEFAULT);
	}
	float dimThreshold = settings.get("DimThreshold", DIM_THRESHOLD_DEFAULT);
	if (dimThreshold < THRESHOLD_MIN || dimThreshold > THRESHOLD_MAX) {
		settings.put("DimThreshold", DIM_THRESHOLD_DEFAULT);
	}
	float dimMult = settings.get("DimMult", DIM_MULT_DEFAULT);
	if (dimMult < MULT_MIN || dimMult > MULT_MAX) {
		settings.put("DimMult", DIM_MULT_DEFAULT);
	}
	int ramp = settings.get("RampTime", RAMP_TIME_DEFAULT);
	if (ramp < RAMP_MIN || ramp > RAMP_MAX) {
		settings.put("RampTime", RAMP_TIME_DEFAULT);
	}
	int muteKey = settings.get("MuteKey", MUTE_KEY_DEFAULT);
	if (muteKey < KEY_MIN || muteKey > KEY_MAX) {
		settings.put("MuteKey", MUTE_KEY_DEFAULT);
	}
	int quitKey = settings.get("QuitKey", QUIT_KEY_DEFAULT);
	if (quitKey < KEY_MIN || quitKey > KEY_MAX) {
		settings.put("QuitKey", QUIT_KEY_DEFAULT);
	}
	boost::property_tree::write_json(settingsFile, settings);
}

int Settings::getMuteKey() {
	return settings.get("MuteKey", MUTE_KEY_DEFAULT);
}
int Settings::setMuteKey(int key) {
	if (key < KEY_MIN || key > KEY_MAX) return -1;
	else {
		settings.put("MuteKey", key);
		return 0;
	}

}

int Settings::getRefreshTime() {
	return settings.get("RefreshTime", REFRESH_TIME_DEFAULT);
}

int Settings::setRefreshTime(int ms){
	if (ms < REFRESH_MIN || ms > REFRESH_MAX) return -1;
	else {
		settings.put("RefreshTime", ms);
		return 0;
	}
}

int Settings::getRampTime() {
	return settings.get("RampTime", RAMP_TIME_DEFAULT);
}

int Settings::setRampTime(int ms) {
	if (ms < RAMP_MIN || ms > RAMP_MAX) return -1;
	else {
		settings.put("RampTime", ms);
		return 0;
	}
}

float Settings::getDimThreshold() {
	return settings.get("DimThreshold", DIM_THRESHOLD_DEFAULT);
}

int Settings::setDimThreshold(float pct) {
	if (pct < THRESHOLD_MIN || pct > THRESHOLD_MAX) return -1;
	else {
		settings.put("DimThreshold", pct);
		return 0;
	}
}

float Settings::getDimMult() {
	return settings.get("DimMult", DIM_MULT_DEFAULT);
}

int Settings::setDimMult(float pct) {
	if (pct < MULT_MIN || pct > MULT_MAX) return -1;
	else {
		settings.put("DimMult", pct);
		return 0;
	}
}

std::string Settings::getSettingsFile() {
	return settingsFile;
}

int Settings::setSettingsFile(std::string file) {
	if (file.length()) {
		settingsFile = file;
		return 0;
	}
	return -1;
}

//Function from wxWidgets keyboard Sample.
const char* GetVirtualKeyCodeName(int keycode)
{
	switch (keycode)
	{
#define WXK_(x) \
        case WXK_##x: return #x;

		WXK_(BACK)
			WXK_(TAB)
			WXK_(RETURN)
			WXK_(ESCAPE)
			WXK_(SPACE)
			WXK_(DELETE)
			WXK_(START)
			WXK_(LBUTTON)
			WXK_(RBUTTON)
			WXK_(CANCEL)
			WXK_(MBUTTON)
			WXK_(CLEAR)
			WXK_(SHIFT)
			WXK_(ALT)
			WXK_(CONTROL)
			WXK_(MENU)
			WXK_(PAUSE)
			WXK_(CAPITAL)
			WXK_(END)
			WXK_(HOME)
			WXK_(LEFT)
			WXK_(UP)
			WXK_(RIGHT)
			WXK_(DOWN)
			WXK_(SELECT)
			WXK_(PRINT)
			WXK_(EXECUTE)
			WXK_(SNAPSHOT)
			WXK_(INSERT)
			WXK_(HELP)
			WXK_(NUMPAD0)
			WXK_(NUMPAD1)
			WXK_(NUMPAD2)
			WXK_(NUMPAD3)
			WXK_(NUMPAD4)
			WXK_(NUMPAD5)
			WXK_(NUMPAD6)
			WXK_(NUMPAD7)
			WXK_(NUMPAD8)
			WXK_(NUMPAD9)
			WXK_(MULTIPLY)
			WXK_(ADD)
			WXK_(SEPARATOR)
			WXK_(SUBTRACT)
			WXK_(DECIMAL)
			WXK_(DIVIDE)
			WXK_(F1)
			WXK_(F2)
			WXK_(F3)
			WXK_(F4)
			WXK_(F5)
			WXK_(F6)
			WXK_(F7)
			WXK_(F8)
			WXK_(F9)
			WXK_(F10)
			WXK_(F11)
			WXK_(F12)
			WXK_(F13)
			WXK_(F14)
			WXK_(F15)
			WXK_(F16)
			WXK_(F17)
			WXK_(F18)
			WXK_(F19)
			WXK_(F20)
			WXK_(F21)
			WXK_(F22)
			WXK_(F23)
			WXK_(F24)
			WXK_(NUMLOCK)
			WXK_(SCROLL)
			WXK_(PAGEUP)
			WXK_(PAGEDOWN)
			WXK_(NUMPAD_SPACE)
			WXK_(NUMPAD_TAB)
			WXK_(NUMPAD_ENTER)
			WXK_(NUMPAD_F1)
			WXK_(NUMPAD_F2)
			WXK_(NUMPAD_F3)
			WXK_(NUMPAD_F4)
			WXK_(NUMPAD_HOME)
			WXK_(NUMPAD_LEFT)
			WXK_(NUMPAD_UP)
			WXK_(NUMPAD_RIGHT)
			WXK_(NUMPAD_DOWN)
			WXK_(NUMPAD_PAGEUP)
			WXK_(NUMPAD_PAGEDOWN)
			WXK_(NUMPAD_END)
			WXK_(NUMPAD_BEGIN)
			WXK_(NUMPAD_INSERT)
			WXK_(NUMPAD_DELETE)
			WXK_(NUMPAD_EQUAL)
			WXK_(NUMPAD_MULTIPLY)
			WXK_(NUMPAD_ADD)
			WXK_(NUMPAD_SEPARATOR)
			WXK_(NUMPAD_SUBTRACT)
			WXK_(NUMPAD_DECIMAL)
			WXK_(NUMPAD_DIVIDE)

			WXK_(WINDOWS_LEFT)
			WXK_(WINDOWS_RIGHT)
#ifdef __WXOSX__
			WXK_(RAW_CONTROL)
#endif
			WXK_(BROWSER_BACK)
			WXK_(BROWSER_FORWARD)
			WXK_(BROWSER_REFRESH)
			WXK_(BROWSER_STOP)
			WXK_(BROWSER_SEARCH)
			WXK_(BROWSER_FAVORITES)
			WXK_(BROWSER_HOME)
			WXK_(VOLUME_MUTE)
			WXK_(VOLUME_DOWN)
			WXK_(VOLUME_UP)
			WXK_(MEDIA_NEXT_TRACK)
			WXK_(MEDIA_PREV_TRACK)
			WXK_(MEDIA_STOP)
			WXK_(MEDIA_PLAY_PAUSE)
			WXK_(LAUNCH_MAIL)
			WXK_(LAUNCH_APP1)
			WXK_(LAUNCH_APP2)
#undef WXK_

	default:
		return NULL;
	}
}


//function from wxWidgets keyboard sample
//returns text description of key used in event
wxString GetKeyName(const wxKeyEvent &event)
{
	int keycode = event.GetKeyCode();
	const char* virt = GetVirtualKeyCodeName(keycode);
	if (virt)
		return virt;
	if (keycode > 0 && keycode < 32)
		return wxString::Format("Ctrl-%c", (unsigned char)('A' + keycode - 1));
	if (keycode >= 32 && keycode < 128)
		return wxString::Format("'%c'", (unsigned char)keycode);

#if wxUSE_UNICODE
	int uc = event.GetUnicodeKey();
	if (uc != WXK_NONE)
		return wxString::Format("'%c'", uc);
#endif

	return "unknown";
}

void SettingsPagePanel::OnNavKey(wxKeyEvent& event) {
	//muteKeyEntry->SetLabel(GetKeyName(event));
	//muteKeyEntry->Show();
	event.StopPropagation();
}

void SettingsPagePanel::OnKeyUp(wxKeyEvent& event) {
	muteKeyEntry->SetLabel(GetKeyName(event));
	muteKeyEntry->Show();
	event.StopPropagation();
}

void SettingsPagePanel::OnKeyDown(wxKeyEvent& event) {
	//muteKeyEntry->SetLabel(GetKeyName(event));
	//muteKeyEntry->Show();
	event.StopPropagation();
}

SettingsPagePanel::SettingsPagePanel(wxWindow* parent) : wxPanel(parent) {
		//this->settings = settings;
		//Mute Key Input:
		//TODO: possibly change NoMove class to wxWindow
		muteKeyText = new wxStaticText(this, wxID_ANY, "Mute Key: ");
		//TODO: indicate when this has focus -- maybe remove key
		muteKeyEntry = new wxTextCtrl(this, wxID_ANY, "TODO:");
		this->SetExtraStyle(wxWANTS_CHARS);
		this->Bind(wxEVT_NAVIGATION_KEY, wxKeyEventHandler(SettingsPagePanel::OnNavKey), this);
		muteKeyEntry->Bind(wxEVT_KEY_UP, wxKeyEventHandler(SettingsPagePanel::OnKeyUp), this);
		muteKeyEntry->Bind(wxEVT_KEY_DOWN, wxKeyEventHandler(SettingsPagePanel::OnKeyDown), this);
		muteEntry = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(""));
		muteEntry->Add(muteKeyText, wxSizerFlags().Expand());
		muteEntry->Add(muteKeyEntry, wxSizerFlags().Expand());
		this->SetSizerAndFit(muteEntry);
		Layout();
		//TODO: insert buttons and bind events
}

NoMoveWindow::NoMoveWindow(wxWindow* parent, wxWindowID id) : wxWindow(parent, id){
	this->SetExtraStyle(wxWS_EX_TRANSIENT);
	this->SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
}
