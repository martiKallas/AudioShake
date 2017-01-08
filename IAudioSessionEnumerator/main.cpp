#include "AudioInterface.hpp"
#include "Menu.hpp"

void mainMenuChoice(int choice, AudioInterface* audioI);

int main() {
	HRESULT hr = S_OK;

	// Initialize COM interface
	checkHR(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED));
	AudioInterface auI;
	auI.initializeManager();

	// Create menu
	menu::Menu mainMenu;
	std::string menuOptions[4];
	menuOptions[0] = "List audio sessions.";
	menuOptions[1] = "Get Current Volume.";
	menuOptions[2] = "Change volume.";
	menuOptions[3] = "Quit.";
	mainMenu.setOptions(menuOptions, 4);

	int currentOption = 0;

	// Begin console interface
	while (currentOption != 4) {
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
		audioI->printVolumes();
		//menu::enterToContinue();
		break;
	case 3: std::cout << "Volume control in progress." << std::endl;
		//menu::enterToContinue();
		break;
	case 4: break;
	}
}