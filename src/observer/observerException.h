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
* File Name : observerException.h
* Creation Date : 14-09-2012
* Last Modified : Tue 13 Nov 2012 12:40:41 AM CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _OBSERVEREXCEPTION_
#define _OBSERVEREXCEPTION_
#include <string>

/// This namespace contains all observer classes
namespace observer
{
/// Error class if a file can't be accessed by the observer class
class CantOpenFile : public std::exception
{
    public:
    CantOpenFile( const char *file )
        : mFilename( file ){};
    virtual const char *what() const throw()
    {
        return std::string( std::string( "Can't open " ) + std::string( mFilename ) ).c_str();
    }

    private:
    const char *mFilename;
};
};
#endif /* _OBSERVEREXCEPTION_ */
