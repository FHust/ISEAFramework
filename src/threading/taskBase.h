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
 * TaskBase.h
 *
 *  Created on: 06.01.2014
 *      Author: chris
 */

#ifndef TaskBase_H_
#define TaskBase_H_

#include "communicators/communicatorInclude.h"
#include "taskData.h"
#include <boost/shared_ptr.hpp>
#include <vector>

namespace threading
{

/// Base class for the several template specialization of Task
template < class CommunicatorType = DummyCommunicator >
class TaskBase
{

    public:
    /// Constructor. The minimal and the maximal workernumber can be declared with both parameters
    TaskBase( const size_t minimalNumberOfWorker, const size_t maximalNumberOfWorker );

    /// Constructor. Minimal worker:1 Maximal worker:Inf
    TaskBase();

    /// Destructor
    virtual ~TaskBase() {}

    /// This method initializes the Tasks with its TaskID and the assigned number of workers. This will call the
    /// abstract Init method, which has to be implemented for a derived Task. This method is called by the ThreadManager
    void Init( size_t numberOfWorker );

    /// Returns the maximal allowed number of worker
    size_t GetMaximalNumberOfWorker() const;

    /// Sets the maximal allowed number of worker
    void SetMaximalNumberOfWorker( const size_t maxWorker );

    /// Returns the minimal allowed number of worker
    size_t GetMinimalNumberOfWorker() const;

    /// Sets the minimal allowed number of worker
    void SetMinimalNumberOfWorker( const size_t minWorker );

    /// Returns the number of worker of this task
    size_t GetNumberOfWorker() const;

    /// Returns relative numbers corresponding to the workers workload
    virtual std::vector< size_t > GetWorkloadIndices() const;

    protected:
    /// Abstract initialization method. It will be called after the workernumber is determined
    virtual void Init();

    size_t mNumberOfWorker;
    bool mIsInitialized;

    private:
    size_t mNumberOfUnreadyWorker;
    size_t mMinimalNumberOfWorker;
    size_t mMaximalNumberOfWorker;
    bool mIsReady;
};

/// Constructor. The minimal and the maximal workernumber can be declared with both parameters
template < class CommunicatorType >
TaskBase< CommunicatorType >::TaskBase( const size_t minimalNumberOfWorker, const size_t maximalNumberOfWorker )
    : mNumberOfWorker( 0 )
    , mIsInitialized( false )
    , mNumberOfUnreadyWorker( 0 )
    , mMinimalNumberOfWorker( minimalNumberOfWorker )
    , mMaximalNumberOfWorker( maximalNumberOfWorker )
    , mIsReady( false )

{
}

/// Constructor. Minimal worker:1 Maximal worker:Inf
template < class CommunicatorType >
TaskBase< CommunicatorType >::TaskBase()
    : mNumberOfWorker( 0 )
    , mIsInitialized( false )
    , mNumberOfUnreadyWorker( 0 )
    , mMinimalNumberOfWorker( 1 )
    , mMaximalNumberOfWorker( std::numeric_limits< size_t >::max() )
    , mIsReady( false )

{
}

/// This method initializes the Tasks with its TaskID and the assigned number of workers. This will call the abstract
/// Init method, which has to be implemented for a derived Task. This method is called by the ThreadManager
template < class CommunicatorType >
void TaskBase< CommunicatorType >::Init( size_t numberOfWorker )    /// Initializes this Tasks.
{
    mNumberOfWorker = numberOfWorker;
    Init();
    mIsInitialized = true;
}

/// Returns the maximal allowed number of worker
template < class CommunicatorType >
size_t TaskBase< CommunicatorType >::GetMaximalNumberOfWorker() const
{
    return mMaximalNumberOfWorker;
}

/// Sets the maximal allowed number of worker
template < class CommunicatorType >
void TaskBase< CommunicatorType >::SetMaximalNumberOfWorker( const size_t maxWorker )
{
    mMaximalNumberOfWorker = maxWorker;
}

/// Returns the minimal allowed number of worker
template < class CommunicatorType >
size_t TaskBase< CommunicatorType >::GetMinimalNumberOfWorker() const
{
    return mMinimalNumberOfWorker;
}

/// Sets the minimal allowed number of worker
template < class CommunicatorType >
void TaskBase< CommunicatorType >::SetMinimalNumberOfWorker( const size_t minWorker )
{
    mMinimalNumberOfWorker = minWorker;
}

/// Returns the number of worker of this task
template < class CommunicatorType >
size_t TaskBase< CommunicatorType >::GetNumberOfWorker() const
{
    return mNumberOfWorker;
}

template < class CommunicatorType >
std::vector< size_t > TaskBase< CommunicatorType >::GetWorkloadIndices() const
{
    return std::vector< size_t >( 1, mNumberOfWorker );
}

template < class CommunicatorType >
void TaskBase< CommunicatorType >::Init()
{
}

} /* namespace threading */

#endif /* TaskBase_H_ */
