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
#include "StrCont.h"

// STD
#include <algorithm>

// ETC
#include "macros.h"
#include "../cstring/strlcpy.h"

namespace misc
{

StrCont::StrCont() { mContent.resize( 1, '\0' ); }

StrCont::StrCont( const char *inputString )
{
    const size_t MAX_SIZE = 1024;
    const size_t sizeOfBuffer = strnlen( inputString, MAX_SIZE ) + 1;
    mContent.resize( sizeOfBuffer, '\0' );
    strlcpy( &( mContent[0] ), inputString, sizeOfBuffer );
}

StrCont::StrCont( const int &inputInt )
{
    const size_t MAX_SIZE = 1024;
    char inputString[MAX_SIZE] = {0};
    real_snprintf( inputString, MAX_SIZE, "%d", inputInt );
    const size_t sizeOfBuffer = strnlen( inputString, MAX_SIZE ) + 1;
    mContent.resize( sizeOfBuffer, '\0' );
    strlcpy( &( mContent[0] ), inputString, sizeOfBuffer );
}

StrCont::StrCont( const size_t &inputInt )
{
    const size_t MAX_SIZE = 1024;
    char inputString[MAX_SIZE] = {0};
    real_snprintf( inputString, MAX_SIZE, "%lu", inputInt );
    const size_t sizeOfBuffer = strnlen( inputString, MAX_SIZE ) + 1;
    mContent.resize( sizeOfBuffer, '\0' );
    strlcpy( &( mContent[0] ), inputString, sizeOfBuffer );
}

StrCont::StrCont( const double &inputDouble )
{
    const size_t MAX_SIZE = 1024;
    char inputString[MAX_SIZE] = {0};
    real_snprintf( inputString, MAX_SIZE, "%f", inputDouble );
    const size_t sizeOfBuffer = strnlen( inputString, MAX_SIZE ) + 1;
    mContent.resize( sizeOfBuffer, '\0' );
    strlcpy( &( mContent[0] ), inputString, sizeOfBuffer );
}

StrCont::~StrCont() {}

StrCont::operator const char *() const { return &( mContent[0] ); }

StrCont &StrCont::operator+=( const StrCont &rhs )
{
    mContent.reserve( mContent.size() + rhs.mContent.size() - 1 );
    mContent.pop_back();
    mContent.insert( mContent.end(), rhs.mContent.begin(), rhs.mContent.end() );
    return *this;
}

StrCont operator+( const StrCont &lhs, const StrCont &rhs )
{
    StrCont tmp( lhs );
    tmp += rhs;
    return tmp;
}

void StrCont::swap( StrCont &rhs ) { std::swap( mContent, rhs.mContent ); }

}    // namespace misc

namespace CxxTest
{


ValueTraits< misc::StrCont >::ValueTraits( const misc::StrCont &t )
    : mOutputString( misc::StrCont( "{ " ) + t + misc::StrCont( " }" ) )
{
}

const char *ValueTraits< misc::StrCont >::asString( void ) const { return mOutputString; }
}
