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
* File Name : observerfactorybuilder.cpp
* Creation Date : 10-08-2015
* Last Modified : Fr 09 Okt 2015 16:41:47 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "observerfactorybuilder.h"

namespace factory
{


template <>
void AddExternalFilterThermal< double, false >(
 Factory< observer::Filter< double, thermal::ThermalElement, observer::ThermalPreperation >, ArgumentTypeObserver >* fct )
{
    UNUSED( fct );
};

template <>
void AddExternalFilterTwoPort< myMatrixType, false >(
 Factory< observer::Filter< myMatrixType, electrical::TwoPort, observer::PreparationType< myMatrixType > >, ArgumentTypeObserver >* fct )
{
    UNUSED( fct );
};


} /* namespace factory */
