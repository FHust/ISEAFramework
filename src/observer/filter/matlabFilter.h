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
* File Name :
* Creation Date : 18-11-2012
* Last Modified : Do 22 Dez 2016 14:17:43 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MAT_FILENAME_
#define _MAT_FILENAME_

#ifndef __NO_STRING__
// BOOST
#include <boost/shared_ptr.hpp>

// STD
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>

#include "../observer.h"
#include "../../container/matio_file.h"
#include "../../container/matio_data.h"
#include "../../electrical/cellelement.h"
#include "../../thermal/blocks/elements/thermal_element.h"

namespace observer
{

template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType >
class MatlabFilter : public Filter< T, TConcrete, ArgumentType >
{
    public:
    typedef Filter< T, TConcrete, ArgumentType > FilterT;


    MatlabFilter( std::string filename )
        : FilterT()
        , mMatFile( filename, MAT_ACC_RDWR ){};


    virtual ~MatlabFilter()
    {
        if ( mTime.empty() )
            return;
        mMatFile << matlab::MatioData( mTime, "diga.daten.Programmdauer" );
    }

    virtual void ProcessData( const typename FilterT::Data_t &data, const double t )
    {
        UNUSED( data );
        mTime.push_back( t );
        FilterT::ProcessData( data, t );
    };

    protected:
    std::vector< double > mTime;
    matlab::MatFile mMatFile;
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
class MatlabFilterBase : public MatlabFilter< T, TConcrete, ArgumentType >
{
    public:
    MatlabFilterBase( std::string filename )
        : MatlabFilter< T, TConcrete, ArgumentType >( filename ){};
};


template < typename T >
class MatlabFilterBase< T, electrical::TwoPort, PreparationType > : public MatlabFilter< T, electrical::TwoPort, PreparationType >
{

    typedef Filter< T, electrical::TwoPort, PreparationType > FilterT;

    public:
    MatlabFilterBase( std::string filename )
        : MatlabFilter< T, electrical::TwoPort, PreparationType >( filename ){};

    virtual ~MatlabFilterBase()
    {
        if ( mCurrent.empty() || mVoltage.empty() || mPower.empty() || mSoc.empty() || mTemperature.empty() )
            return;
        this->mMatFile << matlab::MatioData( mCurrent, "diga.daten.Strom" );
        this->mMatFile << matlab::MatioData( mVoltage, "diga.daten.Spannung" );
        this->mMatFile << matlab::MatioData( mPower, "diga.daten.ThermischLeistung" );
        this->mMatFile << matlab::MatioData( mSoc, "diga.daten.SOC" );
        this->mMatFile << matlab::MatioData( mTemperature, "diga.daten.Temperatur" );
    }

    virtual void PrepareFilter( PreparationType &prePareData ) { InitializeVectors( prePareData.mNumberOfElements ); };

    virtual void ProcessData( const typename FilterT::Data_t &data, const double t )
    {

        for ( size_t i = 0; i < data.size(); ++i )
        {
            electrical::TwoPort< T > *port = data[i];
            mVoltage[i].push_back( port->GetVoltageValue() );
            mCurrent[i].push_back( port->GetCurrentValue() );
            mPower[i].push_back( port->GetPowerValue() );

            if ( port->IsCellelement() )
            {
                electrical::Cellelement< T > *cell = static_cast< electrical::Cellelement< T > * >( port );
                mSoc[i].push_back( cell->GetSocStateValue() );
                mTemperature[i].push_back( cell->GetThermalState()->GetValue() );
            }
            else
            {
                mSoc[i].push_back( -1.0 );
                mTemperature[i].push_back( -274 );
            }
        }
        MatlabFilter< T, electrical::TwoPort, PreparationType >::ProcessData( data, t );
    }

    private:
    void InitializeVectors( const size_t vectorSizes )
    {
        mCurrent.resize( vectorSizes );
        mVoltage.resize( vectorSizes );
        mSoc.resize( vectorSizes );
        mPower.resize( vectorSizes );
        mTemperature.resize( vectorSizes );
    };

