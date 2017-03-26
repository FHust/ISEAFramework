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
* File Name : matioWriteTree.h
* Creation Date : 03-08-2015
* Last Modified : Mo 06 Jun 2016 13:23:12 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MATIOWRITETREE_
#define _MATIOWRITETREE_

// STD
#include <string>
#include <vector>
#include <map>

// ETC
#include "matio_data.h"

struct matvar_t;

class TestMatioWriteTree;
namespace matlab
{

/// Helper class for creating a tree which differentiates between data and structs
struct Leafs
{
    std::map< std::string, Leafs > mTree;
    std::map< std::string, const MatioData * > mNodes;
    /// for easy access to subtrees
    Leafs &operator[]( std::string name ) { return mTree[name]; };
};


/// This class creates a tree from Matio data.
/// The tree is devided in structs (leafs) and data (nodes).
/// This class is not copyable due to problems with the ownership of mLinearizedTree and the cotained data.
class MatioWriteTree : private boost::noncopyable
{

    friend class ::TestMatioWriteTree;

    public:
    /// This c-tor creates a tree from the MatioData vector that is provided with a depth first algorithm
    MatioWriteTree( std::vector< MatioData > &matioVec );

    /// Returns a LinearizedTree that can be written by a linear acces from 0 to n
    std::vector< matvar_t * > GetLinearizedTree() const { return mLinearizedTree; };

    private:
    /// This function triggers the algorithm to span the tree with the provided matioData
    void CreateTree();

    /// A recursive function that tries to create all leafs based on the tokens provided and the data
    /// It creates a tree which can span other trees or other data. This helps later to differntiate between data and
    /// structure
    /// \param tokens a vector with the place where all data should be written ["A.B.C.data"]
    /// \param data the data that should be written to structure
    void RecurseOverLeaf( std::vector< std::string > &tokens, Leafs &leaf, const MatioData &data );

    /// StartRecursionForWrite is the entrypoint for the recursion algorithm to write structs and data
    /// The Tree that has been written by RecurseOverLeaf and CreateTree and is evalutated by StartRecursionForWrite
    /// A vector with the linearized tree is returned
    std::vector< matvar_t * > StartRecursionForWrite();

    /// This function returns structs and data encapseled in the matvar_t type, which can then be used to create the
    /// tree like structure of matfiles
    matvar_t *RecurseForWrite( const Leafs &leaf, std::string name );


    std::vector< MatioData > &mData;
    Leafs mRootNode;
    std::vector< matvar_t * > mLinearizedTree;

    protected:
};

} /* matlab */
#endif /* _MATIOWRITETREE_ */
