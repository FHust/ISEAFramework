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
* Last Modified : Do 15 Jun 2017 14:19:41 CEST
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

template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
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
    virtual void PrepareFilter( ArgumentType &prepData ) = 0;

    protected:
    std::vector< double > mTime;
    matlab::MatFile mMatFile;
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
class MatlabFilterBase : public MatlabFilter< T, TConcrete, ArgumentType >
{

    public:
    virtual void PrepareFilter( ArgumentType &prepData ) = 0;
    MatlabFilterBase( std::string filename )
        : MatlabFilter< T, TConcrete, ArgumentType >( filename ){};
};


template < typename T >
class MatlabFilterBase< T, electrical::TwoPort, PreparationType< T > >
 : public MatlabFilter< T, electrical::TwoPort, PreparationType< T > >
{

    private:
    electrical::TwoPort< T > *mRootPort;

    typedef Filter< T, electrical::TwoPort, PreparationType< T > > FilterT;

    public:
    MatlabFilterBase( std::string filename )
        : MatlabFilter< T, electrical::TwoPort, PreparationType< T > >( filename ){};

    ~MatlabFilterBase()
    {
        if ( mCurrentVec.empty() || mVoltageVec.empty() || mPowerVec.empty() || mSocVec.empty() || mTemperatureVec.empty() )
            return;

        size_t resizer = 0;
        for ( size_t i = 0; i < mCurrentVec.size(); ++i )
        {
            if ( mCurrentVec[i].empty() )
            {
                resizer = i;
                break;
            }
        }
        if ( resizer )
        {
            mCurrentVec.resize( resizer );
            mVoltageVec.resize( resizer );
            mPowerVec.resize( resizer );
            mSocVec.resize( resizer );
            mTemperatureVec.resize( resizer );
        }

        this->mMatFile << matlab::MatioData( mCurrentVec, "diga.daten.StromVec" );
        this->mMatFile << matlab::MatioData( mVoltageVec, "diga.daten.SpannungVec" );
        this->mMatFile << matlab::MatioData( mPowerVec, "diga.daten.ThermischLeistungVec" );
        this->mMatFile << matlab::MatioData( mSocVec, "diga.daten.SOCVec" );
        this->mMatFile << matlab::MatioData( mTemperatureVec, "diga.daten.TemperaturVec" );

        if ( mCurrent.empty() || mVoltage.empty() || mPower.empty() )
            return;

        this->mMatFile << matlab::MatioData( mCurrent, "diga.daten.Strom" );
        this->mMatFile << matlab::MatioData( mVoltage, "diga.daten.Spannung" );
        this->mMatFile << matlab::MatioData( mPower, "diga.daten.ThermischLeistung" );
    }

    virtual void PrepareFilter( PreparationType< T > &prePareData )
    {
        InitializeVectors( prePareData.mNumberOfElements );
        mRootPort = prePareData.mRootPort;
    };

    virtual void ProcessData( const typename FilterT::Data_t &data, const double t )
    {

        for ( size_t i = 0; i < data.size(); ++i )
        {
            electrical::TwoPort< T > *port = data[i];
            mVoltageVec[i].push_back( port->GetVoltageValue() );
            mCurrentVec[i].push_back( port->GetCurrentValue() );
            mPowerVec[i].push_back( port->GetPowerValue() );

            if ( port->IsCellelement() )
            {
                electrical::Cellelement< T > *cell = static_cast< electrical::Cellelement< T > * >( port );
                mSocVec[i].push_back( cell->GetSocStateValue() );
                mTemperatureVec[i].push_back( cell->GetThermalState()->GetValue() );
            }
            else
            {
                mSocVec[i].push_back( -1.0 );
                mTemperatureVec[i].push_back( -273 );
            }
        }

        if ( mRootPort )
        {
            electrical::TwoPort< T > *port = mRootPort;
            mVoltage.push_back( port->GetVoltageValue() );
            mCurrent.push_back( port->GetCurrentValue() );
            mPower.push_back( port->GetPowerValue() );
        }
        MatlabFilter< T, electrical::TwoPort, PreparationType< T > >::ProcessData( data, t );
    }

    private:
    void InitializeVectors( const size_t vectorSizes )
    {
        mCurrentVec.resize( vectorSizes );
        mVoltageVec.resize( vectorSizes );
        mSocVec.resize( vectorSizes );
        mPowerVec.resize( vectorSizes );
        mTemperatureVec.resize( vectorSizes );
    };

    std::vector< std::vector< double > > mCurrentVec;
    std::vector< std::vector< double > > mVoltageVec;
    std::vector< std::vector< double > > mSocVec;
    std::vector< std::vector< double > > mPowerVec;
    std::vector< std::vector< double > > mTemperatureVec;

    std::vector< double > mCurrent;
    std::vector< double > mVoltage;
    std::vector< double > mPower;
};

template < typename T >
class MatlabFilterBase< T, thermal::ThermalElement, ThermalPreperation >
 : public MatlabFilter< T, thermal::ThermalElement, ThermalPreperation >
{

    typedef Filter< T, thermal::ThermalElement, ThermalPreperation > FilterT;

    public:
    MatlabFilterBase( std::string filename )
        : MatlabFilter< T, thermal::ThermalElement, ThermalPreperation >( filename )
        , mFileNameVertices( "Patch_Vertices.csv" )
        , mFileNameAreas( "Patch_Areas.csv" )
        , mFileNameAreasElectrical( "Patch_AreasElectrical.csv" )
        , mFileNameElectricThermalMapping( "Patch_ElectricThermalMapping.csv" )
        , mFileNameVolumes( "Patch_Volumes.csv" )
        , mFileNameVolumeNames( "Patch_VolumeNames.csv" )
        , mFileNameVolumeMaterials( "Patch_VolumeMaterials.csv" ){};

    ~MatlabFilterBase()
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
    std::string mFileNameVertices, mFileNameAreas, mFileNameAreasElectrical, mFileNameElectricThermalMapping,
     mFileNameVolumes, mFileNameVolumeNames, mFileNameVolumeMaterials;

    private:
    std::vector< std::vector< double > > mTemperature;
};


template < typename T >
class MatlabFilterTwoPort : public MatlabFilterBase< T, electrical::TwoPort, PreparationType< T > >
{
    public:
    MatlabFilterTwoPort( std::string filename )
        : MatlabFilterBase< T, electrical::TwoPort, PreparationType< T > >( filename ){};
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
