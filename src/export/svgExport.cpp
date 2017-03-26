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
* File Name : svgExport.cpp
* Creation Date : 24-03-2015
* Last Modified : Do 22 Dez 2016 14:18:02 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "svgExport.h"
#ifndef __NO_STRING__

template class SvgExport< myMatrixType, CELLLEVEL_DEPTH >;
template class SvgExport< myMatrixType, ELECTRICALELEMENTS_DEPTH >;
template class SvgExport< myMatrixType, ALL >;

#endif /* __NO_STRING__ */
