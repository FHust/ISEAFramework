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
#ifndef _MATERIAL_
#define _MATERIAL_

#include "../../exceptions/error_proto.h"


/// thermal contains physical information of thermal part of the model
namespace thermal
{

/// Material contains information about thermal behaviour of a material
template < typename T = double >
class Material
{
    public:
    /// MaterialType describes the (coordinate system of the) anisotropy of the thermal conductivity of a material
    enum MaterialType
    {
        ISOTROPIC,
        CYLINDRIC,
        CARTESIAN,
        NOT_SPECIFIED
    };
    /**
     * density Density in kg/m^3
     * specificCapacity Specific thermal capacity in J/(kg*K)
     * conductivity0,conductivity1,conductivity2  Thermal conductivity in W/(m*K). 0, 1 or 2 map the three coordinates
     * of the cartesian (x,y,z) or the cylindric (rho,phi,z) coordinate system.
     * type The MaterialType of this instance
     */
    Material( T density, T specificCapacity, T conductivity0, T conductivity1, T conductivity2, MaterialType type = NOT_SPECIFIED );
    virtual ~Material();
    T GetDensity() const;
    T GetSpecificCapacity() const;
    ///@param[in] index Must be 0, 1 or 2 mapping the three coordinates of the cartesian (x,y,z) or the cylindric
    ///(rho,phi,z) coordinate system. Throws exception if bigger than 2.
    T GetConductivity( size_t index ) const;
    /// Returns MaterialType of this instance
    MaterialType GetMaterialType() const;

    private:
    const T mDensity;
    const T mSpecificCapacity;
    T mConductivity[3];
    MaterialType mType;
};


template < typename T >
Material< T >::Material( T density, T specificCapacity, T conductivity0, T conductivity1, T conductivity2, MaterialType type )
    : mDensity( density )
    , mSpecificCapacity( specificCapacity )
    , mType( type )
{
    if ( density <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DensityNegative" );
    if ( specificCapacity <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SpecificCapacityNegative" );
    if ( conductivity0 <= 0.0 || conductivity1 <= 0.0 || conductivity2 <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ConductivitiesNegative" );
    if ( mType > NOT_SPECIFIED )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnvalidType" );


    mConductivity[0] = conductivity0;
    mConductivity[1] = conductivity1;
    mConductivity[2] = conductivity2;
}


template < typename T >
Material< T >::~Material()
{
}

template < typename T >
T Material< T >::GetDensity() const
{
    return mDensity;
}

template < typename T >
T Material< T >::GetSpecificCapacity() const
{
    return mSpecificCapacity;
}

template < typename T >
T Material< T >::GetConductivity( size_t index ) const
{
    if ( index > 2 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "IndexForGetConductivityBigger3" );
    return mConductivity[index];
}

template < typename T >
typename Material< T >::MaterialType Material< T >::GetMaterialType() const
{
    return mType;
}
}
#endif
