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
* File Name : vcpfilter.h
* Creation Date : 30-08-2013
* Last Modified : Mo 06 Jun 2016 10:45:56 CEST
* Created By : Christopher Leroi
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _VCPFilter_
#define _VCPFilter_

// BOOST
#include <boost/shared_ptr.hpp>

#include "../../electrical/cellelement.h"
#include "filter.h"

typedef double real_T;

namespace observer
{
/// DESCRIPTION
template < typename T = myMatrixType >
class VCPFilter : public Filter< T, electrical::TwoPort, PreparationType< T > >
{
    public:
    typedef Filter< T, electrical::TwoPort, PreparationType< T > > FilterT;
    VCPFilter( real_T *voltageArray = 0, real_T *currentArray = 0, real_T *powerArray = 0, real_T *socArray = 0,
               real_T *surfaceSoc = 0 );
    virtual ~VCPFilter(){};

    void SetArrays( real_T *voltageArray, real_T *currentArray, real_T *powerArray, real_T *socArray, real_T *surfaceSoc );

    virtual void ProcessData( const typename FilterT::Data_t &data,
                              const double t )    // <--- needs to be inplace otherwise VS will barf
    {
        for ( size_t i = 0; i < data.size(); ++i )
        {
            electrical::TwoPort< T > *port = data[i];

            mVoltageArray[i] = port->GetVoltageValue();
            mCurrentArray[i] = port->GetCurrentValue();
            mPowerArray[i] = port->GetPowerValue();
            if ( port->IsCellelement() )
            {
                mSocArray[i] = static_cast< electrical::Cellelement< T > * >( port )->GetSocStateValue();
                if ( static_cast< electrical::Cellelement< T > * >( port )->mSurfaceSocSet )
                    mSurfaceSocArray[i] = static_cast< electrical::Cellelement< T > * >( port )->GetSurfaceSocStateValue();
                else
                    mSurfaceSocArray[i] = 0;
            }
            else
            {
                mSocArray[i] = 0;
                mSurfaceSocArray[i] = 0;
            }
        }

        FilterT::ProcessData( data, t );
    };

    private:
    real_T *mVoltageArray;
    real_T *mCurrentArray;
    real_T *mPowerArray;
    real_T *mSocArray;
    real_T *mSurfaceSocArray;

    protected:
};

template < typename T >
VCPFilter< T >::VCPFilter( real_T *voltageArray, real_T *currentArray, real_T *powerArray, real_T *socArray, real_T *surfaceSoc )
    : mVoltageArray( voltageArray )
    , mCurrentArray( currentArray )
    , mPowerArray( powerArray )
    , mSocArray( socArray )
    , mSurfaceSocArray( surfaceSoc )
{
}

template < typename T >
void VCPFilter< T >::SetArrays( real_T *voltageArray, real_T *currentArray, real_T *powerArray, real_T *socArray, real_T *surfaceSoc )
{
    mVoltageArray = voltageArray;
    mCurrentArray = currentArray;
    mPowerArray = powerArray;
    mSocArray = socArray;
    mSurfaceSocArray = surfaceSoc;
}

} /* END NAMESPACE */

#endif /* _VCPFilter_ */
