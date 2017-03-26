/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.
*/
/*
 * classnotfoundexception.cpp
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifdef __EXCEPTIONS__

#include "classnotfoundexception.h"

namespace factory
{

ClassNotFoundException::ClassNotFoundException( const char* className )
{
	mErrorMsg = std::string("The Factory could not find the requested class ") + std::string(className) + std::string(".");
}

const char* ClassNotFoundException::what() const throw ( )
{
	return mErrorMsg.c_str();
}

} /* namespace factory */

#endif /* __EXCEPTIONS__ */
