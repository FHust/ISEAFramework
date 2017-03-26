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
/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : toLower.h
* Creation Date : 10-10-2013
* Last Modified : Do 10 Okt 2013 17:12:18 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TOLOWER_
#define _TOLOWER_
#include <boost/shared_array.hpp>

namespace misc
{
/// Converts the input string to a lower string which is given back as shared array
boost::shared_array< char > ToLower( const char *input );
}
#endif /* _TOLOWER_ */
