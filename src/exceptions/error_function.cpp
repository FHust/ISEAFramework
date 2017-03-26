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
* File Name : error_function.cpp
* Creation Date : 15-10-2015
* Last Modified : Do 15 Okt 2015 13:14:21 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "error_function.h"

// STD
#include <exception>

// ETC
#include "../misc/StrCont.h"
#include "../observer/observerException.h"
#include "../factory/exceptions/classnotfoundexception.h"
#include "../factory/exceptions/nochildrenexception.h"
#include "../factory/exceptions/classnotcachedexception.h"


template void ErrorFunction< std::runtime_error >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

template void ErrorFunction< std::out_of_range >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

template void ErrorFunction< std::range_error >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );


template void ErrorFunction< misc::StrCont >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

template void ErrorFunction< observer::CantOpenFile >( const char *fct, const int line, const char *file,
                                                       const char *XML_FEHLER_ID, ... );

template void ErrorFunction< factory::ClassNotFoundException >( const char *fct, const int line, const char *file,
                                                                const char *XML_FEHLER_ID, ... );

template void ErrorFunction< factory::NoChildrenException >( const char *fct, const int line, const char *file,
                                                             const char *XML_FEHLER_ID, ... );

template void ErrorFunction< factory::ClassNotCachedException >( const char *fct, const int line, const char *file,
                                                                 const char *XML_FEHLER_ID, ... );
