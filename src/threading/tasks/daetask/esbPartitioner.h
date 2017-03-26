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
 * esbpartitioner.h
 *
 *  Created on: 06.11.2013
 *      Author: chris
 */

#ifndef ESBPARTITIONER_H_
#define ESBPARTITIONER_H_

#if defined( BOOST_MPI ) || defined( BOOST_THREAD )

#include <vector>
#include <boost/shared_ptr.hpp>
#include "../../../electrical/twoport.h"
#include "../../../electrical/twoport_withchild.h"
#include "../../../electrical/paralleltwoport.h"
#include "../../../electrical/serialtwoport.h"
#include "virtualTwoPort.h"

using namespace electrical;

namespace threading
{
namespace daetask
{

/// This class is used for splitting a circuit into subcircuits. The subcircuits are joint with the use of the
/// VirtualTwoPort class.
template < class CommunicatorType, class MatrixType >
class ESBPartitioner
{

    public:
    /// Constructor. Pass the original RootTwoPort and the number of needed subcircuits. The network will be splitted
    /// immediately.
    ESBPartitioner( boost::shared_ptr< TwoPort< MatrixType > > rootPortToBeReplaced, size_t splitCount );

    virtual ~ESBPartitioner() {}

    /// Returns the new networks RootTwoPort
    boost::shared_ptr< TwoPort< MatrixType > > GetRoot();

    /// Returns the inserted VirtualTwoPorts
    const std::vector< boost::shared_ptr< VirtualTwoPort< CommunicatorType, MatrixType > > >& GetVirtualTwoPortVector();

    /// Return a secific VirtualTwoPort
    VirtualTwoPort< CommunicatorType, MatrixType >* GetVirtualTwoPort( size_t id );

    private:
    /// Default constructor disabled
    ESBPartitioner() {}

    /// This method can be used on any TwoPort type. It may forward the network to PartitionTwoPortWithChild.
    boost::shared_ptr< TwoPort< MatrixType > >
    PartitionESB( boost::shared_ptr< TwoPort< MatrixType > > twoPortToBeReplaced, size_t& splitCount );

    /// Splits TwoPortWithChild into subnetworks.
    boost::shared_ptr< TwoPort< MatrixType > >
    PartitionTwoPortWithChild( boost::shared_ptr< TwoPortWithChild< MatrixType > > twoPortToBeReplaced, size_t& splitCount );

    /// Creates a ParallelTwoPort or a SerialTwoPort, depending on the type of the argument passed.
    boost::shared_ptr< TwoPort< MatrixType > >
    CreateTwoPortWithChildFromSameClass( boost::shared_ptr< TwoPortWithChild< MatrixType > > twoPortToBeReplaced );

    /// Creates a new VirtualTwoPort, connects it to a subnetwork, stores it into a vector and returns it afterwards.
    boost::shared_ptr< VirtualTwoPort< CommunicatorType, MatrixType > >
    CreateAndRegisterVirtualTwoPort( boost::shared_ptr< electrical::TwoPort< MatrixType > > relatedTwoPort );

