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
* File Name : macros.h
* Creation Date : 19-01-2015
* Last Modified : Di 14 Apr 2015 17:51:46 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MACROS_
#define _MACROS_

#include <cstdio>

/// In this file usefull macros are defined

/// This macro disables warnings about unused variabels and is portable. The compiler will optimize this loop
#define UNUSED( expr )    \
    do                    \
    {                     \
        ( void )( expr ); \
    } while ( 0 )

#ifdef _MSC_VER
#define real_snprintf sprintf_s
#else
#define real_snprintf snprintf
#endif

#define OUTPUT( x ) #x << "= " << ( x ) << std::endl


#endif /* _MACROS_ */
