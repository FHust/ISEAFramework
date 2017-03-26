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
 * zarcalg.cpp
 *
 *  Created on: 04.12.2013
 *      Author: chris
 */

#include "zarcalg.h"
#include "../misc/matrixInclude.h"

namespace electrical
{

template<typename T>
const double ZarcAlg<T>::mPhi[] = {0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.92, 0.94, 0.96, 1};

template<typename T>
const double ZarcAlg<T>::mTauFactor[] = { 39.9742, 21.8364, 13.6186, 9.23106, 6.58365, 4.83194, 4.5502, 4.28597, 4.03746, 3.80306};

template<typename T>
const double ZarcAlg<T>::mRFactorAussen[] = {0.28084, 0.26535, 0.2446, 0.21576, 0.17431, 0.11057, 0.093487, 0.074352, 0.05276, 0.028198};

template<typename T>
const double ZarcAlg<T>::mRFactorInnen[] = {0.39633, 0.44284, 0.49461, 0.55903, 0.64641, 0.77687, 0.8115, 0.8502, 0.89378, 0.94327};

template class electrical::ZarcAlg< myMatrixType >;

} /* namespace electrical */
