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