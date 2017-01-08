/*********************************************************************
** Program Filename: validation.cpp
** Author: Marti Kallas
** Date: 4/28/2016
** Description: Implementation file for various input validation functions.
** Input: Standard input from the console (std::cin)
** Output: Returns values for expected input types
*********************************************************************/
#include "Validation.hpp"

namespace validate {
	/*********************************************************************
	** Function: getInt
	** Description: Gets line from std::cin and ensures that the total line
	**		forms a correct integer.
	** Output: Returns input from user as integer
	*********************************************************************/
	int getInt() {
		std::string input;
		bool invalidInput;
		do {
			invalidInput = false;
			std::getline(std::cin, input);
			//check if input is empty
			if (input.empty()) invalidInput = true;
			//First character can be +, -, or 0-9
			else {
				char nextChar = input.at(0);
				if ((nextChar > '9' || nextChar < '0') && (nextChar != '-') && (nextChar != '+')) {
					invalidInput = true;
				}
			}
			unsigned i = 1;
			//Check that remaining characters are 0-9
			while ((i < input.length()) && !invalidInput) {
				char nextChar = input.at(i);
				if ((nextChar > '9' || nextChar < '0')) {
					invalidInput = true;
				}
				i++;
			}//while char unchecked in string
			 //Check if only - or + were entered
			if ((input.length() == 1) && (input.at(0) == '-' || input.at(0) == '+')) {
				invalidInput = true;
			}
			if (invalidInput) {
				std::cout << "Integers only, please: ";
			}
		} while (invalidInput);
		int output = std::stoi(input);
		return output;
	}

	/*********************************************************************
	** Function: getIntMin
	** Description: Gets line from std::cin and ensures that the total line
	**		forms a correct integer that is larger than or equal to
	**		the specified minimum.
	** Parameters: Int specifying the minimum acceptable integer
	** Output: Returns input from user as integer
	*********************************************************************/
	int getIntMin(int min) {
		std::string input;
		bool invalidInput;
		int output;
		do {
			invalidInput = false;
			std::getline(std::cin, input);
			//check if input is empty
			if (input.empty()) invalidInput = true;
			//First character can be +, -, or 0-9
			else {
				char nextChar = input.at(0);
				if ((nextChar > '9' || nextChar < '0') && (nextChar != '-') && (nextChar != '+')) {
					invalidInput = true;
				}
			}
			unsigned i = 1;
			//Check that remaining characters are 0-9
			while ((i < input.length()) && !invalidInput) {
				char nextChar = input.at(i);
				if ((nextChar > '9' || nextChar < '0')) {
					invalidInput = true;
				}
				i++;
			}//while char unchecked in string
			 //Check if only - or + were entered
			if ((input.length() == 1) && (input.at(0) == '-' || input.at(0) == '+')) {
				invalidInput = true;
			}
			//Check if entered integer is greater than or equal to the min parameter
			if (!invalidInput) {
				output = std::stoi(input);
				if (output < min) invalidInput = true;
			}
			if (invalidInput) {
				std::cout << "Please enter an integer larger than " << (min - 1) << ":";
			}
		} while (invalidInput);
		return output;
	}

	/*********************************************************************
	** Function: getIntMax
	** Description: Gets line from std::cin and ensures that the total line
	**		forms a correct integer that is less than or equal to
	**		the specified maximum.
	** Parameters: Int specifying the maximum acceptable integer
	** Output: Returns input from user as integer
	*********************************************************************/
	int getIntMax(int max) {
		std::string input;
		bool invalidInput;
		int output;
		do {
			invalidInput = false;
			std::getline(std::cin, input);
			//check if input is empty
			if (input.empty()) invalidInput = true;
			//First character can be +, -, or 0-9
			else {
				char nextChar = input.at(0);
				if ((nextChar > '9' || nextChar < '0') && (nextChar != '-') && (nextChar != '+')) {
					invalidInput = true;
				}
			}
			unsigned i = 1;
			//Check that remaining characters are 0-9
			while ((i < input.length()) && !invalidInput) {
				char nextChar = input.at(i);
				if ((nextChar > '9' || nextChar < '0')) {
					invalidInput = true;
				}
				i++;
			}//while char unchecked in string
			 //Check if only - or + were entered
			if ((input.length() == 1) && (input.at(0) == '-' || input.at(0) == '+')) {
				invalidInput = true;
			}
			//Check if entered integer is greater than or equal to the min parameter
			if (!invalidInput) {
				output = std::stoi(input);
				if (output > max) invalidInput = true;
			}
			if (invalidInput) {
				std::cout << "Please enter an integer less than " << (max + 1) << ":";
			}
		} while (invalidInput);
		return output;
	}

	/*********************************************************************
	** Function: getIntBetween
	** Description: Gets line from std::cin and ensures that the total line
	**		forms a correct integer that is between the specified minimum
	**		and maximum inclusive.
	** Parameters: Int specifying the maximum acceptable integer and int
	**		specifying the minimum acceptable integer.
	** Output: Returns input from user as integer
	*********************************************************************/
	int getIntBetween(int min, int max) {
		std::string input;
		bool invalidInput;
		int output;
		do {
			invalidInput = false;
			std::getline(std::cin, input);
			//check if input is empty
			if (input.empty()) invalidInput = true;
			//First character can be +, -, or 0-9
			else {
				char nextChar = input.at(0);
				if ((nextChar > '9' || nextChar < '0') && (nextChar != '-') && (nextChar != '+')) {
					invalidInput = true;
				}
			}
			unsigned i = 1;
			//Check that remaining characters are 0-9
			while ((i < input.length()) && !invalidInput) {
				char nextChar = input.at(i);
				if ((nextChar > '9' || nextChar < '0')) {
					invalidInput = true;
				}
				i++;
			}//while char unchecked in string
			 //Check if only - or + were entered
			if ((input.length() == 1) && (input.at(0) == '-' || input.at(0) == '+')) {
				invalidInput = true;
			}
			//Check if entered integer is greater than or equal to the min parameter
			if (!invalidInput) {
				output = std::stoi(input);
				if ((output > max) || (output < min)) invalidInput = true;
			}
			if (invalidInput) {
				std::cout << "Please enter an integer less than " << (max + 1) << " and greater than " << (min - 1) << ":";
			}
		} while (invalidInput);
		return output;
	}

	/*********************************************************************
	** Function: getString
	** Description: Gets string from std::getline. Ensures string is not
	**		empty.
	** Output: Returns a string
	*********************************************************************/
	std::string getString() {
		std::string input;
		bool invalidInput;
		do {
			invalidInput = false;
			std::getline(std::cin, input);
			//check if input is empty
			if (input.empty()) invalidInput = true;
			if (invalidInput) {
				std::cout << "Please enter a string: ";
			}
		} while (invalidInput);
		return input;
	}
}
