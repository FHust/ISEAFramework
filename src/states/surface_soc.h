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
* File Name : surface_soc.h
* Creation Date : 30-06-2014
* Last Modified : Mi 02 Jul 2014 11:41:57 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SURFACE_SOC_
#define _SURFACE_SOC_

// STD
#include <vector>
#include <limits>
#include <boost/shared_ptr.hpp>

// ETC
#include "dgl_state.h"

#include "../object/object.h"
#include "../misc/matrixInclude.h"

namespace electrical
{
template < typename T >
class TwoPort;
// namespace state { class Soc; }
};


/// DESCRIPTION
namespace electrical
{
namespace state
{
/// This class describes the behaviour of the State of Charge depending on the capacity
class SurfaceSoc : public ::state::Dgl_state    //:  public Soc
{
    public:
    SurfaceSoc();
    /// This function has to be called manually. After the call mCurrentValue will be updated with the result of the
    /// reverse lookup.
    /// This can't be done automatically/with state as this would lead to inconsistent states for different twoports,
    /// depending on their creation order
    void UpdateLookUp();

    void SetReverseLookup( boost::shared_ptr< object::Object< double > > reverseLookup );    //< A setter method for the
                                                                                             //reverse lookup object
    void SetElementsForLookup( std::vector< electrical::TwoPort< myMatrixType > * > vec );    //< This setter provides a
                                                                                              //list of all twoPorts
                                                                                              //that are used for
                                                                                              //calculating the outer
                                                                                              //shells overpotential

    virtual double GetMaxDt() const { return ::std::numeric_limits< double >::max(); }
    virtual ~SurfaceSoc(){};

    double GetValue() const;

    private:
    boost::shared_ptr< object::Object< double > > mReverseLookup;
    std::vector< electrical::TwoPort< myMatrixType > * > mVecElements;
    double mCurrentValue;
};
} /* state */
}
#endif /* _SURFACE_SOC_ */
