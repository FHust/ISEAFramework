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
#ifndef _JAGGED_ARRAY_
#define _JAGGED_ARRAY_

#include <vector>
#include <boost/foreach.hpp>
#include <boost/scoped_array.hpp>


namespace thermal
{
using std::vector;

/// JaggedArray is an array of arrays adapted to fast sequential access, which is needed during simulation
template < typename Elem >
class JaggedArray
{
    public:
    ///@param[in] elemVecVec Data from elemVecVec is retrieved and put into an internal arrays
    explicit JaggedArray( vector< vector< Elem > > &elemVectorVector );
    /**
     * Begin returns the pointer to the beginnig of an array, which is also the end of the preceding array
     * @return The begin of the array with index arrayIndex, which is also the one before end pointer of tha array with
     * index - -arrayIndex
     */
    inline Elem *Begin( size_t arrayIndex ) const;
    inline size_t GetNumberOfArrays() const;

    private:
    size_t TotalNumberOfElements( const vector< vector< Elem > > &elemVecVec ) const;
    // Elem* FindFirstElement(vector<vector<Elem> > &elemVectorVector);
    const size_t mSize;
    boost::scoped_array< Elem * > mPointers;
    boost::scoped_array< Elem > mData;
};


template < typename Elem >
JaggedArray< Elem >::JaggedArray( vector< vector< Elem > > &elemVecVec )
    : mSize( elemVecVec.size() )
    , mPointers( new Elem *[elemVecVec.size() + 1] )
    , mData( new Elem[TotalNumberOfElements( elemVecVec )] )
{
    Elem *startPtr = mData.get();

    size_t j = 0;
    for ( size_t i = 0; i < elemVecVec.size(); ++i )
    {
        mPointers[i] = startPtr + j;

        BOOST_FOREACH ( const Elem &elem, elemVecVec[i] )
        {
            mData[j] = elem;
            ++j;
        }
    }
    mPointers[elemVecVec.size()] = startPtr + j;

    // elemVecVec.clear();
    // vector<vector<Elem> >(elemVecVec).swap(elemVecVec);
}

template < typename Elem >
Elem *JaggedArray< Elem >::Begin( size_t arrayIndex ) const
{
    return mPointers[arrayIndex];
}

template < typename Elem >
size_t JaggedArray< Elem >::GetNumberOfArrays() const
{
    return mSize;
}

template < typename Elem >
size_t JaggedArray< Elem >::TotalNumberOfElements( const vector< vector< Elem > > &elemVecVec ) const
{
    size_t sum = 0;
    BOOST_FOREACH ( const vector< Elem > &elemVec, elemVecVec )
        sum += elemVec.size();

    return sum;
}

// TODO: The below outcommented code handles the case if no scoped_array, but a vector is used to store the data
/*template<typename Elem>
Elem* JaggedArray<Elem>::FindFirstElement(vector<vector<Elem> > &elemVectorVector)
{
    bool foundAnElement = false;
    Elem* elemPtr = 0;

    BOOST_FOREACH(vector<Elem> &elemVector, elemVectorVector)
        BOOST_FOREACH(Elem &elem, elemVector)
        {
            elemPtr = &elem;
            foundAnElement = true;
            break;
        }

    if(!foundAnElement)
        return 0;

    mData.push_back(*elemPtr);
    elemPtr = &mData[0];
    mData.pop_back();
    return elemPtr;
}*/

/*template<typename Elem>
JaggedArray<Elem>::JaggedArray(vector<vector<Elem> > &elemVecVec)
    : mSize(elemVecVec.size())
    , mPointers(elemVecVec.size() + 1)
{
    mData.reserve(TotalNumberOfElements(elemVecVec));
    Elem *startPtr = FindFirstElement(elemVecVec);

    size_t j = 0;
    for(size_t i = 0; i < elemVecVec.size(); ++i)
    {
        mPointers[i] = startPtr + j;

        BOOST_FOREACH(const Elem &elem, elemVecVec[i])
        {
            mData.push_back(elem);
            ++j;
        }
    }
    mPointers[elemVecVec.size()] = startPtr + j;

    elemVecVec.clear();
    vector<vector<Elem> >(elemVecVec).swap(elemVecVec);
}*/
}
#endif
