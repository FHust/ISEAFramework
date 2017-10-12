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
* File Name : TestSphericalDiffusion.cpp
* Creation Date : 23-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestSphericalDiffusion.h"

// ETC
#include "../../electrical/sphericalDiffusion.h"

#include "../../factory/electrical/electricalfactorybuilder.h"
#include "../../factory/object/objectfactorybuilder.h"
#include "../../factory/state/statefactorybuilder.h"

#include "../../object/const_obj.h"

#include "../../xmlparser/tinyxml2/xmlparserimpl.h"


extern template class electrical::SphericalDiffusion< myMatrixType >;
extern template class systm::StateSystemGroup< myMatrixType >;
extern template class object::Object< double >;
extern template class object::ConstObj< double >;
extern template class factory::Factory< ::state::Dgl_state, ::factory::ArgumentTypeState >;
extern template class factory::Factory< ::object::Object< double >, ::factory::ArgumentTypeObject< double > >;
extern template class factory::Factory< ::electrical::TwoPort< myMatrixType >, ::factory::ArgumentTypeElectrical >;

static std::vector< double > tauVec = {-1.0086,  -2.9773,  -5.9673,  -10.2124, -16.2117,
                                       -25.2132, -38.8489, -59.9623, -93.5898, -146.6657};
static std::vector< double > inv_c = {10.0030, 9.9797,  10.2763, 10.1112, 14.4984,
                                      15.8474, 16.8469, 24.5525, 33.6549, 37.7058};


void TestSphericalDiffusion::TestSphericalDiffusionCreation()
{

    const size_t maxSize = 12;
    double resValue( 10.0 );
    double tauValue( 1 );
    const double cur = 10;
    const int hasCapacity = 1;
    boost::shared_ptr< object::Object< double > > r( new object::ConstObj< double >( resValue ) );
    boost::shared_ptr< object::Object< double > > tau( new object::ConstObj< double >( tauValue ) );
    electrical::SphericalDiffusion< myMatrixType > sd( r, tau, 10, hasCapacity );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    stateSystemGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    sd.SetSystem( &stateSystemGroup );
    stateSystemGroup.mDglStateSystem.Initialize( maxSize );

    myMatrixType current;
    current.zeros( 1, maxSize );
    current( 0, maxSize - 1 ) = cur;
    sd.SetCurrent( current );
    auto voltage = *sd.GetVoltage();

    for ( size_t i = 0; i < maxSize - 1; ++i )
    {
        TS_ASSERT_DELTA( voltage( 0, i ), 1, 0.0001 );
    }
    TS_ASSERT_DELTA( voltage( 0, maxSize - 1 ),
                     cur * resValue * std::accumulate( electrical::SphericalDiffusion< myMatrixType >::mRFactor.begin() + 10,
                                                       electrical::SphericalDiffusion< myMatrixType >::mRFactor.end(), 0.0 ),
                     0.0001 );

    auto &matrixA = stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix();

    for ( size_t i = 0; i < maxSize - 2; ++i )
    {
        TS_ASSERT_DELTA( matrixA( i, i ), tauVec.at( i ), 0.0001 );
        TS_ASSERT_DELTA( matrixA( i, matrixA.n_cols - 1 ), inv_c.at( i ) * cur, 0.001 );
    }
    TS_ASSERT_DELTA( matrixA( matrixA.n_rows - 1, matrixA.n_cols - 1 ), cur * 15.0, 0.0001 );
}

void TestSphericalDiffusion::TestSphericalDiffusionFactoryCreation()
{
    const size_t maxSize = 11;
    const double cur = 10;
    const double resValue( 10.0 );

    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get() ) );


    const char *xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
            <Options>\
                <SampleRate>\
                    76\
                </SampleRate>\
            </Options>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement class='SphericalDiffusion' WithCapacity='true'>\
                <OhmicResistance class='ConstObj'>\
                    <Value>10</Value>\
                </OhmicResistance>\
                <Tau class='ConstObj'>\
                    <Value>1</Value>\
                </Tau>\
                </RootElement>\
             </Configuration>";


    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    auto sd = electricalFactory->CreateInstance( rootparam );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    stateSystemGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    sd->SetSystem( &stateSystemGroup );
    stateSystemGroup.mDglStateSystem.Initialize( maxSize );

    myMatrixType current;
    current.zeros( 1, maxSize );
    current( 0, maxSize - 1 ) = cur;
    sd->SetCurrent( current );
    auto voltage = *sd->GetVoltage();


    for ( size_t i = 0; i < maxSize - 1; ++i )    // Incl. RC and the last C
    {
        TS_ASSERT_DELTA( voltage( 0, i ), 1, 0.0001 );
    }
    TS_ASSERT_DELTA( voltage( 0, maxSize - 1 ),
                     cur * resValue * std::accumulate( electrical::SphericalDiffusion< myMatrixType >::mRFactor.begin() + 10,
                                                       electrical::SphericalDiffusion< myMatrixType >::mRFactor.end(), 0.0 ) +
                      1,
                     0.0001 );

    auto &matrixA = stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix();

    for ( size_t i = 0; i < maxSize - 1; ++i )
    {
        TS_ASSERT_DELTA( matrixA( i, i ), tauVec.at( i ), 0.0001 );
        TS_ASSERT_DELTA( matrixA( i, matrixA.n_cols - 1 ), inv_c.at( i ) * cur, 0.001 );
    }
    TS_ASSERT_DELTA( matrixA( matrixA.n_rows - 1, matrixA.n_cols - 1 ), cur * 15.0, 0.0001 );
}
