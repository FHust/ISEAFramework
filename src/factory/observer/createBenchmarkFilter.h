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
* File Name : createBenchmarkFilter.h
* Creation Date : 08-10-2015
* Last Modified : Do 08 Okt 2015 14:41:34 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CREATEBENMARKFILTER_
#define _CREATEBENMARKFILTER_

#include "../../observer/observer.h"
#include "../../observer/filter/benchmarkFilter.h"
#include "../../observer/filter/elementCounterFilter.h"

/// This function adds the benchmarkfilter and the element counterfilter to an observer
template < typename MatrixT, template < typename > class TConcrete >
void AddBenchmarkFilter( observer::Observer< MatrixT, TConcrete > *observer_ptr )
{
    observer_ptr->AddFilter( new observer::BenchmarkFilterTwoPort< MatrixT >() );
    observer_ptr->AddFilter( new observer::ElementCounterFilterTwoPort< MatrixT >() );
}
#endif /* _CREATEBENMARKFILTER_ */
