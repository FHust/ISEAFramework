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
#include "exception_tester.h"
#include <cstring>



size_t CmpInit(const std::exception &e, const char *message, const char *function)
{
    if(!strstr(e.what(), "Error in "))
        return 1;
#ifdef __EXCEPTIONS__
    if(!strstr(e.what(), "\nReport this error to the developer\n"))
#else
    if(!strstr(e.what(), "\nProgramm will be aborted\nRetry with Simulink or Executable to find out wheter this is a fatal error or just a wrong xml-input\n"))
#endif
        return 2;
    if(!strstr(e.what(), message))
        return 3;
    const char *funcPos = strstr(e.what(), function);
    if(!funcPos)
        return 4;
    return 0;
}

size_t CmpUnexp(const std::exception &e, const char *message, const char *function)
{
    if(!strstr(e.what(), "Unexpected error in function "))
        return 1;
#ifdef __EXCEPTIONS__
    if(!strstr(e.what(), "\nReport this error to the developer\n"))
#else
    if(!strstr(e.what(), "\nProgramm will be aborted\nRetry with Simulink or Executable to find out wheter this is a fatal error or just a wrong xml-input\n"))
#endif
        return 2;
    if(!strstr(e.what(), message))
        return 3;
    const char *funcPos = strstr(e.what(), function);
    if(!funcPos)
        return 4;
    return 0;
}

size_t CmpParam(const std::exception &e, const char *message, const char *location)
{
    if(!strstr(e.what(), "Mistake in parametrization of model:\n"))
        return 1;
    if(!strstr(e.what(), message))
        return 3;
    if(location)
        if(!strstr(e.what(), location))
            return 4;
    return 0;
}
