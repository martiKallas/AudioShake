/* Author: Marti Kallas
*  Date: 3/28/2018
*  Description: Module for saving, loading and manipulating settings.
*/
#pragma once

//TODO: Remove dependency on AudioInterface.hpp - bring all settings here
#include "AudioInterface.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <string>

class Settings {
private:
	boost::property_tree::ptree settings;
	std::string settingsFile;
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
};