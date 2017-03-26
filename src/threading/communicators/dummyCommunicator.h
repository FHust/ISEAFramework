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
 * dummyCommunicator.h
 *
 *  Created on: 18.11.2013
 *      Author: chris
 */

#ifndef DUMMYCOMMUNICATOR_H_
#define DUMMYCOMMUNICATOR_H_

#include "communicatorBase.h"

namespace threading
{

/// Dummy communicator which can be used for single thread execution
class DummyCommunicator
{
    public:
    DummyCommunicator() {}

    virtual ~DummyCommunicator() {}

    size_t GetSystemThreadCount() const { return 1; }

    void RunThreads() {}

    void SetReady() {}

    bool IsReady() const { return true; }

    void FinishThisCycle() {}

    bool IsCycleFinished() const { return false; }

    void StopAllProcesses() {}

    bool IsStop() const { return false; }

    size_t GetThreadID() const { return 0; }
};

} /* namespace threading */

#endif /* DUMMYCOMMUNICATOR_H_ */
