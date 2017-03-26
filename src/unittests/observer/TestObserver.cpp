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
* File Name : TestObserver.cpp
* Creation Date : 21-02-2014
* Last Modified : Mi 02 Sep 2015 18:25:29 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestObserver.h"

#include "../../misc/matrixInclude.h"

#include "../../electrical/zarc.h"
#include "../../electrical/voltagesource.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/cellelement.h"
#include "../../electrical/paralleltwoport.h"
#include "../../system/system.h"

#include "../../observer/twoPortObserver.h"
#include "../../observer/filter/csvfilter.h"
#include "../../observer/filter/decimatefilter.h"
#include "../../observer/filter/stdoutfilter.h"


std::vector< std::vector< double > > TestObserver::CopyToVector( const double data[7][4] )
{
    std::vector< std::vector< double > > returnvector( 7, std::vector< double >( 4, 0 ) );
    for ( size_t i = 0; i < 7; ++i )
        returnvector.at( i ).assign( data[i], data[i] + 4 );
    return returnvector;
}

void TestObserver::testObserverOperationsSingleCell()
{
    const double socData[] = {5, 10, 20, 30, 35, 40, 50, 60, 65, 70, 80, 90, 100};
    std::vector< double > socM( socData, socData + 12 );
    boost::shared_ptr< ::state::ThermalState< double > > tempState( new ::state::ThermalState< double >( 23 ) );
    boost::shared_ptr< electrical::state::Soc > socState( new electrical::state::Soc( 2.05, 20, socM ) );

    boost::shared_ptr< electrical::Cellelement< myMatrixType > > cellelem(
     new electrical::Cellelement< myMatrixType >( tempState, socState ) );

    const double rowData[] = {0, 10, 23, 50};
    std::vector< double > rows( rowData, rowData + 4 );

    const double colData[] = {5, 20, 35, 50, 65, 80, 90};
    std::vector< double > cols( colData, colData + 7 );


    const double tau1[7][4] = {{1.12980227037822e-01, 1.71749789914134e-01, 4.27878577005388e-01, 4.70584769553997e-01},
                               {2.88029269499999e-01, 2.03069924403468e-01, 7.59434298204209e-02, 5.38248547348156e-02},
                               {1.80149147116020e-01, 1.12630469688383e-01, 3.52185854766679e-02, 2.98230802877780e-02},
                               {1.47909096582298e-01, 8.33921914682953e-02, 2.57060565921076e-02, 2.22501291807582e-02},
                               {1.32900242338032e-01, 7.22938740353130e-02, 2.05480235442034e-02, 9.95886997764575e-02},
                               {1.19852623435413e-01, 6.56657395160509e-02, 2.36971844841798e-02, 1.02914420936082e-01},
                               {1.18273000586515e-01, 6.60491071602289e-02, 3.25921294643322e-02, 1.09197268465146e-01}};

    const double ohm1[7][4] = {{0.0264994082415437, 0.0264994082415437, 0.0264994082415437, 0.0299913094135788},
                               {0.0675569999999998, 0.0313318160740531, 0.0127731516995969, 0.00404164451684630},
                               {0.0536386688114087, 0.0248200664672378, 0.00989750001969683, 0.00303781463624295},
                               {0.0478600636580172, 0.0219992555610513, 0.00877924658179266, 0.00234391473354453},
                               {0.0441171340633137, 0.0206405039153971, 0.00794555485936455, 0.00454415109298745},
                               {0.0418986584966986, 0.0203278573267527, 0.00866505912064747, 0.00344848953405180},
                               {0.0421316495020664, 0.0207584663601899, 0.0102011087727835, 0.00296383805146105}};

    const double phi1[7][4] = {{0.574590000000000, 0.516891694022437, 0.408379079339744, 0.110443124808620},
                               {0.574590000000000, 0.516891694022437, 0.568073405078093, 0.566435038531618},
                               {0.616859078283881, 0.579111165549142, 0.662374262351899, 0.620812008168125},
                               {0.625502091112064, 0.610693210867254, 0.697319797270455, 0.671812046180353},
                               {0.621586176902636, 0.620972340223097, 0.721715387830978, 0.323239015612757},
                               {0.620607929391110, 0.630419920219851, 0.671002168217511, 0.371661290927877},
                               {0.620534462894923, 0.628465004550853, 0.585312180039373, 0.410302314031108}};


    boost::shared_ptr< object::LookupObj2dWithState< double > > tauObj1(
     new object::LookupObj2dWithState< double >( CopyToVector( tau1 ), rows, cols, tempState, socState ) );

    boost::shared_ptr< object::LookupObj2dWithState< double > > ohmObj1(
     new object::LookupObj2dWithState< double >( CopyToVector( ohm1 ), rows, cols, tempState, socState ) );

    boost::shared_ptr< object::LookupObj2dWithState< double > > phiObj1(
     new object::LookupObj2dWithState< double >( CopyToVector( phi1 ), rows, cols, tempState, socState ) );

    cellelem->AddChild( new electrical::Zarc< myMatrixType >( tauObj1, ohmObj1, phiObj1 ) );

    const double tau2[7][4] = {{0.0264994082415437, 0.0264994082415437, 0.0264994082415437, 0.0299913094135788},
                               {0.0675569999999998, 0.0313318160740531, 0.0127731516995969, 0.00404164451684630},
                               {0.0536386688114087, 0.0248200664672378, 0.00989750001969683, 0.00303781463624295},
                               {0.0478600636580172, 0.0219992555610513, 0.00877924658179266, 0.00234391473354453},
                               {0.0441171340633137, 0.0206405039153971, 0.00794555485936455, 0.00454415109298745},
                               {0.0418986584966986, 0.0203278573267527, 0.00866505912064747, 0.00344848953405180},
                               {0.0421316495020664, 0.0207584663601899, 0.0102011087727835, 0.00296383805146105}};

    const double ohm2[7][4] = {{0.0422632965521782, 0.0422632965521782, 0.0422632965521782, 0.0646154311756893},
                               {0.0122909999999999, 0.00820305593966980, 0.00584662021670523, 0.00383743519303631},
                               {0.0322225070656886, 0.0289438148360227, 0.0225495908462913, 0.00599634893454420},
                               {0.0257481760483287, 0.0186632656230284, 0.00993329376244036, 0.00582987109816202},
                               {0.0157426548912461, 0.0171684201770746, 0.0134953332473813, 0.00457332312382527},
                               {0.0218280618476754, 0.0131225853688649, 0.00741527720123879, 0.00329020951414971},
                               {0.0153521701301020, 0.00846967601820038, 0.00563955480281332, 0.00315825717209717}};

    const double phi2[7][4] = {{1, 0.957683980942010, 0.762474540690386, 0.810166837787303},
                               {1, 0.957683980942010, 0.837851589303327, 0.826206919812180},
                               {0.806534226878276, 0.755415801951905, 0.630228467428149, 0.686969202518359},
                               {0.708526377255388, 0.661628608281274, 0.630894213874104, 0.675415879938948},
                               {0.791964147404381, 0.697353484425094, 0.628110619592649, 0.801166605326404},
                               {0.759343341643463, 0.720390317831926, 0.755733232495091, 0.938336421996388},
                               {0.790468798628622, 0.797350100822233, 0.852903199927494, 0.986742576117900}};


    boost::shared_ptr< object::LookupObj2dWithState< double > > tauObj2(
     new object::LookupObj2dWithState< double >( CopyToVector( tau2 ), rows, cols, tempState, socState ) );

    boost::shared_ptr< object::LookupObj2dWithState< double > > ohmObj2(
     new object::LookupObj2dWithState< double >( CopyToVector( ohm2 ), rows, cols, tempState, socState ) );

    boost::shared_ptr< object::LookupObj2dWithState< double > > phiObj2(
     new object::LookupObj2dWithState< double >( CopyToVector( phi2 ), rows, cols, tempState, socState ) );

    cellelem->AddChild( new electrical::Zarc< myMatrixType >( tauObj2, ohmObj2, phiObj2 ) );

    const double ocv[4][10] = {{3.47980625000000, 3.56020000000000, 3.61080000000000, 3.64033750000000, 3.67903750000000,
                                3.74791250000000, 3.84935000000000, 3.94420000000000, 4.05327500000000, 4.18230000000000},
                               {3.48386250000000, 3.56396250000000, 3.61388750000000, 3.64350000000000, 3.68183750000000,
                                3.75231250000000, 3.85146875000000, 3.94563750000000, 4.05550000000000, 4.18410000000000},
                               {3.48390000000000, 3.56456250000000, 3.61843125000000, 3.64930000000000, 3.68808750000000,
                                3.76500000000000, 3.85450000000000, 3.94680000000000, 4.05477500000000, 4.17770000000000},
                               {3.47570000000000, 3.55060000000000, 3.60896250000000, 3.64975000000000, 3.68677500000000,
                                3.75358750000000, 3.84280000000000, 3.93626250000000, 4.04295000000000, 4.16480000000000}};
    std::vector< std::vector< double > > ocvLookupData( 4, std::vector< double >( 10, 0 ) );
    for ( size_t i = 0; i < 4; ++i )
    {
        ocvLookupData.at( i ).assign( ocv[i], ocv[i] + 10 );
    }

    const double ocvColData[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    std::vector< double > ocvCol( ocvColData, ocvColData + 10 );

    boost::shared_ptr< object::LookupObj2dWithState< double > > ocvLookup(
     new object::LookupObj2dWithState< double >( ocvLookupData, ocvCol, rows, socState, tempState ) );

    const double rser[7][4] = {{0.0288200000000001, 0.0273753907300395, 0.0264344729259093, 0.0135127660821939},
                               {0.0288200000000001, 0.0273753907300395, 0.0270767085789135, 0.0275280515729565},
                               {0.0290500142586083, 0.0277525443197526, 0.0275261486868454, 0.0276368633915343},
                               {0.0288630838514731, 0.0278576890117617, 0.0275537618404411, 0.0277933657755191},
                               {0.0286781206123875, 0.0277738617773271, 0.0275453220226692, 0.0259269199477185},
                               {0.0285331061111646, 0.0277867915477187, 0.0268358840093433, 0.0267808653711796},
                               {0.0286358289536196, 0.0277803384908296, 0.0257328031445100, 0.0272959924289106}};

    boost::shared_ptr< object::LookupObj2dWithState< double > > rserLookup(
     new object::LookupObj2dWithState< double >( CopyToVector( rser ), rows, cols, tempState, socState ) );


    electrical::VoltageSource< myMatrixType > *voltSource = new electrical::VoltageSource< myMatrixType >( ocvLookup );
    electrical::OhmicResistance< myMatrixType > *ohmResistance = new electrical::OhmicResistance< myMatrixType >( rserLookup );
    cellelem->AddChild( voltSource );
    cellelem->AddChild( ohmResistance );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;

    double t = 0.000;
    double dt = 100;
    double Current = 2.05;

    cellelem->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    cellelem->SetInitialCurrent( Current );
    cellelem->UpdateStateSystemGroup();

    observer::TwoPortObserver< myMatrixType > k( cellelem );
    k.AddFilter( new observer::DecimateFilterTwoPort< myMatrixType >( 0.1 ) );
//        k.AddFilter( new observer::StdoutFilter< myMatrixType >() );
#ifndef __NO_STRING__
    k.AddFilter( new observer::CsvFilterTwoPort< myMatrixType >( "singlecelltest.csv" ) );
#endif

    k( t );

    systm::System< myMatrixType > system( &stateSystemGroup, dt );

    while ( t < 30 )
    {
        cellelem->UpdateStateSystemGroup();
        t = system.Solve();
        cellelem->CalculateStateDependentValues();
        k( t );
    }
}
