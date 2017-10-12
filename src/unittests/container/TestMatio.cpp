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
* File Name : TestMatio.cpp
* Creation Date : 03-07-2015
* Last Modified : Di 13 Okt 2015 17:07:37 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "../../container/matio_file.h"
#include "../../container/matio_data.h"
#include "../../misc/macros.h"
#include "TestMatio.h"

using namespace matlab;

void TestMatio::TestMatFileEmpty() { MatFile emptyConstructor; }

void TestMatio::TestMatFileFail()
{
    bool hasThrown = false;
    try
    {
        MatFile readOly( "unknown_file.mat", MAT_ACC_RDONLY );
    }
    catch ( const std::runtime_error &e )
    {
        TS_ASSERT_EQUALS( e.what(), "Unknown file: unknown_file.mat\n" );
        hasThrown = true;
    }
    if ( !hasThrown )
    {
        TS_FAIL( "File was found, though it should not have been found" );
    }
}

void TestMatio::TestMatFileEmptyStruct()
{
    MatFile matfileRO( "emptyStructs.mat", MAT_ACC_RDONLY );
    TS_ASSERT_THROWS( matfileRO["diga"]["asijdo"], std::runtime_error );

    // Check for leaks
    MatioData &x = matfileRO["diga"];
    MatioData &y = matfileRO["diga"]["daten"];
    UNUSED( x );
    UNUSED( y );
}

void TestMatio::TestMatFileContent1D()
{

    MatFile matfileRO( "structWithContent.mat", MAT_ACC_RDONLY );
    TS_ASSERT_THROWS( matfileRO["diga"]["asijdo"], std::runtime_error );

    MatioData &z = matfileRO["diga"]["daten"]["Programmdauer"];
    for ( size_t i = 0; i < 10; ++i )
    {
        TS_ASSERT_DELTA( static_cast< double >( i ), z( 0, i ), 0.0001 );
    }
}

void TestMatio::TestMatFileContent2D()
{
    MatFile matfileRO( "structWith2D.mat", MAT_ACC_RDONLY );
    TS_ASSERT_THROWS_NOTHING( matfileRO["diga"]["daten"] );

    MatioData &z = matfileRO["diga"]["daten"]["test2D"];
    size_t counter = 1;

    for ( size_t x = 0; x < 2; ++x )
    {
        for ( size_t y = 0; y < 10; ++y )
        {
            TS_ASSERT_DELTA( static_cast< double >( counter ), z( x, y ), 0.0001 );
            ++counter;
        }
    }
}

void TestMatio::TestMatFileContent2DSlice()
{
    MatFile matfileRO( "structWith2D.mat", MAT_ACC_RDONLY );
    TS_ASSERT_THROWS_NOTHING( matfileRO["diga"]["daten"] );

    MatioData &z = matfileRO["diga"]["daten"]["test2D"];
    size_t counter = 1;
    const size_t maxSize_Y = 10, maxSize_X = 2;

    for ( size_t x = 0; x < maxSize_X; ++x )
    {
        for ( size_t y = 0; y < maxSize_Y; ++y )
        {
            TS_ASSERT_DELTA( static_cast< double >( counter ), z( x, y ), 0.0001 );
            ++counter;
        }
    }

    {
        std::vector< double > shortVecY_DIM = z.operator()< MatioData::DIMENSION::Y_DIM >( 0 );
        counter = 1;
        for ( size_t i = 0; i < shortVecY_DIM.size(); ++i )
        {
            TS_ASSERT_DELTA( shortVecY_DIM.at( i ), static_cast< double >( counter ), 0.0001 );
            counter += maxSize_Y;
        }
    }


    std::vector< double > shortVecY_DIM = z.operator()< MatioData::DIMENSION::Y_DIM >( 1 );
    TS_ASSERT_EQUALS( shortVecY_DIM.size(), 2 );
    counter = 2;
    for ( size_t i = 0; i < shortVecY_DIM.size(); ++i )
    {
        TS_ASSERT_DELTA( shortVecY_DIM.at( i ), static_cast< double >( counter ), 0.0001 );
        counter += maxSize_Y;
    }

    {
        std::vector< double > shortVecX_DIM = z.operator()< MatioData::DIMENSION::X_DIM >( 0 );

        counter = 1;
        for ( size_t i = 0; i < shortVecX_DIM.size(); ++i )
        {
            TS_ASSERT_DELTA( shortVecX_DIM.at( i ), static_cast< double >( counter ), 0.0001 );
            counter += 1;
        }
    }

    std::vector< double > shortVecX_DIM = z.operator()< MatioData::DIMENSION::X_DIM >( 1 );

    counter = 11;
    for ( size_t i = 0; i < shortVecX_DIM.size(); ++i )
    {
        TS_ASSERT_DELTA( shortVecX_DIM.at( i ), static_cast< double >( counter ), 0.0001 );
        counter += 1;
    }
}

