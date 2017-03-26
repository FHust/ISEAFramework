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
* File Name : TestZarc.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Nov 2014 18:43:58 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestZarc.h"

//ETC
#include "../../object/lookup_obj2d_with_state.h"

#include "../../electrical/zarc.h"

std::vector< std::vector<double> > CreatemPoints()
{
    std::vector< std::vector<double> > mPoints;
    for( size_t i = 0 ; i < 7 ; ++i )
        mPoints.push_back( std::vector<double>(4,0) );
    return mPoints;
}

std::vector<double> CreateRows()
{
    std::vector< double > rows;
    rows.push_back(0);
    rows.push_back(10);
    rows.push_back(23);
    rows.push_back(40);
    return rows;
}
std::vector<double> CreateCols()
{
    std::vector< double > cols;
    cols.push_back(5);
    cols.push_back(20);
    cols.push_back(35);
    cols.push_back(50);
    cols.push_back(65);
    cols.push_back(80);
    cols.push_back(90);
    return cols;
}


void TestZarc::testZarcCreation()
{
    boost::shared_ptr< ::state::ThermalState< double > > tempState(new ::state::ThermalState< double >(23) );
    const double socValues[] = {5,20,35,50,65,80,90};

    std::vector<double> tmp;
    tmp.insert(tmp.begin(), socValues, socValues+7);

    boost::shared_ptr< electrical::state::Soc > socState(new electrical::state::Soc(10, 0.5, tmp));

    std::vector< std::vector<double> > mPoints = CreatemPoints();
    std::vector<double> rows = CreateRows();
    std::vector<double> cols = CreateCols();

    boost::shared_ptr< object::LookupObj2dWithState<double> > capObj( new object::LookupObj2dWithState<double>(
                mPoints
                , rows
                , cols, tempState, socState) );

    boost::shared_ptr< object::LookupObj2dWithState<double> > ohmObj( new object::LookupObj2dWithState<double>(
                mPoints
                , rows
                , cols, tempState, socState) );

    boost::shared_ptr< object::LookupObj2dWithState<double> > phiObj( new object::LookupObj2dWithState<double>(
                mPoints
                , rows 
                , cols,  tempState, socState) );

    electrical::Zarc< myMatrixType > zarc(
            capObj
            , ohmObj
            , phiObj
            );
};

void TestZarc::testZarcOperations()
{
    boost::shared_ptr< ::state::ThermalState< double > > tempState(new ::state::ThermalState< double >(23) );
    const double socValues[] = {5,20,35,50,65,80,90};

    std::vector<double> tmp;
    tmp.insert(tmp.begin(), socValues, socValues+7);

    boost::shared_ptr< electrical::state::Soc > socState(new electrical::state::Soc(10, 0.5, tmp));

    std::vector< std::vector<double> > mPoints = CreatemPoints();
    std::vector<double> rows = CreateRows();
    std::vector<double> cols = CreateCols();

    boost::shared_ptr< object::LookupObj2dWithState<double> > capObj( new object::LookupObj2dWithState<double>(
                mPoints
                , rows
                , cols, tempState, socState) );

    boost::shared_ptr< object::LookupObj2dWithState<double> > ohmObj( new object::LookupObj2dWithState<double>(
                mPoints
                , rows
                , cols, tempState, socState) );

    boost::shared_ptr< object::LookupObj2dWithState<double> > phiObj( new object::LookupObj2dWithState<double>(
                mPoints
                , rows 
                , cols,  tempState, socState) );

    electrical::Zarc< myMatrixType > zarc(
            capObj
            , ohmObj
            , phiObj
            );

    TS_ASSERT( capObj.get() == zarc.GetTauObject() );
    TS_ASSERT( ohmObj.get() == zarc.GetRObject() );
    TS_ASSERT( phiObj.get() == zarc.GetPhiObject() );

    TS_ASSERT( &zarc.mLookupPhi2TauFactor == &zarc.GetLookupPhi2TauFactor() );
    TS_ASSERT( &zarc.mLookupPhi2RFactorInnen == &zarc.GetLookupPhi2RFactorInnen() );
    TS_ASSERT( &zarc.mLookupPhi2RFactorAussen == &zarc.GetLookupPhi2RFactorAussen() );

    TS_ASSERT_EQUALS(zarc.CanHaveChildren(), false);
    TS_ASSERT_EQUALS(zarc.HasChildren(), false);
    TS_ASSERT_EQUALS(zarc.HasSystem(), false);
    TS_ASSERT_DELTA( (*tempState)(), 23.0, 0.00001);
};

