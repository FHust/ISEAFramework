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
* File Name : TestMatioWriteTree.cpp
* Creation Date : 03-08-2015
* Last Modified : Do 13 Aug 2015 10:14:53 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestMatioWriteTree.h"
#include "../../container/matio_file.h"
#include "../../container/matioWriteTree.h"

using namespace matlab;

void TestMatioWriteTree::TestMatioData()
{
    MatFile matfileRO( "structWith2D.mat", MAT_ACC_RDONLY );
    std::vector< double > x = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    MatioData data_element1( x, "diga.daten.Programmdauer" );
    MatioData data_element2( x, "diga.daten.Strom" );
    MatioData data_element3( x, "diga.daten.Spannung" );

    std::vector< MatioData > data = {data_element1, data_element2, data_element3};
    MatioWriteTree tree( data );
    auto entry = tree.mRootNode.mTree.find( "diga" );
    TS_ASSERT( entry != tree.mRootNode.mTree.end() );
    auto nextEntry = tree.mRootNode["diga"].mTree.find( "daten" );
    TS_ASSERT( nextEntry != tree.mRootNode["diga"].mTree.end() );

    std::vector< matvar_t * > matvar_data = tree.GetLinearizedTree();
    std::vector< std::string > fields = {"Programmdauer", "Strom", "Spannung"};
    for ( size_t j = 0; j < fields.size(); ++j )
    {
        auto nextNode = tree.mRootNode["diga"]["daten"].mNodes.find( fields.at( j ) );
        for ( size_t i = 0; i < 9; ++i )
        {
            TS_ASSERT_EQUALS( ( *nextNode->second )( 0, i ), i + 1 );
        }
    }
    for ( size_t i = 0; i < matvar_data.size(); ++i )
    {
        Mat_VarFree( matvar_data.at( i ) );
    }
}


void TestMatioWriteTree::TestTree()
{
    MatFile matfileRO( "structWith2D.mat", MAT_ACC_RDONLY );
    std::vector< double > x = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    MatioData data_element1( x, "diga.daten.Programmdauer" );
    MatioData data_element2( x, "diga.daten.Strom" );
    MatioData data_element3( x, "diga.daten.Spannung" );
    MatioData data_element4( x, "x" );

    std::vector< MatioData > data = {data_element1, data_element2, data_element3, data_element4};
    MatioWriteTree tree( data );
    std::vector< matvar_t * > matvar_data = tree.GetLinearizedTree();

    // For manual check
    // mat_t *mfile = Mat_Create( "struct_out.mat", NULL );
    // for ( size_t i = 0; i < matvar_data.size(); ++i )
    //{
    //    Mat_VarWrite( mfile, matvar_data.at( i ), MAT_COMPRESSION_NONE );
    //}
    // Mat_Close( mfile );

    MatioData exData1( matvar_data.at( 0 ) );    // <-- diga
    MatioData exData2( matvar_data.at( 1 ) );    // <-- x

    MatioData ref1 = exData1["daten.Strom"];
    MatioData ref2 = exData1["daten.Programmdauer"];
    MatioData ref3 = exData1["daten.Spannung"];
    std::vector< MatioData > vec = {ref1, ref2, ref3, exData2};
    for ( size_t i = 0; i < vec.size(); ++i )
    {
        for ( size_t j = 0; j < 9; ++j )
        {
            TS_ASSERT_EQUALS( vec.at( i )( 0, j ), j + 1 );
        }
    }

    for ( size_t i = 0; i < matvar_data.size(); ++i )
    {
        Mat_VarFree( matvar_data.at( i ) );
    }
}

void TestMatioWriteTree::TestTreeOtherOrder()
{
    MatFile matfileRO( "structWith2D.mat", MAT_ACC_RDONLY );
    std::vector< double > x = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector< double > y = {10, 20, 30, 40, 50, 60, 70, 80, 90};
    MatioData data_element1( x, "diga.daten.Programmdauer" );
    MatioData data_element2( x, "diga.daten.Strom" );
    MatioData data_element3( x, "diga.daten.Spannung" );
    MatioData data_element4( y, "x" );

    std::vector< MatioData > data = {data_element4, data_element3, data_element2, data_element1};
    MatioWriteTree tree( data );
    std::vector< matvar_t * > matvar_data = tree.GetLinearizedTree();

    // For manual check
    // mat_t *mfile = Mat_Create( "struct_out.mat", NULL );
    // for ( size_t i = 0; i < matvar_data.size(); ++i )
    //{
    //    Mat_VarWrite( mfile, matvar_data.at( i ), MAT_COMPRESSION_NONE );
    //}
    // Mat_Close( mfile );

    MatioData exData1( matvar_data.at( 0 ) );    // <-- diga
    MatioData exData2( matvar_data.at( 1 ) );    // <-- x

    MatioData ref1 = exData1["daten.Strom"];
    MatioData ref2 = exData1["daten.Programmdauer"];
    MatioData ref3 = exData1["daten.Spannung"];
    std::vector< MatioData > vec = {ref1, ref2, ref3};
    for ( size_t i = 0; i < vec.size(); ++i )
    {
        for ( size_t j = 0; j < 9; ++j )
        {
            TS_ASSERT_EQUALS( vec.at( i )( 0, j ), j + 1 );
        }
    }
    for ( size_t j = 0; j < 9; ++j )
    {
        TS_ASSERT_EQUALS( exData2( 0, j ), ( j + 1 ) * 10 );
    }


    for ( size_t i = 0; i < matvar_data.size(); ++i )
    {
        Mat_VarFree( matvar_data.at( i ) );
    }
}
