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
* File Name : thermal_probe.h
* Creation Date : 04-03-2015
* Last Modified : Fr 06 Mär 2015 10:53:40 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ERMAL_PROBE_
#define _ERMAL_PROBE_

#include <vector>

#include "../geometry/cartesian.h"
#include "../thermal/blocks/elements/thermal_element.h"


class TestFactories;
/// Namespace for probes
namespace probe
{

/// DESCRIPTION
class ThermalProbe
{
    friend class ::TestFactories;

    public:
    /// Constructor for vector
    ThermalProbe( const std::vector< double > &vec );
    /// Constructor for x-y-z coordinates
    ThermalProbe( const double x, const double y, const double z );
    ~ThermalProbe(){};
    geometry::Cartesian< double > GetCoordinates() const { return mProbeCoordinates; };

    void SetCorrespondingThermalElement( boost::shared_ptr< thermal::ThermalElement< double > > mCorr )
    {
        mCorrespondingThermalElement = mCorr;
    };

    thermal::ThermalElement< double > *GetCorrespondingThermalElement() const
    {
        return mCorrespondingThermalElement.get();
    };

    double GetTemperature() const { return mCorrespondingThermalElement->GetTemperature(); };


    private:
    geometry::Cartesian< double > mProbeCoordinates;
    boost::shared_ptr< thermal::ThermalElement< double > > mCorrespondingThermalElement;

    protected:
};
}

#endif /* _ERMAL_PROBE_ */
