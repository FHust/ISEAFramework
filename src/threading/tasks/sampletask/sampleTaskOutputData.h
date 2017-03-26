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
 * SampleTaskOutputData.h
 *
 *  Created on: 14.08.2013
 *      Author: chris
 */

#ifndef SAMPLETASKOUTPUTDATA_H_
#define SAMPLETASKOUTPUTDATA_H_

#include "../../taskData.h"

namespace threading
{
namespace sampletask
{

class SampleTaskOutputData : public threading::TaskData
{
    public:
    SampleTaskOutputData()
        : mytestvar( 0 )
    {
    }

    virtual ~SampleTaskOutputData() {}

    bool operator==( const TaskData & /* obj */ ) const { return true; }

    int mytestvar;
    size_t mMiliSecsOfOptionalWork;

    private:
    friend class boost::serialization::access;
    template < class Archive >
    void serialize( Archive &ar, const unsigned int /* version */ )
    {
        ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP( TaskData );
        ar &BOOST_SERIALIZATION_NVP( mytestvar );
        ar &BOOST_SERIALIZATION_NVP( mMiliSecsOfOptionalWork );
    }
};
}
} /* namespace threading */
#endif /* SAMPLETASKOUTPUTDATA_H_ */
