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
* File Name : surface_soc.cpp
* Creation Date : 30-06-2014
* Last Modified : Mo 07 Jul 2014 11:53:31 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

//STD
#include <iostream>

//ETC
#include "surface_soc.h"
#include "../electrical/twoport.h"

namespace electrical{ namespace state
{

SurfaceSoc::SurfaceSoc()
:
    mCurrentValue(0) {}

void SurfaceSoc::SetElementsForLookup(std::vector< electrical::TwoPort<myMatrixType> *> vec)
{
    mVecElements = vec;
}

void SurfaceSoc::UpdateLookUp()
{
    double ocvValue(0);
#ifndef _SYMBOLIC_
    for( size_t i = 0 ; i < mVecElements.size() ; ++i )
        ocvValue += mVecElements[i]->GetVoltageValue();
#endif

    mCurrentValue = mReverseLookup->GetValue(ocvValue);
}

void SurfaceSoc::SetReverseLookup(boost::shared_ptr<object::Object<double> > reverseLookup)
{
    mReverseLookup = reverseLookup;
}

double SurfaceSoc::GetValue() const
{
    return mCurrentValue;
}
} }
