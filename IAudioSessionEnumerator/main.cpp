#include "AudioInterface.hpp"
#include "Menu.hpp"

void mainMenuChoice(int choice, AudioInterface* audioI);

#define MENU_OPTIONS 9


int main() {
	HRESULT hr = S_OK;

	// Initialize COM interface
	checkHR(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED), "main");
	AudioInterface auI;
	auI.initializeManager();

	// Create menu
	menu::Menu mainMenu;
	std::string menuOptions[MENU_OPTIONS];
	menuOptions[0] = "List audio sessions.";
	menuOptions[1] = "Get Current Volume.";
	menuOptions[2] = "Change volume.";
	menuOptions[3] = "Monitor peak volume.";
	menuOptions[4] = "Bind programs to mute key.";
	menuOptions[5] = "Bind dependent programs for muting.";
	menuOptions[6] = "Bind master programs for muting.";
	menuOptions[7] = "Begin mute listen.";
	menuOptions[MENU_OPTIONS - 1] = "Quit.";
	mainMenu.setOptions(menuOptions, MENU_OPTIONS);

	int currentOption = 0;

	// Begin console interface
	while (currentOption != MENU_OPTIONS) {
		currentOption = mainMenu.getChoice();
		std::cout << std::endl;
		mainMenuChoice(currentOption, &auI);
		std::cout << std::endl;
	}
	CoUninitialize();

	//TODO: potentially use multiple threads to monitor, change volume, interact with teamspeak, etc...

	/*************************** To be notified when a new session is created: **************************************/
	// See the IAudioSessionEnumerator page
	// Some of these steps may be better suited for functions within Enumerater.cpp

	//TODO: implement the IAudioSessionNotification interface to provide the callback behavior

	//TODO: Call IAudioSessionManager2::RegisterSessionNotification to register the application's implemenation of IAudioSessionNotification

	//TODO: create and initialize the session enumerator object by calling IAudioSessionManager2::GetSessionEnumerator

	return 0;
}

void mainMenuChoice(int choice, AudioInterface* audioI) {
	switch (choice) {
	case 1:
		audioI->refreshSessions();
		audioI->printSessions();
		//menu::enterToContinue();
		break;
	case 2: std::cout << "Volume control in progress." << std::endl;
		audioI->refreshSessions();
		audioI->printVolumes();
		//menu::enterToContinue();
		break;
	case 3: std::cout << "Volume control in progress." << std::endl;
		audioI->refreshSessions();
		audioI->changeVolume();
		break;
	case 4: 
		audioI->monitorMeter();
		break;
	case 5:
		audioI->addMuteKeyed();
		break;
	case 6:
		audioI->addMuteDependent();
		break;
	case 7:
		audioI->addMuteMaster();
		break;
	case 8:
		audioI->beginMuteListen();
		break;
	case MENU_OPTIONS: 
		audioI->restoreVolumes();
		break;
	}
}