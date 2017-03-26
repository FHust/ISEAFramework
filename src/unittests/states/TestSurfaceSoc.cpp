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
* File Name : TestSurfaceSoc.cpp
* Creation Date : 30-06-2014
* Last Modified : Di 12 Aug 2014 19:23:18 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestSurfaceSoc.h"

//STD
#include <vector>

//BOOST
#include <boost/shared_ptr.hpp>

//ETC
#include "../../states/surface_soc.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/voltagesource.h"
#include "../../object/const_obj.h"
#include "../../object/lookup_obj1d.h"

void TestSurfaceSoc::testCreation()
{
    typedef object::ConstObj<double> CObj;
    electrical::state::SurfaceSoc surfaceSoc;

    boost::shared_ptr< CObj> cObj( new CObj( 10.0 ) );
    surfaceSoc.SetReverseLookup(cObj);
    surfaceSoc.UpdateLookUp();
    TS_ASSERT_EQUALS( surfaceSoc.GetValue(), 10.0 );
}

void TestSurfaceSoc::testCreation1DLookup()
{
    typedef object::LookupObj1D<double> LObj;
    typedef object::ConstObj<double> CObj;
    electrical::state::SurfaceSoc surfaceSoc;
    std::vector< electrical::TwoPort<myMatrixType> *> vec;
    const double voltageValue = 3.5501;
    const double socData[] = {2.5, 5, 7.5, 10, 12.5, 15, 17.5, 20, 22.5, 25, 27.5, 30, 32.5, 35, 37.5, 40, 42.5, 45, 47.5, 50, 52.5, 55, 57.5, 60, 62.5, 65, 67.5, 70, 72.5, 75, 77.5, 80, 82.5, 85, 87.5, 90, 92.5, 95, 97.5, 100};

    const double voltageData[] = {3.2428, 3.3898, 3.4603, 3.4949, 3.5156, 3.533, 3.5501, 3.5663, 3.5797, 3.5929, 3.6045, 3.6139, 3.6234, 3.6332, 3.6427, 3.6528, 3.6622, 3.6711, 3.682, 3.6928, 3.7043, 3.717, 3.7314, 3.7469, 3.7631, 3.7824, 3.8025, 3.8221, 3.843, 3.8656, 3.8917, 3.9181, 3.9435, 3.969, 3.9949, 4.0211, 4.048, 4.0759, 4.1046, 4.1341};
    std::vector<double> soc(socData, socData + sizeof(socData) / sizeof(double));
    std::vector<double> voltage(voltageData, voltageData + sizeof(voltageData) / sizeof(double));

    boost::shared_ptr< CObj> cObj( new CObj( voltageValue ) );
    boost::shared_ptr< electrical::VoltageSource< myMatrixType > > volt( ( new electrical::VoltageSource< myMatrixType >( cObj, false )) );
    vec.push_back( volt.get() );

    boost::shared_ptr< LObj> lObj( new LObj( soc, voltage) );

    surfaceSoc.SetReverseLookup(lObj);
    surfaceSoc.SetElementsForLookup(vec);

    for( size_t i = 0 ; i < soc.size() ; ++i )
    {
        volt->mVoltageValue = voltage[i];
        TS_ASSERT_DELTA( lObj->GetValue( voltage[i]) , soc[i], 0.0000001);
        surfaceSoc.UpdateLookUp();
        TS_ASSERT_EQUALS( surfaceSoc.GetValue(), soc[i]);
    }
}
