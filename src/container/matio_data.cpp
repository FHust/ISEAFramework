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
* File Name : matio_data.cpp
* Creation Date : 06-07-2015
* Last Modified : Mo 06 Jun 2016 13:24:59 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "matio_data.h"

// STD
#include <exception>

// BOOST
#include <boost/algorithm/string.hpp>

// ETC
#include "../exceptions/error_proto.h"

namespace matlab
{

MatioData::MatioData( matvar_t *matvar )
    : mMatlabVar( matvar )
{
    if ( !matvar )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "matioEmptyVariable" );
    mName = matvar->name;
};

MatioData::MatioData( std::vector< double > &data, std::string name )
{
    size_t dims[2] = {1, data.size()};
    mMatlabVar = Mat_VarCreate( SetNames( name ).c_str(), MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, &data[0], 0 );
}


MatioData::MatioData( std::vector< std::vector< double > > &data, std::string name )
{

    for ( size_t i = 1; i < data.size(); ++i )
    {
        if ( data[i].size() != data[0].size() )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "matioEmptyVariable", data[i].size(),
                                                 data[0].size() );
    }


    size_t dims[2] = {data.size(), data.at( 0 ).size()};
    std::vector< double > finalVec( dims[0] * dims[1] );

    for ( size_t j = 0; j < dims[0]; ++j )
    {
        for ( size_t i = 0; i < dims[1]; ++i )
        {
            finalVec[j + i * dims[0]] = data[j][i];
        }
    }

    mMatlabVar = Mat_VarCreate( SetNames( name ).c_str(), MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, &finalVec[0], 0 );
}

std::string MatioData::SetNames( std::string &name )
{
    size_t found = name.find_last_of( "." );

    std::string matvarName;
    if ( found == std::string::npos )
    {
        mName = "";
        matvarName = name;
    }
    else
    {
        std::string tmpstring = name.substr( 0, found );    // <-- can't be done without the tmp variable
        mName = tmpstring.c_str();
        matvarName = name.substr( found + 1, std::string::npos );    // +1 because of '.'
    }
    name = matvarName;
    return name;
}

void MatioData::CheckSanity() const
{
    if ( !mMatlabVar )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "matioEmptyVariable" );

    if ( mMatlabVar->rank != 2 )
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "unsupportedDimension" );
    }
}

double &MatioData::operator()( size_t x, size_t y )
{
    CheckSanity();
    CheckOutOfBoundAccess( x, y );
    size_t offset = ( mMatlabVar->dims[X_DIM] ) * y + x;
    return static_cast< double * >( mMatlabVar->data )[offset];
}

void MatioData::CheckOutOfBoundAccess( size_t x, size_t y ) const
{

    if ( x > mMatlabVar->dims[X_DIM] )
        ErrorFunction< std::out_of_range >( __FUNCTION__, __LINE__, __FILE__, "OutOfBound", x, mMatlabVar->dims[X_DIM] );

    if ( y > mMatlabVar->dims[Y_DIM] )
        ErrorFunction< std::out_of_range >( __FUNCTION__, __LINE__, __FILE__, "OutOfBound", y, mMatlabVar->dims[Y_DIM] );
}

double MatioData::operator()( size_t x, size_t y ) const
{
    CheckSanity();
    CheckOutOfBoundAccess( x, y );
    size_t offset = ( mMatlabVar->dims[X_DIM] ) * y + x;
    return static_cast< double * >( mMatlabVar->data )[offset];
}

MatioData &MatioData::operator[]( std::string variableName )
{
    if ( !mMatlabVar )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "matioEmptyVariable" );

    std::vector< std::string > tokens;
    boost::split( tokens, variableName, boost::is_any_of( "." ) );

    if ( tokens.empty() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyToken" );

    matvar_t *cell = Mat_VarGetStructFieldByName( mMatlabVar, tokens[0].c_str(), 0 );

    if ( !cell )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnfindableToken", tokens[0].c_str() );


    for ( size_t i = 1; i < tokens.size(); ++i )
    {

        cell = Mat_VarGetStructFieldByName( cell, tokens[i].c_str(), 0 );
        if ( !cell )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnfindableToken", tokens[i].c_str() );
    }

    mReturnMember.reset( new MatioData( cell ) );
    return *mReturnMember;
}

} /* matlab */