    std::vector< std::vector< double > > mCurrent;
    std::vector< std::vector< double > > mVoltage;
    std::vector< std::vector< double > > mSoc;
    std::vector< std::vector< double > > mPower;
    std::vector< std::vector< double > > mTemperature;

    protected:
};

template < typename T >
class MatlabFilterBase< T, thermal::ThermalElement, ThermalPreperation >
 : public MatlabFilter< T, thermal::ThermalElement, ThermalPreperation >
{

    typedef Filter< T, thermal::ThermalElement, ThermalPreperation > FilterT;

    public:
    MatlabFilterBase( std::string filename )
        : MatlabFilter< T, thermal::ThermalElement, ThermalPreperation >( filename ){};

    virtual ~MatlabFilterBase()
    {
        if ( mTemperature.empty() )
            return;
        this->mMatFile << matlab::MatioData( mTemperature, "diga.daten.Temperature" );
    }

    virtual void PrepareFilter( ThermalPreperation &prepData )
    {
        InitializeVectors( prepData.mAreas.size() );

        std::ofstream fileVertices( mFileNameVertices );
        std::ofstream fileAreas( mFileNameAreas );
        std::ofstream fileVolumes( mFileNameVolumes );
        std::ofstream fileVolumeNames( mFileNameVolumeNames );

        BOOST_FOREACH ( const geometry::Cartesian< T > &vertex, prepData.mVertices )
            fileVertices << vertex.GetX() << ", " << vertex.GetY() << ", " << vertex.GetZ() << "\n";

        BOOST_FOREACH ( const std::vector< size_t > &area, prepData.mAreas )
        {
            std::vector< size_t >::const_iterator it = area.begin();
            if ( it != area.end() )
            {
                fileAreas << *it;
                for ( ++it; it != area.end(); ++it )
                {
                    fileAreas << ", " << *it;
                }
                fileAreas << "\n";
            }
        }
        BOOST_FOREACH ( const std::vector< size_t > &volume, prepData.mVolumes )
        {
            std::vector< size_t >::const_iterator it = volume.begin();
            if ( it != volume.end() )
            {
                fileVolumes << *it;
                for ( ++it; it != volume.end(); ++it )
                {
                    fileVolumes << ", " << *it;
                }
                fileVolumes << "\n";
            }
        }
        BOOST_FOREACH ( misc::StrCont &volumeName, prepData.mVolumeNames )
        {
            fileVolumeNames << volumeName << "\n";
        }
        fileVertices.close();
        fileAreas.close();
        fileVolumes.close();
        fileVolumeNames.close();
    }


    virtual void ProcessData( const typename FilterT::Data_t &data, const double t )
    {
        size_t element = 0;
        BOOST_FOREACH ( const thermal::ThermalElement< T > *thermalElement, data )
        {
            mTemperature[element].push_back( thermalElement->GetTemperature() );
            ++element;
        }

        MatlabFilter< T, thermal::ThermalElement, ThermalPreperation >::ProcessData( data, t );
    }

    void InitializeVectors( const size_t vectorSizes ) { mTemperature.resize( vectorSizes ); }

    protected:
    std::string mFileNameVertices, mFileNameAreas, mFileNameVolumes, mFileNameVolumeNames;

    private:
    std::vector< std::vector< double > > mTemperature;
};


template < typename T >
class MatlabFilterTwoPort : public MatlabFilterBase< T, electrical::TwoPort, PreparationType >
{
    public:
    MatlabFilterTwoPort( std::string filename )
        : MatlabFilterBase< T, electrical::TwoPort, PreparationType >( filename ){};
};

template < typename T >
class MatlabFilterThermal : public MatlabFilterBase< T, thermal::ThermalElement, ThermalPreperation >
{
    public:
    MatlabFilterThermal( std::string filename )
        : MatlabFilterBase< T, thermal::ThermalElement, ThermalPreperation >( filename ){};
};

} /* namespace */


#endif /* __NO_STRING__ */
#endif /* _FILENAME_ */
