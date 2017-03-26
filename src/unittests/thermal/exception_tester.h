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
#ifndef _EXCEPTION_TESTER_
#define _EXCEPTION_TESTER_

#include <exception>
#include <cstddef>


/// Returns 0 if comparing is suceesfull
size_t CmpInit( const std::exception &e, const char *message, const char *function );

/// Returns 0 if comparing is suceesfull
size_t CmpUnexp( const std::exception &e, const char *message, const char *function );

/// Returns 0 if comparing is suceesfull
size_t CmpParam( const std::exception &e, const char *message, const char *location = 0 );

#endif
