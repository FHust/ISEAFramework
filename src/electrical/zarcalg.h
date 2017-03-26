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
/*
 * ZarcAlgalg.h
 *
 *  Created on: 04.12.2013
 *      Author: chris
 */
#ifndef ZARCALG_H_
#define ZARCALG_H_

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

class TestZarc;
class TestXML;
class TestFactoryLookUpCellElements;

namespace electrical
{

/// This TwoPort describes the behaviour of a ZarcAlg element (3 parallelRC)
template < typename T = myMatrixType >
class ZarcAlg : public TwoPort< T >
{
    friend class ::TestZarc;
    friend class ::TestXML;
    friend class ::TestFactoryLookUpCellElements;

    public:
    ZarcAlg( boost::shared_ptr< object::Object< double > > lookupRelaxationTime,
             boost::shared_ptr< object::Object< double > > lookupOhmicResistance,
             boost::shared_ptr< object::Object< double > > lookupPhi, const double samplingRate = 0.001,
             const bool observable = false );    ///< The ZarcAlg element needs a thermal and an electrical state to be created, and does not take care of these. These  are just beeing utelized for the lookups

    virtual ~ZarcAlg(){};

    virtual T* GetVoltage();    ///< Returns the voltage over the ZarcAlg element
    std::vector< double >& GetTau() const;
    virtual void CalculateStateDependentValues();

    virtual const char* GetName() const;

    private:
    struct RcElements
    {
        double rFactorAussen, rFactorInnen, tauFactor, const_r, const_tau;
    };
    RcElements mCurrentRcElements;

    void InitializeZarc( const double samplingRate );

    void CalculateZarcElements( const double samplingRate );

    boost::shared_ptr< object::Object< double > > mLookupOhmicResistance;
    boost::shared_ptr< object::Object< double > > mLookupRelaxationTime;
    boost::shared_ptr< object::Object< double > > mLookupPhi;

    const lookup::Lookup1D< double > mLookupPhi2TauFactor;
    const lookup::Lookup1D< double > mLookupPhi2RFactorInnen;
    const lookup::Lookup1D< double > mLookupPhi2RFactorAussen;

    std::vector< size_t > mUIDs;
    std::vector< double > mTau;

    static const double mPhi[];
    static const double mTauFactor[];
    static const double mRFactorAussen[];
    static const double mRFactorInnen[];

    size_t mNumberOfElements;
    static const size_t THREE_TAU = 3;
    static const size_t MAX_RC_ELEMENTS = 3;

    double mCapacitorVoltages[MAX_RC_ELEMENTS];

    protected:
};

template < typename T >
ZarcAlg< T >::ZarcAlg( boost::shared_ptr< object::Object< double > > lookupRelaxationTime,
                       boost::shared_ptr< object::Object< double > > lookupOhmicResistance,
                       boost::shared_ptr< object::Object< double > > lookupPhi, const double samplingRate, const bool observable )
    : TwoPort< T >( observable )
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
    InitializeZarc( samplingRate );
}

template < typename T >
void ZarcAlg< T >::CalculateStateDependentValues()
{
#ifndef _SYMBOLIC_
    this->CalculateCurrentValue();

    this->mPowerValue = ::std::pow( this->mCurrentValue, 2.0 ) *
                        ( ( 2 * mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r );
#endif
}

template < typename T >
void ZarcAlg< T >::InitializeZarc( const double samplingRate )
{
    CalculateZarcElements( samplingRate );
    mCapacitorVoltages[0] = 0;
    mCapacitorVoltages[1] = 0;
    mCapacitorVoltages[2] = 0;
}

template < typename T >
void ZarcAlg< T >::CalculateZarcElements( const double samplingRate )
{
    const double const_tau = mLookupRelaxationTime->GetMinValueOfLookup();
    const double min_tauFactor = mLookupPhi2TauFactor( mLookupPhi->GetMinValueOfLookup() );
    if ( samplingRate == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidSampleRate" );

    double minSamplingRate = samplingRate * THREE_TAU;


    mTau.push_back( const_tau * min_tauFactor );
    mTau.push_back( const_tau );
    mTau.push_back( const_tau / min_tauFactor );

    for ( size_t i = 0; i < MAX_RC_ELEMENTS; ++i )
    {
        if ( mTau[i] < minSamplingRate )
            ++mNumberOfElements;
    }
}

template < typename T >
T* ZarcAlg< T >::GetVoltage()
{
#ifndef _SYMBOLIC_
    const double const_phi = mLookupPhi->GetValue();
    mCurrentRcElements.rFactorAussen = mLookupPhi2RFactorAussen( const_phi );
    mCurrentRcElements.rFactorInnen = mLookupPhi2RFactorInnen( const_phi );
    mCurrentRcElements.tauFactor = mLookupPhi2TauFactor( const_phi );

    mCurrentRcElements.const_r = mLookupOhmicResistance->GetValue();
    mCurrentRcElements.const_tau = mLookupRelaxationTime->GetValue();

    double re[MAX_RC_ELEMENTS];
    double rc[MAX_RC_ELEMENTS];
    double c[MAX_RC_ELEMENTS];

    c[0] = ( ( mCurrentRcElements.rFactorAussen * mCurrentRcElements.const_r ) /
             ( mCurrentRcElements.tauFactor * mCurrentRcElements.const_tau ) );
    rc[0] = 1.0 / ( mCurrentRcElements.tauFactor * mCurrentRcElements.const_tau );
    re[0] = ( 2 * mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r;

    c[1] = ( ( mCurrentRcElements.rFactorInnen * mCurrentRcElements.const_r ) / ( mCurrentRcElements.const_tau ) );
    rc[1] = 1.0 / ( mCurrentRcElements.const_tau );
    re[1] = ( mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r;

    c[2] = ( ( mCurrentRcElements.tauFactor * mCurrentRcElements.rFactorAussen * mCurrentRcElements.const_r ) /
             ( mCurrentRcElements.const_tau ) );
    rc[2] = mCurrentRcElements.tauFactor / ( mCurrentRcElements.const_tau );
    re[2] = mCurrentRcElements.rFactorAussen * mCurrentRcElements.const_r;

    this->mVoltageValue = 0;

    const double dt = this->mStateSystemGroup->mDt;

    for ( size_t i = 0; i < mNumberOfElements; ++i )
    {
        double deltaU = this->mCurrentValue * c[i] - mCapacitorVoltages[i] * rc[i];
        mCapacitorVoltages[i] += deltaU * dt;

        // Einzelne Spannungen adddieren
        this->mVoltageValue += mCapacitorVoltages[i];
    }

    // If mNumberOfElements < 3 that means we have to add the ohmic resistance of the RC element to the voltage
    if ( mNumberOfElements < 3 )
        this->mVoltageValue += this->mCurrentValue * re[mNumberOfElements];

    // return the constant value
    this->mVoltage( 0, this->mCurrent.n_cols - 1 ) = this->mVoltageValue;
#endif
    return TwoPort< T >::GetVoltage();
}

template < typename T >
const char* ZarcAlg< T >::GetName() const
{
    return "ZarcAlg";
}

} /* namespace electrical */

#endif /* ZARCALG_H_ */
