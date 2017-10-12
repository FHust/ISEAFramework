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
* File Name : sphericalDiffusion.h
* Creation Date : 22-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SERICALDIFFUSION_
#define _SERICALDIFFUSION_
// STD
#include <numeric>

// ETC
#include "twoport.h"
#include "../object/object.h"

extern template class electrical::TwoPort< myMatrixType >;
extern template class object::Object< double >;

namespace electrical
{

/// This class defines the spherical diffusion derived by Heiko Witzenhausen in particles mostly used on the anode
template < typename T = myMatrixType >
class SphericalDiffusion : public TwoPort< T >
{
    public:
    explicit SphericalDiffusion(
     boost::shared_ptr< object::Object< double > > rObj, boost::shared_ptr< object::Object< double > > tauObj,
     size_t rcElements, int hasCapacity = 1, const bool observable = false,
     typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType(new ElectricalDataStruct< ScalarUnit >))
        : TwoPort< T >( observable, dataValues )
        , mHasCapacity( hasCapacity )
        , mUIDCount( rcElements )
        , mResidualResistanceFactor( std::accumulate( mRFactor.begin() + rcElements, mRFactor.end(), 0.0 ) )
        , mR( rObj )
        , mTau( tauObj ){};

    explicit SphericalDiffusion( object::Object< double >* rObj, object::Object< double >* tauObj, size_t rcElements,
                                 int hasCapacity = 1, const bool observable = false,
                                 typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType(new ElectricalDataStruct< ScalarUnit >))
        : SphericalDiffusion( boost::shared_ptr< object::Object< double > >( rObj ),
                              boost::shared_ptr< object::Object< double > >( tauObj ), rcElements, hasCapacity,
                              observable, dataValues ){};    // delegating constructor

    virtual ~SphericalDiffusion(){};

    virtual ScalarUnit GetValue() const { return mR->GetValue(); };    ///< Get Value of Object

    const object::Object< double >* GetObject() const
    {
        return mR.get();    ///< Get Object if instance is const
    }
    virtual const char* GetName() const { return "SphericalDiffusion"; };

    void SetVoltage();
    virtual T* GetVoltage();    ///< Return a Voltage over the TwoPort including children

    virtual void SetCurrent( const T currentval );    ///< Sets the current value
    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );

    const static std::vector< double > mRFactor;
    const static std::vector< double > mTauFactor;

    private:
    int mHasCapacity = 0;
    const size_t mUIDCount = 0;

    const ScalarUnit mResidualResistanceFactor = ScalarUnit();
    T mConstVoltageVector = T();
    T mDGLPart = T();

    std::vector< size_t > mUIDs;
    boost::shared_ptr< object::Object< double > > mR;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
    boost::shared_ptr< object::Object< double > > mTau;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
};

template < typename T >
void SphericalDiffusion< T >::SetVoltage()
{
    mConstVoltageVector.zeros( this->mCurrent.n_rows, this->mCurrent.n_cols );
    for ( size_t i = 0; i < mUIDCount; ++i )
    {
        mConstVoltageVector( 0, mUIDs[i] ) = 1;    // RCs
    }

    if ( mHasCapacity )
        mConstVoltageVector( 0, mUIDs.back() ) = 1;    // C

    this->mVoltage = mConstVoltageVector;
    mDGLPart.zeros( this->mCurrent.n_rows, this->mCurrent.n_cols );
}


template < typename T >
T* SphericalDiffusion< T >::GetVoltage()
{
    const double r = mR->GetValue();
    if ( this->HasSystem() )
    {
        const double inv_c = r / mTau->GetValue();
        const double neg_inv_tau = -1.0 / mTau->GetValue();
        for ( size_t i = 0; i < mUIDCount; ++i )
        {
            mDGLPart = this->mCurrent * ( inv_c * ( mRFactor[i] / mTauFactor[i] ) );
            mDGLPart( this->mCurrent.n_rows - 1, mUIDs[i] ) += neg_inv_tau / mTauFactor[i];
            this->mStateSystemGroup->mDglStateSystem.AddEquations( mUIDs[i], mDGLPart );
        }

        if ( mHasCapacity )
        {
            mDGLPart = ( mHasCapacity * this->mCurrent ) * ( inv_c * ( ( 3.0 / 2.0 ) ) );
            this->mStateSystemGroup->mDglStateSystem.AddEquations( mUIDs.back(), mDGLPart );
        }
    }
    this->mVoltage = mConstVoltageVector + this->mCurrent * mResidualResistanceFactor * r;
    return &this->mVoltage;
}

template < typename T >
void SphericalDiffusion< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{
    for ( size_t i = 0; i < mUIDCount; ++i )
    {
        mUIDs.push_back( stateSystemGroup->mDglStateSystem.GetNewEquation() );    // One per RC-element
    }

    if ( mHasCapacity )
        mUIDs.push_back( stateSystemGroup->mDglStateSystem.GetNewEquation() );    // One capacity at the end

    TwoPort< T >::SetSystem( stateSystemGroup );
}

template < typename T >
void SphericalDiffusion< T >::SetCurrent( const T current )
{
    TwoPort< T >::SetCurrent( current );
    SetVoltage();
}
}

#endif /* _SERICALDIFFUSION_ */