void TestMatio::TestMatFileContent2DSliceTokenAcccesAtFileLevel()
{
    MatFile matfileRO( "structWith2D.mat", MAT_ACC_RDONLY );
    TS_ASSERT_THROWS_NOTHING( matfileRO["diga"]["daten"] );

    MatioData &z = matfileRO["diga.daten.test2D"];
    size_t counter = 1;
    const size_t maxSize_Y = 10, maxSize_X = 2;

    for ( size_t x = 0; x < maxSize_X; ++x )
    {
        for ( size_t y = 0; y < maxSize_Y; ++y )
        {
            TS_ASSERT_DELTA( static_cast< double >( counter ), z( x, y ), 0.0001 );
            ++counter;
        }
    }

    {
        std::vector< double > shortVecY_DIM = z.operator()< MatioData::DIMENSION::Y_DIM >( 0 );
        counter = 1;
        for ( size_t i = 0; i < shortVecY_DIM.size(); ++i )
        {
            TS_ASSERT_DELTA( shortVecY_DIM.at( i ), static_cast< double >( counter ), 0.0001 );
            counter += maxSize_Y;
        }
    }


    std::vector< double > shortVecY_DIM = z.operator()< MatioData::DIMENSION::Y_DIM >( 1 );
    TS_ASSERT_EQUALS( shortVecY_DIM.size(), 2 );
    counter = 2;
    for ( size_t i = 0; i < shortVecY_DIM.size(); ++i )
    {
        TS_ASSERT_DELTA( shortVecY_DIM.at( i ), static_cast< double >( counter ), 0.0001 );
        counter += maxSize_Y;
    }

    {
        std::vector< double > shortVecX_DIM = z.operator()< MatioData::DIMENSION::X_DIM >( 0 );

        counter = 1;
        for ( size_t i = 0; i < shortVecX_DIM.size(); ++i )
        {
            TS_ASSERT_DELTA( shortVecX_DIM.at( i ), static_cast< double >( counter ), 0.0001 );
            counter += 1;
        }
    }

    std::vector< double > shortVecX_DIM = z.operator()< MatioData::DIMENSION::X_DIM >( 1 );

    counter = 11;
    for ( size_t i = 0; i < shortVecX_DIM.size(); ++i )
    {
        TS_ASSERT_DELTA( shortVecX_DIM.at( i ), static_cast< double >( counter ), 0.0001 );
        counter += 1;
    }
}

void TestMatio::TestMatFileContent2DSliceTokenAcccesAtDataLevel()
{
    MatFile matfileRO( "structWith2D.mat", MAT_ACC_RDONLY );
    TS_ASSERT_THROWS_NOTHING( matfileRO["diga"]["daten"] );

    MatioData &z = matfileRO["diga"]["daten.test2D"];
    size_t counter = 1;
    const size_t maxSize_Y = 10, maxSize_X = 2;

    for ( size_t x = 0; x < maxSize_X; ++x )
    {
        for ( size_t y = 0; y < maxSize_Y; ++y )
        {
            TS_ASSERT_DELTA( static_cast< double >( counter ), z( x, y ), 0.0001 );
            ++counter;
        }
    }

    {
        std::vector< double > shortVecY_DIM = z.operator()< MatioData::DIMENSION::Y_DIM >( 0 );
        counter = 1;
        for ( size_t i = 0; i < shortVecY_DIM.size(); ++i )
        {
            TS_ASSERT_DELTA( shortVecY_DIM.at( i ), static_cast< double >( counter ), 0.0001 );
            counter += maxSize_Y;
        }
    }


    std::vector< double > shortVecY_DIM = z.operator()< MatioData::DIMENSION::Y_DIM >( 1 );
    TS_ASSERT_EQUALS( shortVecY_DIM.size(), 2 );
    counter = 2;
    for ( size_t i = 0; i < shortVecY_DIM.size(); ++i )
    {
        TS_ASSERT_DELTA( shortVecY_DIM.at( i ), static_cast< double >( counter ), 0.0001 );
        counter += maxSize_Y;
    }

    {
        std::vector< double > shortVecX_DIM = z.operator()< MatioData::DIMENSION::X_DIM >( 0 );

        counter = 1;
        for ( size_t i = 0; i < shortVecX_DIM.size(); ++i )
        {
            TS_ASSERT_DELTA( shortVecX_DIM.at( i ), static_cast< double >( counter ), 0.0001 );
            counter += 1;
        }
    }

    std::vector< double > shortVecX_DIM = z.operator()< MatioData::DIMENSION::X_DIM >( 1 );

    counter = 11;
    for ( size_t i = 0; i < shortVecX_DIM.size(); ++i )
    {
        TS_ASSERT_DELTA( shortVecX_DIM.at( i ), static_cast< double >( counter ), 0.0001 );
        counter += 1;
    }
}

