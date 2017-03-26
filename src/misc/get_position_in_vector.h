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
#ifndef _GET_POSITION_IN_VECTOR_
#define _GET_POSITION_IN_VECTOR_

#include <vector>


namespace misc
{

/// Returns the position in vec of the first element that equals val. If no element equals val, vec.size() is returned.
template < typename T >
typename std::vector< T >::size_type GetPositionInVector( const std::vector< T > &vec, const T &val )
{

    size_t j = vec.size();
    for ( size_t k = 0; k < vec.size(); ++k )
        if ( vec[k] == val )
        {
            j = k;
            break;
        }
    return j;
}
}
#endif
