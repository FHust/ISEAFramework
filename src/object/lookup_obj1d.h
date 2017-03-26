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
* File Name : lookup_obj1d.h
* Creation Date : 31-10-2012
* Last Modified : Di 25 Nov 2014 11:54:21 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _LOOKUP_OBJ_
#define _LOOKUP_OBJ_
// ETC
#include "../lookup/lookup.h"
#include "../operators/vectorOperator.h"

#include "object.h"


namespace object
{

/// This Object has a 1D Lookup which can be called with GetValue(const double lookuppoint) or operator(const double
/// lookuppoint)
template < typename T >
class LookupObj1D : public Object< T >
{
    public:
    LookupObj1D( const ::std::vector< T >& lookupData, const ::std::vector< T >& measurementPoints,
                 lookup::LookupType type = lookup::LINEAR_INTERPOLATION );
    LookupObj1D( const ::std::vector< T >& lookupData, const ::std::vector< T >& measurementPoints,
                 LookupObj1D* CloneObj, lookup::LookupType type = lookup::LINEAR_INTERPOLATION );

    virtual ~LookupObj1D(){};

    virtual T GetValue() const;      ///< Return the last done lookup
    virtual T operator()() const;    ///< Return the last done lookup

    virtual T GetValue( const T lookuppoint );    ///< Calculate the lookup value and store it for a faster return in the next call with ()
    virtual T operator()( const T lookuppoint );    ///< Calculate the lookup value and store it for a faster return in the next call with ()
    inline virtual T GetMaxValueOfLookup() { return mLookup.GetMaxValueOfLookup(); }
    inline virtual T GetMinValueOfLookup() { return mLookup.GetMinValueOfLookup(); }

    inline const lookup::Lookup1D< T >& GetLookup() const { return mLookup; }

    virtual const char* GetName() const;

    private:
    protected:
    const lookup::Lookup1D< T > mLookup;
};

template < typename T >
LookupObj1D< T >::LookupObj1D( const ::std::vector< T >& lookupData, const ::std::vector< T >& measurementPoints, lookup::LookupType type )
    : Object< T >()
    , mLookup( lookupData, measurementPoints, type )
{
}

template < typename T >
LookupObj1D< T >::LookupObj1D( const ::std::vector< T >& lookupData, const ::std::vector< T >& measurementPoints,
                               LookupObj1D< T >* CloneObj, lookup::LookupType type )
    : Object< T >()
    , mLookup( ScalarVectorMultiplication( lookupData, CloneObj->mLookup.GetLookup() ), measurementPoints, type )
{
}

template < typename T >
T LookupObj1D< T >::GetValue( const T lookuppoint )
{
    this->mLastValue = mLookup( lookuppoint );
    return this->mLastValue;
}

template < typename T >
T LookupObj1D< T >::operator()( const T lookuppoint )
{
    this->mLastValue = mLookup( lookuppoint );
    return this->mLastValue;
}

template < typename T >
T LookupObj1D< T >::GetValue() const
{
    return this->mLastValue;
}

template < typename T >
T LookupObj1D< T >::operator()() const
{
    return this->mLastValue;
}

template < typename T >
const char* LookupObj1D< T >::GetName() const
{
    return "LookupObj1D";
}

} /* END NAMESPACE */
#endif /* _LOOKUP_OBJ_ */
