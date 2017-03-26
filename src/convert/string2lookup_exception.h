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
* File Name : string2lookup_exception.h
* Creation Date : 31-10-2012
* Last Modified : Do 20 Feb 2014 19:19:50 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _STRING2LOOKUP_EXCEPTION_
#define _STRING2LOOKUP_EXCEPTION_
// STD
#include <string>
#include <exception>

namespace convert
{
using namespace std;
/// Error class if the Lookup is not symetric e.g rows of lookup  is unequal to the rows of the data
class AsymetricLookupError : public std::exception
{
    public:
    AsymetricLookupError( std::string &whe )
        : where( whe ){};
    virtual ~AsymetricLookupError() throw(){};
    virtual const char *what() const throw()
    {
        return std::string( "Rows are having asymetric number of elements" + where ).c_str();
    }

    private:
    std::string where;
};

} /* END NAMESPACE */
#endif /* _STRING2LOOKUP_EXCEPTION_ */
