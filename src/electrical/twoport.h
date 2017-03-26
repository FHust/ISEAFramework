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
* File Name : twoport.h
* Creation Date : 30-10-2012
* Last Modified : Di 13 Okt 2015 17:31:21 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TWOPORT_
#define _TWOPORT_

#include "../system/stateSystemGroup.h"
#include "../exceptions/error_proto.h"

// STD
#include <vector>
#ifdef __EXCEPTIONS__
#include <stdexcept>
#include <string>
#endif /* __EXCEPTIONS__ */


// Friend
class TestDaeSystem;
class TestStateSystem;
class TestLinearSystem;

namespace electrical
{
#ifndef _SYMBOLIC_
typedef double ScalarUnit;
#else
typedef symbolic::Symbolic ScalarUnit;
#endif


/// Class is the basic interface for any TwoPort. In case of creating a new TwoPort this should be the least basic
/// class. The relation should rather be "HAS A" instead of "IS A"
template < typename T = myMatrixType >
class TwoPort
{
    friend class ::TestDaeSystem;
    friend class ::TestStateSystem;
    friend class ::TestLinearSystem;

    public:
    TwoPort( const bool observable = false );
    virtual ~TwoPort(){};

    virtual void SetCurrent( const T current );    ///< Sets the current
    const T GetCurrent() const;                    ///< Sets the current vector

    void UpdateStateSystemGroup();    ///<Calculates all voltage equations and saves them in the StateSystem
    void SetInitialCurrent( ScalarUnit current = 0 );          ///< Only called at the RootTwoPort. Creates the current vector and sends it into the network. It also makes the TwoPort observable.
    virtual void SetCurrent( const ScalarUnit currentval );    ///< Sets the current value

    virtual T* GetVoltage();    ///< Return a Voltage over the TwoPort including children

    const T* GetConstVoltage() const;    ///< Return a Voltage over the TwoPort including children

    virtual bool HasChildren() const;        ///< If this twoport has a child
    virtual bool CanHaveChildren() const;    ///< If this TwoPort is able to have children e.g. mChildren
    virtual bool IsCellelement() const;      ///< Is this element a cell element?

    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );    ///< This function sets the system, where the TwoPorts registeres itself and submits its equations Dgl or Alg

    bool HasSystem() const;    ///< Does this TwoPort has a system set?

    bool IsObservable() const;    ///< Is this TwoPort observable?

    size_t GetID() const;
    void SetID( size_t iD );

    ScalarUnit GetCurrentValue() const;    ///< Returns the precalculated current
    ScalarUnit GetVoltageValue() const;    ///< Returns the precalculated voltage
    ScalarUnit GetPowerValue() const;      ///< Returns the precalculated power

#ifndef _SYMBOLIC_

    virtual void LoadInternalData( std::vector< double >& dataVector );    ///< Loads the StateDependentValues from a vector. Some kind serialization. Used for parallelization.
    virtual void SaveInternalData( std::vector< double >& dataVector );    ///< Saves the StateDependentValues  to a vector.
#endif

    virtual void CalculateStateDependentValues();    ///< Calculates the StateDependentValues. Should be overwritten.
    virtual const char* GetName() const;

    private:
    protected:
    /// Calculate the current of the previous simulation step.
    void CalculateCurrentValue();

    /// Calculate the voltage of the previous simulation step.
    void CalculateVoltageValue();

    size_t mID;
    T mCurrent;
    T mVoltage;
    ScalarUnit mCurrentValue;
    ScalarUnit mVoltageValue;
    ScalarUnit mPowerValue;

    systm::StateSystemGroup< T >* mStateSystemGroup;

    bool mObservable;
};

template < typename T >
void TwoPort< T >::CalculateStateDependentValues()
{
    if ( this->mObservable )
    {
        this->CalculateCurrentValue();
        this->CalculateVoltageValue();
    }
}

#ifndef _SYMBOLIC_

template < typename T >
void TwoPort< T >::LoadInternalData( std::vector< double >& dataVector )
{
    mPowerValue = dataVector.back();
    dataVector.pop_back();

    mVoltageValue = dataVector.back();
    dataVector.pop_back();

    mCurrentValue = dataVector.back();
    dataVector.pop_back();
}

