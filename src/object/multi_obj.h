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
* File Name : multi_obj.h
* Creation Date : 25-11-2016
* Last Modified : Mo 28 Nov 2016 10:56:36 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MULTI_OBJ_
#define _MULTI_OBJ_

// STD
#include <vector>

// boost
#include <boost/shared_ptr.hpp>

// ETC
#include "object.h"

namespace object
{

template < typename T = double >
class MultiObj : public Object< T >
{
    public:
    MultiObj( std::vector< boost::shared_ptr< Object< T > > > &vecObj )
        : Object< T >()
        , mVecObj( vecObj ){};
    virtual ~MultiObj(){};

    virtual T GetValue() const = 0;      //{ return T(); };
    virtual T operator()() const = 0;    //{ return T(); };

    private:
    protected:
    std::vector< boost::shared_ptr< Object< T > > > mVecObj;
};

template < typename T >
class AdderObj : public MultiObj< T >
{
    public:
    AdderObj( std::vector< boost::shared_ptr< Object< T > > > &vecObj )
        : MultiObj< T >( vecObj ){};

    //    AdderObj( std::vector< boost::shared_ptr< Object< T > > > &vecObj, AdderObj< T > *CloneObj );

    virtual ~AdderObj(){};
    virtual T GetValue() const { return Add(); };
    virtual T operator()() const { return Add(); };

    private:
    T Add() const
    {
        T returnValue = 0;
        for ( size_t i = 0; i < this->mVecObj.size(); ++i )
        {
            returnValue += this->mVecObj[i]->GetValue();
        }
        return returnValue;
    }
};

template < typename T >
class MultiplyObj : public MultiObj< T >
{
    public:
    MultiplyObj( std::vector< boost::shared_ptr< Object< T > > > &vecObj )
        : MultiObj< T >( vecObj ){};

    //    MultiplyObj( std::vector< boost::shared_ptr< Object< T > > > &vecObj, MultiplyObj< T > *CloneObj );

    virtual ~MultiplyObj(){};
    virtual T GetValue() const { return Multiply(); };
    virtual T operator()() const { return Multiply(); };

    private:
    T Multiply() const
    {
        T returnValue = 1;

        for ( size_t i = 0; i < this->mVecObj.size(); ++i )
        {
            returnValue *= this->mVecObj[i]->GetValue();
        }
        return returnValue;
    }
};

template < typename T >
class DivideObj : public MultiObj< T >
{
    public:
    DivideObj( std::vector< boost::shared_ptr< Object< T > > > &vecObj )
        : MultiObj< T >( vecObj )
    {
        if ( vecObj.size() <= 1 )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnsupportedOperation",
                                                 "GetMaxValueOfLookup" );
        }
    }

    //    DivideObj( std::vector< boost::shared_ptr< Object< T > > > &vecObj, DivideObj< T > *CloneObj );

    virtual ~DivideObj(){};
    virtual T GetValue() const { return Divide(); };
    virtual T operator()() const { return Divide(); };

    private:
    T Divide() const
    {
        T returnValue = this->mVecObj[0]->GetValue();
        for ( size_t i = 1; i < this->mVecObj.size(); ++i )
        {
            returnValue /= this->mVecObj[i]->GetValue();
        }
        return returnValue;
    }
};
}
/// DESCRIPTION
#endif /* _MULTI_OBJ_ */
