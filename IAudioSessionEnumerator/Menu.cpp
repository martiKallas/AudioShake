/*********************************************************************
** Program Filename: menu.cpp
** Author: Marti Kallas
** Date: 6/3/2016
** Description: Implementation for menu template.
** Input:
** Output:
*********************************************************************/
#include "Menu.hpp"
#include "Validation.hpp"
#include <iostream>
#include <string>

namespace menu {
	/*********************************************************************
	** Function: clearScreen
	** Description: Clears console output
	*********************************************************************/
	void clearScreen() {
		#if defined _WIN32
			system("cls");
		#else
			system("clear");
		#endif
	}

	/*********************************************************************
	** Function: msDelay
	** Description: Pauses program action for x ms.
	** Parameters: ms to pause.
	*********************************************************************/
	void msDelay(int ms) {
		#if defined _WIN32
			Sleep(ms);
		#else
			usleep(1000 * ms);
		#endif
	}


	/*********************************************************************
	** Function: clearScreen
	** Description: Waits for user to hit enter to continue.
	*********************************************************************/
	void enterToContinue() {
		std::cout << "Hit enter to continue...";
		std::cin.ignore();
	}

	/*********************************************************************
	** Function: Menu
	** Description: Default constructor for Menu class
	** Parameters: initializes all variables to default values
	*********************************************************************/
	Menu::Menu() {
		optionList = NULL_POINT;
		listLength = 0;
	}

	/*********************************************************************
	** Function: Menu
	** Description: Constructor for Menu class
	** Parameters: *list should be an array of strings to be used as
	**		options for the menu. length gives the length of the array.
	*********************************************************************/
	Menu::Menu(std::string *list, int length) {
		this->optionList = list;
		this->listLength = length;
	}

	/*********************************************************************
	** Function: getChoice
	** Description: Displays options to the user.
	**		Uses input validation to get a single option from the user.
	**		Returns the number (starting at 1) of the selected option.
	*********************************************************************/
	int Menu::getChoice() {
		if (optionList == NULL_POINT) throw NoOptions();
		for (int i = 0; i < this->listLength; i++) {
			std::cout << i + 1 << ") " << this->optionList[i] << std::endl;
		}
		std::cout << "Please enter a number between 1 and " << this->listLength << " to indicate your selection: ";
		int choice = validate::getIntBetween(1, this->listLength);
		return choice;
	}

	/*********************************************************************
	** Function: setOptions
	** Description: Gives menu a new set of options.
	** Parameters: *list is the list of options to be displayed. newLength
	**		is the length of the option list.
	*********************************************************************/
	void Menu::setOptions(std::string *list, int newLength) {
		this->optionList = list;
		this->listLength = newLength;
	}

	/*********************************************************************
	** Function: setOption
	** Description: Allows the user to set a single option in the menu.
	** Parameters: newOption is the new text to display for that option.
	**		index is the location of the option to be replaced (start at 0).
	*********************************************************************/
	void Menu::setOption(std::string newOption, int index) {
		if ((index < 0) || (index >(this->listLength - 1))) throw IndexOutOfRange();
		this->optionList[index] = newOption;
	}
}