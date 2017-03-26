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
* File Name : generalizedSystem.h
* Creation Date : 03-09-2014
* Created By : Fabian Frie
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef _GENERALIZEDSYSTEM_
#define _GENERALIZEDSYSTEM_
#include "../misc/matrixInclude.h"
#include <vector>

namespace systm
{

template < typename MatrixT >
class GeneralizedSystem
{
    public:
    GeneralizedSystem();
    virtual ~GeneralizedSystem(){};

    virtual void PrepareEquationSystem(){};
    virtual void operator()( const MatrixT& /* x */, MatrixT& /* dxdt */,
                             const double /* t */ ){};    // kann vermutlich nicht vererbt werden, muss also jede Klasse
                                                          // selber implementieren!
    virtual void operator()( const std::vector< double >& /* x */, std::vector< double >& /* dxdt */, const double /* t */ ){};    // Um boost::odeint-solver mit vector<double> benutzen zu können
    virtual const char* GetName() const { return "GeneralizedSystem"; };
    virtual void SolveForStateVector( MatrixT& /* x */ ){};    // NICHT SCHOEN DA NUR IN LINALGSYSTEM GEBRAUCHT!!
    //    private:
};

template < typename MatrixT >
GeneralizedSystem< MatrixT >::GeneralizedSystem()
{
}
}

#endif
