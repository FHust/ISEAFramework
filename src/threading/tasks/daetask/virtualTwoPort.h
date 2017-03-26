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
 * VirtualTwoPort.h
 *
 *  Created on: 22.10.2013
 *      Author: fhu-cle
 */

#ifndef VirtualTwoPortMASTER_H_
#define VirtualTwoPortMASTER_H_

#if defined( BOOST_MPI ) || defined( BOOST_THREAD )

#include "../../../electrical/twoport.h"
#include "../../../electrical/twoport_withchild.h"
#include "../../communicators/communicatorInclude.h"
#include <vector>

namespace threading
{
namespace daetask
{

/// Data object
struct RowCopyInfo
{
    int FirstRowToCopy;
    int LastRowToCopy;
};

/// Data object
struct SystemCopyInfo
{
    RowCopyInfo DglRows;
    RowCopyInfo AlgRows;
};

/// Special electrical::TwoPort for parallelization
template < class CommunicatorType, typename MatrixType = arma::Mat< double > >
class VirtualTwoPort : public electrical::TwoPortWithChild< MatrixType >
{
};

#ifdef BOOST_MPI

/// Special electrical::TwoPort for parallelization
template < typename MatrixType >
class VirtualTwoPort< MPICommunicator, MatrixType > : public electrical::TwoPortWithChild< MatrixType >
{
    public:
    /// Creates a new VirtualTwoPort and puts it on top of another TwoPort.
    VirtualTwoPort( boost::shared_ptr< electrical::TwoPort< MatrixType > > replacedTwoPort );

    virtual ~VirtualTwoPort() {}

    /// Calculates with the internally stored values.
    void CalculateStateDependentValues();

    /// Forwards the call to the underlying TwoPort and calculates the initial voltage by calling GetVoltage()
    void SetCurrent( const MatrixType current );

    /// Saves the new current internally for later use. Called by main process.
    void SetCurrent( const double currentval );    ///< Sets the current value

    /// Forwards the call to the underlying TwoPort and gather information about the used matrix entries.
    void SetSystem( systm::StateSystemGroup< MatrixType >* stateSystemGroup );

    /// Returns information about the used matrix entries
    const SystemCopyInfo& GetSystemCopyInfo();

    /// Stores the voltage of the subnetwork internally for later use.
    void SetVoltage( const MatrixType& voltage );

    /// Returns the saved current value.
    const double GetCurrent() const;    ///< Gets the current value

    /// Sets the current of the subnetwork. Called in slave processes.
    void SetSubNetworkCurrent( double current );

    /// Returns the voltage of the subnetwork
    MatrixType* GetSubNetworkVoltage();

    /// Returns a list of all observable TwoPorts in the subnetwork
    std::vector< boost::shared_ptr< electrical::TwoPort< MatrixType > > >& GetObservableTwoPorts();

    /// Calculate the StateDependentValues of the subnetwork
    void CalculateStateDependentValuesOfSubNetwork();

    private:
    /// Default constructor disabled
    VirtualTwoPort() {}

    /// Searches for observable TwoPorts in the subnetwork and saves them in a vector
    void RegisterObservableTwoPort( boost::shared_ptr< electrical::TwoPort< MatrixType > > tp );

