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
 * DAEInputData.h
 *
 *  Created on: 14.08.2013
 *      Author: chris
 */

#ifndef DAEINPUTDATA_H_
#define DAEINPUTDATA_H_

#ifdef BOOST_MPI

#include "../../taskData.h"
#include "../../../misc/matrixInclude.h"
#include <vector>

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>
#include "../../serialization/armadilloMatrix.h"
#include "../../serialization/matlabMatrix.h"

namespace threading
{
namespace daetask
{
/// DTO for DAETask
template < class MatrixType >
class DAEInputData : public threading::TaskData
{
    public:
    DAEInputData() {}

    virtual ~DAEInputData() {}

    bool operator==( const TaskData &obj ) const { return true; }

    double CurrentValue;
    MatrixType StateVector;
    double Dt;

    private:
    friend class boost::serialization::access;
    template < class Archive >
    void serialize( Archive &ar, const unsigned int version )
    {
        ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP( TaskData );
        ar &BOOST_SERIALIZATION_NVP( CurrentValue );
        ar &BOOST_SERIALIZATION_NVP( StateVector );
        ar &BOOST_SERIALIZATION_NVP( Dt );
    }
};
}
} /* namespace threading */

#endif /* BOOST_MPI */

#endif /* DAEINPUTDATA_H_ */
