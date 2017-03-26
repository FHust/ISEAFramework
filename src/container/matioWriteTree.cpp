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
* File Name : matioWriteTree.cpp
* Creation Date : 03-08-2015
* Last Modified : Mo 06 Jun 2016 13:23:38 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

// BOOST
#include <boost/algorithm/string.hpp>

// ETC
#include "matio.h"
#include "matioWriteTree.h"
#include "../exceptions/error_proto.h"

namespace matlab
{
MatioWriteTree::MatioWriteTree( std::vector< MatioData > &matioVec )
    : mData( matioVec )
{
    CreateTree();
    mLinearizedTree = StartRecursionForWrite();
}

void MatioWriteTree::CreateTree()
{
    //    mRootNode
    for ( size_t i = 0; i < mData.size(); ++i )
    {
        std::vector< std::string > tokens;
        boost::split( tokens, mData[i].mName, boost::is_any_of( "." ) );
        if ( tokens.empty() )
        {
            continue;
        }
        // es kann sein, dass hier ein token mit "" entsteht, dieses müssen wir für die
        // korrekte weiterverarbeitung rauslöschen

        tokens.erase( std::remove( tokens.begin(), tokens.end(), "" ), tokens.end() );
        RecurseOverLeaf( tokens, mRootNode, mData[i] );
    }
}

void MatioWriteTree::RecurseOverLeaf( std::vector< std::string > &tokens, Leafs &leaf, const MatioData &data )
{

    if ( !data.mMatlabVar )
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "matioCreation" );
    }

    std::string currentLeafName;
    if ( tokens.empty() )
    {
        currentLeafName = data.mMatlabVar->name;
        leaf.mNodes[currentLeafName] = &data;
        return;
    }
    else
    {
        currentLeafName = tokens.front();
    }

    tokens.erase( tokens.begin() );    // linear time access due to move of all other elements :/
    RecurseOverLeaf( tokens, leaf[currentLeafName], data );
}

std::vector< matvar_t * > MatioWriteTree::StartRecursionForWrite()
{

    std::vector< matvar_t * > data;
    for ( std::map< std::string, Leafs >::const_iterator it = mRootNode.mTree.begin(); it != mRootNode.mTree.end(); ++it )
    {
        data.push_back( RecurseForWrite( it->second, it->first ) );
    }

    for ( std::map< std::string, const MatioData * >::const_iterator it = mRootNode.mNodes.begin();
          it != mRootNode.mNodes.end(); ++it )
    {
        data.push_back( it->second->mMatlabVar );
    }
    data.push_back( 0 );

    return data;
}

matvar_t *MatioWriteTree::RecurseForWrite( const Leafs &leaf, std::string name )
{
    std::vector< matvar_t * > data;
    for ( std::map< std::string, Leafs >::const_iterator it = leaf.mTree.begin(); it != leaf.mTree.end(); ++it )
    {
        data.push_back( RecurseForWrite( it->second, it->first ) );
    }

    for ( std::map< std::string, const MatioData * >::const_iterator it = leaf.mNodes.begin(); it != leaf.mNodes.end(); ++it )
    {
        data.push_back( it->second->mMatlabVar );
    }
    size_t dims[2] = {1, 1};    // matlab structs are defined with a dimension {1,1}. This dimension does not have
                                // anything to do with contained elements of the struct
    data.push_back( 0 );
    matvar_t *mat = Mat_VarCreate( name.c_str(), MAT_C_STRUCT, MAT_T_STRUCT, 2, dims, &data[0], 0 );

    return mat;
}

} /* matlab */
