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
 * nochildrenexception.h
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifndef NOCHILDRENEXCEPTION_H_
#define NOCHILDRENEXCEPTION_H_

#ifdef __EXCEPTIONS__
#include <string>

namespace factory
{

/// This exception is thrown if a electrical::TwoPortWithChild has not got any children added.
class NoChildrenException : public std::exception
{
    public:
    NoChildrenException( const char* className );

    virtual ~NoChildrenException() throw() {}

    virtual const char* what() const throw();

    protected:
    std::string mErrorMsg;
};

} /* namespace factory */
#endif /* __EXCEPTIONS__ */

#endif /* NOCHILDRENEXCEPTION_H_ */