void TestMatio::TestMadioDataCreationFromVector1D()
{
    std::vector< double > x = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    MatioData testData( x, "diga" );
    MatioData testData1( x, "diga.daten" );
    MatioData testData2( x, "diga.daten.Programmdauer" );

    TS_ASSERT_EQUALS( testData.mName, "" );
    TS_ASSERT_EQUALS( testData1.mName, "diga" );
    TS_ASSERT_EQUALS( testData2.mName, "diga.daten" );

    TS_ASSERT_EQUALS( testData.mMatlabVar->name, "diga" );
    TS_ASSERT_EQUALS( testData1.mMatlabVar->name, "daten" );
    TS_ASSERT_EQUALS( testData2.mMatlabVar->name, "Programmdauer" );

    std::vector< matvar_t * > vec = {testData.mMatlabVar, testData1.mMatlabVar, testData2.mMatlabVar};
    for ( size_t i = 0; i < vec.size(); ++i )
    {
        Mat_VarFree( vec[i] );
    }
}

void TestMatio::TestMadioDataCreationFromVector2D()
{
    std::vector< std::vector< double > > x = {{1, 2, 3, 4, 5, 6, 7, 8, 9}, {1, 2, 3, 4, 5, 6, 7, 8, 9}, {1, 2, 3, 4, 5, 6, 7, 8, 9}};
    MatioData testData( x, "diga" );
    MatioData testData1( x, "diga.daten" );
    MatioData testData2( x, "diga.daten.Programmdauer" );
    TS_ASSERT_EQUALS( testData.mName, "" );
    TS_ASSERT_EQUALS( testData1.mName, "diga" );
    TS_ASSERT_EQUALS( testData2.mName, "diga.daten" );

    TS_ASSERT_EQUALS( testData.mMatlabVar->name, "diga" );
    TS_ASSERT_EQUALS( testData1.mMatlabVar->name, "daten" );
    TS_ASSERT_EQUALS( testData2.mMatlabVar->name, "Programmdauer" );
    for ( size_t i = 0; i < x.size(); ++i )
    {
        for ( size_t j = 0; j < x[i].size(); ++j )
        {
            TS_ASSERT_EQUALS( x[i][j], testData( i, j ) );
            TS_ASSERT_EQUALS( x[i][j], testData1( i, j ) );
            TS_ASSERT_EQUALS( x[i][j], testData2( i, j ) );
        }
    }


    std::vector< matvar_t * > vec = {testData.mMatlabVar, testData1.mMatlabVar, testData2.mMatlabVar};
    for ( size_t i = 0; i < vec.size(); ++i )
    {
        Mat_VarFree( vec[i] );
    }
}


void TestMatio::TestMatFileWriteAndRead()
{
    std::string fileName = "selfWrittenStructWith2D.mat";

    {
        MatFile matfileRW( fileName.c_str(), MAT_ACC_RDWR );
        std::vector< std::vector< double > > x = {{1, 2, 3, 4, 5, 6, 7, 8, 9},
                                                  {1, 2, 3, 4, 5, 6, 7, 8, 9},
                                                  {1, 2, 3, 4, 5, 6, 7, 8, 9}};
        std::vector< double > y = {1, 2, 3, 4, 5, 6, 7, 8, 9};

        matfileRW << MatioData( y, "diga.daten.Programmdauer" );
        matfileRW << MatioData( x, "diga.daten.Strom" );
    }

    MatFile matfileRO( fileName.c_str(), MAT_ACC_RDONLY );

    MatioData &z = matfileRO["diga.daten.Strom"];

    for ( size_t x = 0; x < 2; ++x )
    {
        for ( size_t y = 0; y < 9; ++y )
        {
            TS_ASSERT_DELTA( static_cast< double >( y + 1 ), z( x, y ), 0.0001 );
        }
    }

    MatioData &a = matfileRO["diga.daten.Programmdauer"];
    for ( size_t y = 0; y < 9; ++y )
    {
        TS_ASSERT_DELTA( static_cast< double >( y + 1 ), a( 0, y ), 0.0001 );
    }
}
