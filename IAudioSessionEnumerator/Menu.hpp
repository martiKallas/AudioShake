/*********************************************************************
** Program Filename: menu.hpp
** Author: Marti Kallas
** Date: 4/28/2016
** Description: Header for menu template
** Input:
** Output:
*********************************************************************/
#ifndef MK_MENU_HPP
#define MK_MENU_HPP

#include <string>

#if defined _WIN32
#include <windows.h>
#define NULL_POINT nullptr
#else
#include <unistd.h>
#define NULL_POINT 0
#endif

namespace menu {
	//int menu();
	void clearScreen();
	void enterToContinue();
	void msDelay(int ms);

	class Menu {
	private:
		std::string *optionList;
		int listLength;
	public:
		Menu();
		Menu(std::string *list, int length);
		void setOptions(std::string *list, int newLength);
		void setOption(std::string newOption, int index);
		int getChoice();
		class IndexOutOfRange {};
		class NoOptions {};
	};



}

#endif