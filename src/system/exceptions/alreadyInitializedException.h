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
 * alreadyInitializedException.h
 *
 *  Created on: 27.11.2013
 *      Author: chris
 */

#ifndef ALREADYINITIALIZEDEXCEPTION_H_
#define ALREADYINITIALIZEDEXCEPTION_H_

#include <exception>

namespace systm
{

/// Exception for a second, not allowed, initialization of a StateSystem or StateSystemGroup
class AlreadyInitializedException : public std::exception
{
    public:
    AlreadyInitializedException() {}

    virtual ~AlreadyInitializedException() throw() {}

    virtual const char* what() const throw() { return "The StateSystemGroup has already been initialized"; }
};

} /* namespace systm */

#endif /* ALREADYINITIALIZEDEXCEPTION_H_ */