    double mCurrent;
    SystemCopyInfo mSystemCopyInfo;
    std::vector< boost::shared_ptr< electrical::TwoPort< MatrixType > > > mObservableTwoports;
};

template < typename MatrixType >
VirtualTwoPort< MPICommunicator, MatrixType >::VirtualTwoPort( boost::shared_ptr< electrical::TwoPort< MatrixType > > replacedTwoPort )
    : electrical::TwoPortWithChild< MatrixType >()
    , mCurrent( 0 )
{
    RegisterObservableTwoPort( replacedTwoPort );
    electrical::TwoPortWithChild< MatrixType >::AddChild( replacedTwoPort );
}

template < typename MatrixType >
void VirtualTwoPort< MPICommunicator, MatrixType >::CalculateStateDependentValues()
{
    electrical::TwoPort< MatrixType >::CalculateStateDependentValues();
}

template < typename MatrixType >
void VirtualTwoPort< MPICommunicator, MatrixType >::SetCurrent( const MatrixType current )
{
    this->mChildren[0]->SetCurrent( current );
    this->mVoltage = *this->mChildren[0]->GetVoltage();
}

template < typename MatrixType >
void VirtualTwoPort< MPICommunicator, MatrixType >::SetCurrent( const double currentval )    ///< Sets the current value
{
    mCurrent = currentval;
}

template < typename MatrixType >
void VirtualTwoPort< MPICommunicator, MatrixType >::SetSystem( systm::StateSystemGroup< MatrixType >* stateSystemGroup )
{
    mSystemCopyInfo.DglRows.FirstRowToCopy = stateSystemGroup->mDglStateSystem.GetEquationCount();
    mSystemCopyInfo.AlgRows.FirstRowToCopy = stateSystemGroup->mAlgStateSystem.GetEquationCount();

    this->mChildren[0]->SetSystem( stateSystemGroup );

    mSystemCopyInfo.DglRows.LastRowToCopy = stateSystemGroup->mDglStateSystem.GetEquationCount() - 1;
    mSystemCopyInfo.AlgRows.LastRowToCopy = stateSystemGroup->mAlgStateSystem.GetEquationCount() - 1;
}

template < typename MatrixType >
const SystemCopyInfo& VirtualTwoPort< MPICommunicator, MatrixType >::GetSystemCopyInfo()
{
    return mSystemCopyInfo;
}

template < typename MatrixType >
void VirtualTwoPort< MPICommunicator, MatrixType >::SetVoltage( const MatrixType& voltage )
{
    this->mVoltage = voltage;
}

template < typename MatrixType >
const double VirtualTwoPort< MPICommunicator, MatrixType >::GetCurrent() const    ///< Gets the current value
{
    return mCurrent;
}

template < typename MatrixType >
void VirtualTwoPort< MPICommunicator, MatrixType >::SetSubNetworkCurrent( double current )
{
    this->mChildren[0]->SetCurrent( current );
}

template < typename MatrixType >
MatrixType* VirtualTwoPort< MPICommunicator, MatrixType >::GetSubNetworkVoltage()
{
    return this->mChildren[0]->GetVoltage();
}

template < typename MatrixType >
std::vector< boost::shared_ptr< electrical::TwoPort< MatrixType > > >&
VirtualTwoPort< MPICommunicator, MatrixType >::GetObservableTwoPorts()
{
    return mObservableTwoports;
}

template < typename MatrixType >
void VirtualTwoPort< MPICommunicator, MatrixType >::CalculateStateDependentValuesOfSubNetwork()
{
    electrical::TwoPortWithChild< MatrixType >::CalculateStateDependentValues();
}

template < typename MatrixType >
void VirtualTwoPort< MPICommunicator, MatrixType >::RegisterObservableTwoPort( boost::shared_ptr< electrical::TwoPort< MatrixType > > tp )
{
    if ( tp->IsObservable() )
        mObservableTwoports.push_back( tp );

    if ( !tp->HasChildren() )
        return;

    boost::shared_ptr< electrical::TwoPortWithChild< MatrixType > > tpwc(
     boost::static_pointer_cast< electrical::TwoPortWithChild< MatrixType > >( tp ) );

    for ( size_t i = 0; i < tpwc->size(); ++i )
        RegisterObservableTwoPort( tpwc->shared_at( i ) );
}

#endif    // BOOST_MPI

#ifdef BOOST_THREAD

/// Special electrical::TwoPort for parallelization
template < typename MatrixType >
class VirtualTwoPort< ThreadCommunicator, MatrixType > : public electrical::TwoPortWithChild< MatrixType >
{
    public:
    /// Creates a new VirtualTwoPort and puts it on top of another TwoPort.
    VirtualTwoPort( boost::shared_ptr< electrical::TwoPort< MatrixType > > replacedTwoPort );

    virtual ~VirtualTwoPort() {}

    /// Calculates with the internally stored values.
    void CalculateStateDependentValues();

