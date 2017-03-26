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
#ifndef _STRCONT_
#define _STRCONT_

#include <boost/shared_array.hpp>
#include <vector>


class TestStrCont;

namespace misc
{

/// StrCont contains a c-style string that is persistent as long as it exists
class StrCont
{
    friend class ::TestStrCont;

    public:
    StrCont();
    ///@param[in] inputString Is copied into instance
    StrCont( const char *inputString );
    ///@param[in] inputInt Is transformed into string and copied into instance
    StrCont( const int &inputInt );
    ///@param[in] inputInt Is transformed into string and copied into instance
    StrCont( const size_t &inputInt );
    ///@param[in] inputDouble Is transformed into string and copied into instance
    StrCont( const double &inputDouble );
    void swap( StrCont &rhs );
    virtual ~StrCont();
    operator const char *() const;
    StrCont &operator+=( const StrCont &rhs );

    private:
    std::vector< char > mContent;
};

StrCont operator+( const StrCont &lhs, const StrCont &rhs );
}    // namespace misc

namespace CxxTest
{

template < typename T >
class ValueTraits;

template <>
class ValueTraits< misc::StrCont >
{
    public:
    ValueTraits( const misc::StrCont &t );
    const char *asString( void ) const;

    private:
    misc::StrCont mOutputString;
};
}

#endif
