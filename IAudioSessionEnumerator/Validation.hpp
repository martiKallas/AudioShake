/*********************************************************************
** Program Filename: validation.hpp
** Author: Marti Kallas
** Date: 4/28/2016
** Description: Header file for various input validation functions.
** Input: Standard input from the console (std::cin)
** Output: Returns values for expected input types
*********************************************************************/
#ifndef MK_VALIDATION_HPP
#define MK_VALIDATION_HPP

#include <string>
#include <iostream>

namespace validate {
	/*********************************************************************
	** Function: getInt
	** Description: Gets line from std::cin and ensures that the total line
	**		forms a correct integer.
	** Output: Returns input from user as integer
	*********************************************************************/
	int getInt();

	/*********************************************************************
	** Function: getIntMin
	** Description: Gets line from std::cin and ensures that the total line
	**		forms a correct integer that is larger than or equal to
	**		the specified minimum.
	** Parameters: Int specifying the minimum acceptable integer
	** Output: Returns input from user as integer
	*********************************************************************/
	int getIntMin(int min);

	/*********************************************************************
	** Function: getIntMax
	** Description: Gets line from std::cin and ensures that the total line
	**		forms a correct integer that is less than or equal to
	**		the specified maximum.
	** Parameters: Int specifying the maximum acceptable integer
	** Output: Returns input from user as integer
	*********************************************************************/
	int getIntMax(int max);

	/*********************************************************************
	** Function: getIntBetween
	** Description: Gets line from std::cin and ensures that the total line
	**		forms a correct integer that is between the specified minimum
	**		and maximum inclusive.
	** Parameters: Int specifying the maximum acceptable integer and int
	**		specifying the minimum acceptable integer.
	** Output: Returns input from user as integer
	*********************************************************************/
	int getIntBetween(int min, int max);

	/*********************************************************************
	** Function: getString
	** Description: Gets string from std::getline. Ensures string is not
	**		empty.
	** Output: Returns a string
	*********************************************************************/
	std::string getString();
}

#endif