    /// Forwards the call to the underlying TwoPort and calculates the initial voltage by calling GetVoltage()
    void SetCurrent( const MatrixType current );

    /// Forwards the call to the underlying TwoPort
    void SetCurrent( const double currentval );    ///< Sets the current value

    /// Forwards the call to the underlying TwoPort and gather information about the used matrix entries.
    void SetSystem( systm::StateSystemGroup< MatrixType >* stateSystemGroup );

    /// Calculates the voltage of the subnetwork
    void CalcSubNetworkVoltage();

    /// Calculate the StateDependentValues of the subnetwork
    void CalculateStateDependentValuesOfSubNetwork();

    /// Returns information about the used matrix entries
    const SystemCopyInfo& GetSystemCopyInfo();

    /// Sets the current of the subnetwork. Called in slave processes.
    void SetSubNetworkCurrent();

    private:
    /// Default constructor disabled
    VirtualTwoPort() {}

    SystemCopyInfo mSystemCopyInfo;
    double mCurrent;
};

template < typename MatrixType >
VirtualTwoPort< ThreadCommunicator, MatrixType >::VirtualTwoPort( boost::shared_ptr< electrical::TwoPort< MatrixType > > replacedTwoPort )
    : electrical::TwoPortWithChild< MatrixType >()
    , mCurrent( 0 )
{
    electrical::TwoPortWithChild< MatrixType >::AddChild( replacedTwoPort );
}

template < typename MatrixType >
void VirtualTwoPort< ThreadCommunicator, MatrixType >::CalculateStateDependentValues()
{
    electrical::TwoPort< MatrixType >::CalculateStateDependentValues();
}

template < typename MatrixType >
void VirtualTwoPort< ThreadCommunicator, MatrixType >::SetCurrent( const MatrixType current )
{
    this->mChildren[0]->SetCurrent( current );
    this->mVoltage = *this->mChildren[0]->GetVoltage();
}

template < typename MatrixType >
void VirtualTwoPort< ThreadCommunicator, MatrixType >::SetCurrent( const double currentval )    ///< Sets the current value
{
    mCurrent = currentval;
}

template < typename MatrixType >
void VirtualTwoPort< ThreadCommunicator, MatrixType >::SetSystem( systm::StateSystemGroup< MatrixType >* stateSystemGroup )
{
    mSystemCopyInfo.DglRows.FirstRowToCopy = stateSystemGroup->mDglStateSystem.GetEquationCount();
    mSystemCopyInfo.AlgRows.FirstRowToCopy = stateSystemGroup->mAlgStateSystem.GetEquationCount();

    this->mChildren[0]->SetSystem( stateSystemGroup );

    mSystemCopyInfo.DglRows.LastRowToCopy = stateSystemGroup->mDglStateSystem.GetEquationCount() - 1;
    mSystemCopyInfo.AlgRows.LastRowToCopy = stateSystemGroup->mAlgStateSystem.GetEquationCount() - 1;
}

template < typename MatrixType >
void VirtualTwoPort< ThreadCommunicator, MatrixType >::CalcSubNetworkVoltage()
{
    this->mVoltage = *this->mChildren[0]->GetVoltage();
}

template < typename MatrixType >
void VirtualTwoPort< ThreadCommunicator, MatrixType >::CalculateStateDependentValuesOfSubNetwork()
{
    electrical::TwoPortWithChild< MatrixType >::CalculateStateDependentValues();
}

template < typename MatrixType >
const SystemCopyInfo& VirtualTwoPort< ThreadCommunicator, MatrixType >::GetSystemCopyInfo()
{
    return mSystemCopyInfo;
}

template < typename MatrixType >
void VirtualTwoPort< ThreadCommunicator, MatrixType >::SetSubNetworkCurrent()
{
    this->mChildren[0]->SetCurrent( mCurrent );
}

#endif    // BOOST_THREAD
}
} /* namespace threading */

#endif /* defined(BOOST_MPI) || defined(BOOST_THREAD) */
#endif /* VirtualTwoPortMASTER_H_ */
