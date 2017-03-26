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
* File Name : charArrayCmp.cpp
* Creation Date : 10-10-2013
* Last Modified : Fr 08 Nov 2013 16:33:02 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include <cstring>
#include <algorithm>
#include "toLower.h"

#include "charArrayCmp.h"

namespace misc
{
    int CaseInsensitiveStringCompare(const char* first,  const char* second)
    {
        boost::shared_array<char> firstLower = misc::ToLower(first);
        boost::shared_array<char> secondLower = misc::ToLower(second);
        return strcmp(firstLower.get(), secondLower.get());
    }

}
