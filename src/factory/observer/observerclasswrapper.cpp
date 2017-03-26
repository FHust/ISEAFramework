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
* File Name : observerclasswrapper.cpp
* Creation Date : 10-08-2015
* Last Modified : Mo 06 Jun 2016 13:46:07 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "observerclasswrapper.h"

#include "../../misc/matrixInclude.h"

namespace factory
{

template class ObserverClassWrapperTwoPort< myMatrixType, observer::DecimateFilterTwoPort >;
template class ObserverClassWrapperThermal< myMatrixType, observer::DecimateFilterThermal >;

template class ObserverClassWrapperTwoPort< myMatrixType, observer::StdoutFilterTwoPort >;
template class ObserverClassWrapperTwoPort< myMatrixType, observer::CsvFilterTwoPort >;
template class ObserverClassWrapperTwoPort< myMatrixType, observer::MatlabFilterTwoPort >;

template class ObserverClassWrapperThermal< double, observer::CsvFilterThermal >;
template class ObserverClassWrapperThermal< double, observer::MatlabFilterThermal >;
}
