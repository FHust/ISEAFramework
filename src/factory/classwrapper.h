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
 * factorywrapper.h
 *
 *  Created on: 12.09.2013
 *      Author: chris
 */

#ifndef FACTORYWRAPPER_H_
#define FACTORYWRAPPER_H_

#include "../xmlparser/xmlparameter.h"
#include "../misc/cmp_str.h"
#include "../misc/StrCont.h"
#include "../states/thermal_state.h"
#include <map>
#include <boost/shared_ptr.hpp>

namespace factory
{
/// Contains additional information created during run time for the factory
struct ArgumentType
{
};
typedef std::map< misc::StrCont, double, misc::cmp_str >::const_iterator DoubleMapConstIterator;

/// This class is a base class for all classwrappers. A classwrapper is able to create an instance of a particular class
/// which is derived from TBase.
template < typename TBase, typename Argument = ArgumentType >
class ClassWrapperBase
{
    public:
    ClassWrapperBase() {}

    virtual ~ClassWrapperBase() {}

    /// Creates an instance of TBase.
    virtual boost::shared_ptr< TBase > CreateInstance( const xmlparser::XmlParameter* param, const Argument* arg = 0 ) = 0;
};

} /* namespace xmlfactory */
#endif /* FACTORYWRAPPER_H_ */