    std::vector< boost::shared_ptr< VirtualTwoPort< CommunicatorType, MatrixType > > > mVirtualTwoPortVec;
    boost::shared_ptr< TwoPort< MatrixType > > mNewRootPort;
};

template < class CommunicatorType, class MatrixType >
ESBPartitioner< CommunicatorType, MatrixType >::ESBPartitioner( boost::shared_ptr< TwoPort< MatrixType > > rootPortToBeReplaced,
                                                                size_t splitCount )
{
    if ( splitCount == 1 )    // Die VirtualTwoPorts unterstuetzen nicht die nötigen Initialisierungsmethoden. Deshalb
                              // wird ein SerialTwoPort davorgeschaltet.
    {
        boost::shared_ptr< SerialTwoPort< MatrixType > > newRootPort( new SerialTwoPort< MatrixType >() );
        newRootPort->AddChild( CreateAndRegisterVirtualTwoPort( rootPortToBeReplaced ) );
        mNewRootPort = newRootPort;
    }
    else
    {
        mNewRootPort = PartitionESB( rootPortToBeReplaced, splitCount );
    }
}

template < class CommunicatorType, class MatrixType >
boost::shared_ptr< TwoPort< MatrixType > > ESBPartitioner< CommunicatorType, MatrixType >::GetRoot()
{
    return mNewRootPort;
}

template < class CommunicatorType, class MatrixType >
const std::vector< boost::shared_ptr< VirtualTwoPort< CommunicatorType, MatrixType > > >&
ESBPartitioner< CommunicatorType, MatrixType >::GetVirtualTwoPortVector()
{
    return mVirtualTwoPortVec;
}

template < class CommunicatorType, class MatrixType >
VirtualTwoPort< CommunicatorType, MatrixType >* ESBPartitioner< CommunicatorType, MatrixType >::GetVirtualTwoPort( size_t id )
{
    return mVirtualTwoPortVec.at( id ).get();
}

template < class CommunicatorType, class MatrixType >
boost::shared_ptr< TwoPort< MatrixType > >
ESBPartitioner< CommunicatorType, MatrixType >::PartitionESB( boost::shared_ptr< TwoPort< MatrixType > > twoPortToBeReplaced,
                                                              size_t& splitCount )
{
    if ( splitCount == 1 )
        return CreateAndRegisterVirtualTwoPort( twoPortToBeReplaced );

    if ( !twoPortToBeReplaced->HasChildren() )    // Erklaerung: fuer den Fall, dass im String Serienwiderstaende sind,
                                                  // sollen diese nicht durch ThreadedPorts ersetzt werden und somit
                                                  // einen ganzen Worker verschwenden.
    {
        splitCount = 1;
        return twoPortToBeReplaced;
    }

    boost::shared_ptr< TwoPortWithChild< MatrixType > > twoportWithChild =
     boost::static_pointer_cast< TwoPortWithChild< MatrixType > >( twoPortToBeReplaced );
    return PartitionTwoPortWithChild( twoportWithChild, splitCount );
}

template < class CommunicatorType, class MatrixType >
boost::shared_ptr< TwoPort< MatrixType > >
ESBPartitioner< CommunicatorType, MatrixType >::CreateTwoPortWithChildFromSameClass( boost::shared_ptr< TwoPortWithChild< MatrixType > > twoPortToBeReplaced )
{
    if ( twoPortToBeReplaced->IsSerialTwoPort() )
        return boost::shared_ptr< TwoPort< MatrixType > >( new SerialTwoPort< MatrixType >( twoPortToBeReplaced->IsObservable() ) );
    else
        return boost::shared_ptr< TwoPort< MatrixType > >( new ParallelTwoPort< MatrixType >( twoPortToBeReplaced->IsObservable() ) );
}

template < class CommunicatorType, class MatrixType >
boost::shared_ptr< TwoPort< MatrixType > > ESBPartitioner< CommunicatorType, MatrixType >::PartitionTwoPortWithChild(
 boost::shared_ptr< TwoPortWithChild< MatrixType > > twoPortToBeReplaced, size_t& splitCount )
{
    boost::shared_ptr< TwoPortWithChild< MatrixType > > newTwoPortWithChild =
     boost::static_pointer_cast< TwoPortWithChild< MatrixType > >( CreateTwoPortWithChildFromSameClass( twoPortToBeReplaced ) );

    if ( splitCount < twoPortToBeReplaced->size() )
    {
        const size_t splitEach = twoPortToBeReplaced->size() / splitCount;
        const size_t splitRest = twoPortToBeReplaced->size() % splitCount;

        size_t childid = 0;

        for ( size_t splitnr = 0; splitnr < splitCount; ++splitnr )
        {
            boost::shared_ptr< TwoPortWithChild< MatrixType > > innerTwoPortWithChild =
             boost::static_pointer_cast< TwoPortWithChild< MatrixType > >( CreateTwoPortWithChildFromSameClass( twoPortToBeReplaced ) );

            for ( size_t i = 0; i < splitEach; ++i )
                innerTwoPortWithChild->AddChild( twoPortToBeReplaced->shared_at( childid++ ) );

            if ( splitnr < splitRest )
                innerTwoPortWithChild->AddChild( twoPortToBeReplaced->shared_at( childid++ ) );

            newTwoPortWithChild->AddChild( CreateAndRegisterVirtualTwoPort( innerTwoPortWithChild ) );
        }

        return newTwoPortWithChild;
    }

    size_t elementsLeftForSplit = splitCount;
    const size_t children = twoPortToBeReplaced->size();

    for ( size_t childid = 0; childid < children; ++childid )
    {
        const size_t splitEach =
         elementsLeftForSplit /
         ( children - childid );    // Immer neuberechnen, da mache children eventuell nicht so oft teilbar sind.
        const size_t splitRest = elementsLeftForSplit % ( children - childid );

        size_t splittedElements = splitEach;    // Per Referenz übergeben und verändert

        if ( childid < children - 1 && splitRest )
            splittedElements += 1;
        else
            splittedElements += splitRest;

        newTwoPortWithChild->AddChild( PartitionESB( twoPortToBeReplaced->shared_at( childid ), splittedElements ) );
        elementsLeftForSplit -= splittedElements;
    }
    splitCount -= elementsLeftForSplit;
    return newTwoPortWithChild;
}

template < class CommunicatorType, class MatrixType >
boost::shared_ptr< VirtualTwoPort< CommunicatorType, MatrixType > >
ESBPartitioner< CommunicatorType, MatrixType >::CreateAndRegisterVirtualTwoPort( boost::shared_ptr< electrical::TwoPort< MatrixType > > relatedTwoPort )
{
    boost::shared_ptr< VirtualTwoPort< CommunicatorType, MatrixType > > vTwoPort(
     new VirtualTwoPort< CommunicatorType, MatrixType >( relatedTwoPort ) );

    mVirtualTwoPortVec.push_back( vTwoPort );

    return vTwoPort;
}
}
} /* namespace threading */
#endif /* defined(BOOST_MPI) || defined(BOOST_THREAD) */

#endif /* ESBPARTITIONER_H_ */
