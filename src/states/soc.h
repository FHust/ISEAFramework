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
* File Name : soc.h
* Creation Date : 01-10-2012
* Last Modified : Do 23 Jan 2014 13:51:05 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SOC_
#define _SOC_
// STD

#include <vector>


#include "dgl_state.h"


class TestSoc;
class TestXML;

namespace electrical
{
namespace state
{

/// This class describes the behaviour of the State of Charge depending on the capacity
class Soc : public ::state::Dgl_state
{
    friend class ::TestSoc;
    friend class ::TestXML;

    public:
    Soc( double maxCapcity /* [Ah] */, double currenState /*[%]*/, std::vector< double > measurmentpoints );
    virtual ~Soc(){};

    double GetValue() const;
    double GetMaxCapacity() const;
    double GetCapacity() const;
    double GetMaxDt() const;

    void SetCapacity( double capacity );
    void UpdateCapacity( double deltaCapacity );

    private:
    double const mMaxCapAsec;
    double mCurCapAsec;
    double mCurrent;
    std::vector< double > mMeasurementpoints;

    protected:
};


} /* state */
} /* electrical */
#endif /* _SOC_ */
