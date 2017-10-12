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
* File Name : zarc.h
* Creation Date : 11-11-2012
* Last Modified : Do 10 Mär 2016 13:01:10 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ZARC_
#define _ZARC_

#include "twoport.h"
#include <boost/shared_ptr.hpp>

// STD
#include <cmath>
#ifdef __EXCEPTIONS__
#include <stdexcept>
#endif /* __EXCEPTIONS__ */

#include "../object/object.h"
#include "../object/lookup_obj1d.h"
#include "../operators/vectorOperator.h"
#include "../states/soc.h"
#include "../states/thermal_state.h"

// friend
class TestZarc;
class TestXML;
class TestFactoryLookUpCellElements;
class TestElectricalFactory;

namespace electrical
{

/// This TwoPort describes the behaviour of a zarc element (3 parallelRC)
template < typename T = myMatrixType >
class Zarc : public TwoPort< T >
{
    friend class ::TestZarc;
    friend class ::TestXML;
    friend class ::TestFactoryLookUpCellElements;
    friend class ::TestElectricalFactory;

    public:
    explicit Zarc( boost::shared_ptr< object::Object< double > > lookupRelaxationTime,
                   boost::shared_ptr< object::Object< double > > lookupOhmicResistance,
                   boost::shared_ptr< object::Object< double > > lookupPhi, const double samplingRate = 0.001,
                   const bool observable = false,
                   typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType(new ElectricalDataStruct< ScalarUnit >));    ///< The zarc element needs a thermal and an electrical state to be created, and does not take care of these. These  are just beeing utelized for the lookups

    virtual ~Zarc(){};

    virtual void SetCurrent( const T current );    ///< Sets the current

    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );
    virtual T* GetVoltage();    ///< Returns the voltage over the Zarc element
    virtual void CalculateStateDependentValues();

    const std::vector< ScalarUnit >& GetRValues() const;
    const std::vector< ScalarUnit >& GetCValues() const;

    const std::vector< double >& GetTau() const;

    virtual const char* GetName() const;
    size_t GetNumberOfElements() const;

    const std::vector< ScalarUnit >& GetVoltageValues() const;

    inline const object::Object< double >* GetRObject() const { return mLookupOhmicResistance.get(); }
    inline const object::Object< double >* GetTauObject() const { return mLookupRelaxationTime.get(); }
    inline const object::Object< double >* GetPhiObject() const { return mLookupPhi.get(); }

    inline const lookup::Lookup1D< double >& GetLookupPhi2TauFactor() const { return mLookupPhi2TauFactor; }
    inline const lookup::Lookup1D< double >& GetLookupPhi2RFactorInnen() const { return mLookupPhi2RFactorInnen; }
    inline const lookup::Lookup1D< double >& GetLookupPhi2RFactorAussen() const { return mLookupPhi2RFactorAussen; }

    private:
    struct RcElements
    {
        ScalarUnit rFactorAussen, rFactorInnen, tauFactor, const_r, const_tau;
    };
    RcElements mCurrentRcElements;
    typedef void ( Zarc::*rcImplementation )( const RcElements& rc, T& voltageEquation );
    typedef void ( Zarc::*ohmicResistanceFunction )();

    void InitializeZarc( const double samplingRate );
    void FirstRcVoltage( const RcElements& rc, T& voltageEquation );
    void SecondRcVoltage( const RcElements& rc, T& voltageEquation );
    void ThirdRcVoltage( const RcElements& rc, T& voltageEquation );

    void AddOhmicResistanceToVoltage1R();
    void AddOhmicResistanceToVoltage2R();
    void AddOhmicResistanceToVoltage3R();

    void SetVoltage();
    void CalculateZarcElements( const double samplingRate );

    T mZarcVoltageValue;
    T mVoltageEquation;
    std::vector< ScalarUnit > mRValues;
    std::vector< ScalarUnit > mCValues;

