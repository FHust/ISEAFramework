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
 * TaskData.h
 *
 *  Created on: 08.08.2013
 *      Author: chris
 */

#ifndef TASKDATA_H_
#define TASKDATA_H_

#include <boost/serialization/access.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
//#include <boost/serialization/string.hpp>
//#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
//#include <boost/date_time/posix_time/time_serialize.hpp>
//#include <boost/date_time/gregorian/greg_serialize.hpp>
//#include <boost/serialization/extended_type_info.hpp>

namespace threading
{
/// Datatansferobject for MPI communication (has to be serializeable)
class TaskData
{

    public:
    TaskData() {}

    virtual ~TaskData() {}

    virtual bool operator==( const TaskData & /* obj */ ) const { return true; }

    private:
    friend class boost::serialization::access;
    template < class Archive >
    void serialize( Archive & /* ar */, const unsigned int /* version */ )
    {
    }
};
}
/* namespace threading */
#endif /* TASKDATA_H_ */
