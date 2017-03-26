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
/*
 * cellelementCounter.h
 *
 *  Created on: 14.01.2014
 *      Author: chris
 */

#ifndef CELLELEMENTCOUNTER_H_
#define CELLELEMENTCOUNTER_H_

#include "../electrical/twoport_withchild.h"

template < class MatrixType >
size_t CountCellelements( boost::shared_ptr< electrical::TwoPort< MatrixType > > tp )
{
    size_t count = 0;

    if ( tp->IsCellelement() )
        count = 1;

    if ( !tp->HasChildren() )
        return count;

    boost::shared_ptr< electrical::TwoPortWithChild< MatrixType > > tpwc(
     boost::static_pointer_cast< electrical::TwoPortWithChild< MatrixType > >( tp ) );

    for ( size_t i = 0; i < tpwc->size(); ++i )
        count += CountCellelements< MatrixType >( tpwc->shared_at( i ) );

    return count;
}

#endif /* CELLELEMENTCOUNTER_H_ */