    void CalculateLookupValues();
    mutable std::vector< ScalarUnit > mVoltageValues;    // ugly with mutable

    boost::shared_ptr< object::Object< double > > mLookupOhmicResistance;
    boost::shared_ptr< object::Object< double > > mLookupRelaxationTime;
    boost::shared_ptr< object::Object< double > > mLookupPhi;

    const lookup::Lookup1D< double > mLookupPhi2TauFactor;
    const lookup::Lookup1D< double > mLookupPhi2RFactorInnen;
    const lookup::Lookup1D< double > mLookupPhi2RFactorAussen;

    std::vector< size_t > mUIDs;
    std::vector< rcImplementation > mFunctionVector;
    std::vector< ohmicResistanceFunction > mOhmicFunctionVector;
    std::vector< double > mTau;

    static const double mPhi[];
    static const double mTauFactor[];
    static const double mRFactorAussen[];
    static const double mRFactorInnen[];

    size_t mNumberOfElements;
    static const size_t THREE_TAU = 3;
    static const size_t MAX_RC_ELEMENTS = 3;

    protected:
};

template < typename T >
Zarc< T >::Zarc( boost::shared_ptr< object::Object< double > > lookupRelaxationTime,
                 boost::shared_ptr< object::Object< double > > lookupOhmicResistance,
                 boost::shared_ptr< object::Object< double > > lookupPhi, const double dtValue, const bool observable,
                 typename TwoPort< T >::DataType dataValues )
    : TwoPort< T >( observable, dataValues )
    , mZarcVoltageValue( T() )
    , mVoltageEquation( T() )
    , mVoltageValues( MAX_RC_ELEMENTS )
    , mLookupOhmicResistance( lookupOhmicResistance )
    , mLookupRelaxationTime( lookupRelaxationTime )
    , mLookupPhi( lookupPhi )
    , mLookupPhi2TauFactor( std::vector< double >( mTauFactor, mTauFactor + 10 ),
                            std::vector< double >( mPhi, mPhi + 10 ), lookup::LINEAR_INTERPOLATION )
    , mLookupPhi2RFactorInnen( std::vector< double >( mRFactorInnen, mRFactorInnen + 10 ),
                               std::vector< double >( mPhi, mPhi + 10 ), lookup::LINEAR_INTERPOLATION )
    , mLookupPhi2RFactorAussen( std::vector< double >( mRFactorAussen, mRFactorAussen + 10 ),
                                std::vector< double >( mPhi, mPhi + 10 ), lookup::LINEAR_INTERPOLATION )
    , mNumberOfElements( 0 )
{
    InitializeZarc( dtValue );
}

template < typename T >
void Zarc< T >::CalculateStateDependentValues()
{
    GetVoltageValues();

    if ( this->mObservable )
    {
        this->CalculateCurrentValue();
        this->CalculateVoltageValue();
    }

    this->mPowerValue = 0;

    for ( size_t i = 0; i < mNumberOfElements; ++i )
    {
        this->mPowerValue += ::std::pow( mVoltageValues[i], 2.0 ) / mRValues[i];
    }

    if ( mNumberOfElements < 3 )
    {
        if ( !this->mObservable )
            this->CalculateCurrentValue();

        this->mPowerValue += ::std::pow( this->mCurrentValue, 2 ) * mRValues.at( mNumberOfElements );
    }
}

template < typename T >
void Zarc< T >::InitializeZarc( const double samplingRate )
{
    CalculateZarcElements( samplingRate );
    mFunctionVector.push_back( &Zarc< T >::FirstRcVoltage );
    mFunctionVector.push_back( &Zarc< T >::SecondRcVoltage );
    mFunctionVector.push_back( &Zarc< T >::ThirdRcVoltage );

    mOhmicFunctionVector.push_back( &Zarc< T >::AddOhmicResistanceToVoltage3R );
    mOhmicFunctionVector.push_back( &Zarc< T >::AddOhmicResistanceToVoltage2R );
    mOhmicFunctionVector.push_back( &Zarc< T >::AddOhmicResistanceToVoltage1R );
}

