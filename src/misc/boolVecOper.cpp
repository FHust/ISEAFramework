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
* File Name : boolVecOper.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 11:21:23 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "boolVecOper.h"
#include <cstring>

bool Any(std::vector<bool> container)
{
    for( size_t i = 0 ; i < container.size() ; ++i )
    {
        if ( container[i] )
            return true;
    }
    return false;
}

bool All(std::vector<bool> container)
{
    for( size_t i = 0 ; i < container.size() ; ++i )
    {
        if ( !container[i] )
            return false;
    }
    return true;
}

size_t Sum(std::vector<bool> container)
{
    size_t counter = 0;
    for( size_t i = 0 ; i < container.size() ; ++i )
    {
        if ( container[i] )
            ++counter;
    }
    return counter;
}
