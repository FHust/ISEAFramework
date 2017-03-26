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
 * cannotRegisterEquationAfterInitializationException.h
 *
 *  Created on: 27.11.2013
 *      Author: chris
 */

#ifndef CANNOTREGISTEREQUATIONAFTERINITIALIZATIONEXCEPTION_H_
#define CANNOTREGISTEREQUATIONAFTERINITIALIZATIONEXCEPTION_H_

#include <exception>

namespace systm
{

/// Exception for registering a new equation after the initialization of a StateSystem
class CannotRegisterEquationAfterInitializationException : public std::exception
{
    public:
    CannotRegisterEquationAfterInitializationException() {}

    virtual ~CannotRegisterEquationAfterInitializationException() throw() {}

    virtual const char* what() const throw()
    {
        return "Cannot register a new equation after the system is initialized";
    }
};

} /* namespace systm */

#endif /* CANNOTREGISTEREQUATIONAFTERINITIALIZATIONEXCEPTION_H_ */