template < typename T >
void Zarc< T >::CalculateZarcElements( const double dtValue )
{
    const double const_tau = mLookupRelaxationTime->GetMinValueOfLookup();
    const double min_tauFactor = mLookupPhi2TauFactor( mLookupPhi->GetMinValueOfLookup() );
    if ( dtValue == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidSampleRate" );

    double minDtValue = dtValue * THREE_TAU;

    mTau.push_back( const_tau * min_tauFactor );
    mTau.push_back( const_tau );
    mTau.push_back( const_tau / min_tauFactor );

    for ( size_t i = 0; i < MAX_RC_ELEMENTS; ++i )
    {
        if ( mTau[i] > minDtValue )
            ++mNumberOfElements;
    }
    mRValues.resize( MAX_RC_ELEMENTS );
    mCValues.resize( mNumberOfElements );
    mVoltageValues.resize( mNumberOfElements );
}

template < typename T >
void Zarc< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{

    for ( size_t i = 0; i < mNumberOfElements; ++i )
        mUIDs.push_back( stateSystemGroup->mDglStateSystem.GetNewEquation() );

    TwoPort< T >::SetSystem( stateSystemGroup );
}

template < typename T >
void Zarc< T >::SetCurrent( const T current )
{
    TwoPort< T >::SetCurrent( current );
    SetVoltage();
    mVoltageEquation.zeros( mNumberOfElements, this->mCurrent.n_cols );
}


template < typename T >
void Zarc< T >::CalculateLookupValues()
{
    const double const_phi = mLookupPhi->GetValue();
    mCurrentRcElements.rFactorAussen = mLookupPhi2RFactorAussen( const_phi );
    mCurrentRcElements.rFactorInnen = mLookupPhi2RFactorInnen( const_phi );
    mCurrentRcElements.tauFactor = mLookupPhi2TauFactor( const_phi );

    mCurrentRcElements.const_r = mLookupOhmicResistance->GetValue();
    mCurrentRcElements.const_tau = mLookupRelaxationTime->GetValue();
}

template < typename T >
T* Zarc< T >::GetVoltage()
{
    CalculateLookupValues();
    for ( size_t i = 0; i < mNumberOfElements; ++i )
    {
        ( this->*mFunctionVector[i] )( mCurrentRcElements, mVoltageEquation );
        this->mStateSystemGroup->mDglStateSystem.AddEquations( mUIDs[i], mVoltageEquation );
    }

    // If mNumberOfElements < 3 that means we have to add the ohmic resistance of the RC element to the voltage
    if ( mNumberOfElements < 3 )
        ( this->*mOhmicFunctionVector[mNumberOfElements] )();

    return TwoPort< T >::GetVoltage();
}

template < typename T >
void Zarc< T >::AddOhmicResistanceToVoltage1R()
{
    mRValues[2] = mCurrentRcElements.rFactorAussen * mCurrentRcElements.const_r;
    this->mVoltage = mZarcVoltageValue + this->mCurrent * ( mCurrentRcElements.rFactorAussen * mCurrentRcElements.const_r );
}

template < typename T >
void Zarc< T >::AddOhmicResistanceToVoltage2R()
{
    mRValues[1] = ( mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r;
    this->mVoltage =
     mZarcVoltageValue +
     this->mCurrent * ( ( mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r );
}

template < typename T >
void Zarc< T >::AddOhmicResistanceToVoltage3R()
{
    mRValues[0] = ( 2 * mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r;
    this->mVoltage =
     mZarcVoltageValue +
     this->mCurrent * ( ( 2 * mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r );
}

// TAU = R*C
// C = TAU / R
template < typename T >
void Zarc< T >::FirstRcVoltage( const RcElements& rc, T& voltageEquation )
{
    mRValues[0] = rc.rFactorAussen * rc.const_r;
    mCValues[0] = rc.tauFactor * rc.const_tau / mRValues[0];
    voltageEquation = this->mCurrent * ( mRValues[0] / ( rc.tauFactor * rc.const_tau ) );
    voltageEquation( 0, mUIDs[0] ) -= 1.0 / ( rc.tauFactor * rc.const_tau );
}

template < typename T >
void Zarc< T >::SecondRcVoltage( const RcElements& rc, T& voltageEquation )
{
    mRValues[1] = rc.rFactorInnen * rc.const_r;
    mCValues[1] = rc.const_tau / mRValues[1];
    voltageEquation = this->mCurrent * ( ( rc.rFactorInnen * rc.const_r ) / ( rc.const_tau ) );
    voltageEquation( 0, mUIDs[1] ) -= 1.0 / ( rc.const_tau );
}

template < typename T >
void Zarc< T >::ThirdRcVoltage( const RcElements& rc, T& voltageEquation )
{
    mRValues[2] = rc.rFactorAussen * rc.const_r;
    mCValues[2] = ( rc.const_tau / rc.tauFactor ) / mRValues[2];
    voltageEquation = this->mCurrent * ( ( rc.tauFactor * rc.rFactorAussen * rc.const_r ) / ( rc.const_tau ) );
    voltageEquation( 0, mUIDs[2] ) -= rc.tauFactor / ( rc.const_tau );
}

template < typename T >
void Zarc< T >::SetVoltage()
{
    mZarcVoltageValue.zeros( this->mCurrent.n_rows, this->mCurrent.n_cols );

    for ( size_t i = 0; i < mNumberOfElements; ++i )
        mZarcVoltageValue( 0, mUIDs[i] ) = 1.0;

    this->mVoltage = mZarcVoltageValue;
}

template < typename T >
const char* Zarc< T >::GetName() const
{
    return "Zarc";
}
template < typename T >
size_t Zarc< T >::GetNumberOfElements() const
{
    return mNumberOfElements;
}

template < typename T >
const std::vector< ScalarUnit >& Zarc< T >::GetRValues() const
{
    return mRValues;
}

template < typename T >
const std::vector< ScalarUnit >& Zarc< T >::GetCValues() const
{
    return mCValues;
}

template < typename T >
const std::vector< double >& Zarc< T >::GetTau() const
{
    return mTau;
}

template < typename T >
const std::vector< ScalarUnit >& Zarc< T >::GetVoltageValues() const
{
    for ( size_t i = 0; i < mUIDs.size(); ++i )
        mVoltageValues[i] = this->mStateSystemGroup->mStateVector( mUIDs[i], 0 );

    return mVoltageValues;
}

#ifdef _EIGEN_
template <>
Eigen::SparseMatrix< double, RowMajor >* Zarc< Eigen::SparseMatrix< double, RowMajor > >::GetVoltage();

template <>
void Zarc< Eigen::SparseMatrix< double, RowMajor > >::FirstRcVoltage( const RcElements& rc,
                                                                      Eigen::SparseMatrix< double, RowMajor >& voltageEquation );
template <>
void Zarc< Eigen::SparseMatrix< double, RowMajor > >::SecondRcVoltage( const RcElements& rc,
                                                                       Eigen::SparseMatrix< double, RowMajor >& voltageEquation );
template <>
void Zarc< Eigen::SparseMatrix< double, RowMajor > >::ThirdRcVoltage( const RcElements& rc,
                                                                      Eigen::SparseMatrix< double, RowMajor >& voltageEquation );
#endif /* _EIGEN_ */

#ifdef _SYMBOLIC_
template <>
Zarc< symbolic::Symbolic >::CalculateLookupValues();
#endif
} /* END NAMESPACE */
#endif /* _ZARC_ */
