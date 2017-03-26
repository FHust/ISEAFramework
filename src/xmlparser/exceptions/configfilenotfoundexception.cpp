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
 * configfilenotfoundexception.cpp
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifdef __EXCEPTIONS__
#include "configfilenotfoundexception.h"
#include <boost/lexical_cast.hpp>

namespace xmlparser
{

ConfigFileNotFoundException::ConfigFileNotFoundException( const char* fileName )
{
	mErrorMsg = std::string("The following file could not be read/found by the parser: ") + std::string(fileName) + std::string(".");
}

const char* ConfigFileNotFoundException::what() const throw ( )
{
	return mErrorMsg.c_str();
}

} /* namespace xmlparser */

#endif /* __EXCEPTIONS__ */