template < typename T >
void TwoPort< T >::SaveInternalData( std::vector< double >& dataVector )
{
    dataVector.push_back( mCurrentValue );
    dataVector.push_back( mVoltageValue );
    dataVector.push_back( mPowerValue );
}
#endif

template < typename T >
ScalarUnit TwoPort< T >::GetCurrentValue() const
{
    return mCurrentValue;
}

template < typename T >
ScalarUnit TwoPort< T >::GetVoltageValue() const
{
    return mVoltageValue;
}

template < typename T >
ScalarUnit TwoPort< T >::GetPowerValue() const
{
    return mPowerValue;
}


template < typename T >
inline double ReturnFirstElement( T& mat )
{
    return mat( 0, 0 );
}

#ifdef _EIGEN_
template <>
double ReturnFirstElement( Eigen::SparseMatrix< double, RowMajor >& mat );
#endif

template < typename T >
void TwoPort< T >::CalculateCurrentValue()
{
    T m = ( mCurrent * this->mStateSystemGroup->mStateVector );
    this->mCurrentValue = ReturnFirstElement( m );
}

template < typename T >
void TwoPort< T >::CalculateVoltageValue()
{
    T m = ( mVoltage * this->mStateSystemGroup->mStateVector );
    this->mVoltageValue = ReturnFirstElement( m );
}

template < typename T >
TwoPort< T >::TwoPort( const bool observable )
    : mID( 0 )
    , mCurrent( T() )
    , mVoltage( T() )
    , mCurrentValue( 0 )
    , mVoltageValue( 0 )
    , mPowerValue( 0 )
    , mStateSystemGroup( 0 )
    , mObservable( observable )
{
}

template < typename T >
void TwoPort< T >::UpdateStateSystemGroup()
{
    if ( mID )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "OnlyRootport" );

    this->GetVoltage();
}

template < typename T >
void TwoPort< T >::SetInitialCurrent( ScalarUnit current )
{
    if ( mID )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "OnlyRootport" );
    mObservable = true;    // Das RootElement soll VCP automatisch berechnen und bereitstellen

    T tmpcurrent;
    tmpcurrent.zeros( 1, 1 + mStateSystemGroup->GetStateCount() );
    tmpcurrent( 0, mStateSystemGroup->GetStateCount() ) = current;
    this->SetCurrent( tmpcurrent );
}

template < typename T >
bool TwoPort< T >::HasSystem() const
{
    return mStateSystemGroup != 0;    // double negation for bool conversion
}

template < typename T >
size_t TwoPort< T >::GetID() const
{
    return mID;
}

template < typename T >
void TwoPort< T >::SetID( size_t iD )
{
    mID = iD;
}

template < typename T >
T* TwoPort< T >::GetVoltage()
{
    return &mVoltage;
}

template < typename T >
const T* TwoPort< T >::GetConstVoltage() const
{
    return static_cast< const T* >( &mVoltage );
}

template < typename T >
bool TwoPort< T >::IsObservable() const
{
    return mObservable;
}

template < typename T >
void TwoPort< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{
    if ( HasSystem() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SystemAlreadyDefined" );
    mStateSystemGroup = stateSystemGroup;
}

template < typename T >
void TwoPort< T >::SetCurrent( const T current )
{
    mCurrent = current;
    mVoltage.zeros( mCurrent.n_rows, mCurrent.n_cols );
}

template < typename T >
const T TwoPort< T >::GetCurrent() const
{
    return mCurrent;
}

template < typename T >
void TwoPort< T >::SetCurrent( const ScalarUnit currentval )
{
    this->mCurrent( 0, this->mCurrent.n_cols - 1 ) = currentval;
}

template < typename T >
bool TwoPort< T >::IsCellelement() const
{
    return false;
}

template < typename T >
bool TwoPort< T >::CanHaveChildren() const
{
    return false;
}

template < typename T >
bool TwoPort< T >::HasChildren() const
{
    return false;
}

template < typename T >
const char* TwoPort< T >::GetName() const
{
    return "TwoPort";
}

} /* END NAMESPACE */

#endif /* _TWOPORT_ */
