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
* File Name : observerclasswrapper.h
* Creation Date : 10-08-2015
* Last Modified : Mo 06 Jun 2016 13:44:29 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef _OBSERVERCLASSWRAPPER_H_
#define _OBSERVERCLASSWRAPPER_H_


// ETC
#include "../../observer/filter/csvfilter.h"
#include "../../observer/filter/decimatefilter.h"
#include "../../observer/filter/matlabFilter.h"
#include "../../observer/filter/stdoutfilter.h"
#include "../../observer/filter/vcpfilter.h"
#include "../../observer/filter/filter.h"
#include "../../observer/filter/benchmarkFilter.h"
#include "../../observer/filter/elementCounterFilter.h"


#include "../../misc/macros.h"
#include "../factory.h"
#include "../classwrapper.h"

namespace factory
{

struct ArgumentTypeObserver
{
    ArgumentTypeObserver(){};
};

/// Classwrapper for ::observer namespace. This template class has to be specialized in order to create an instance of a
/// particular class.
template < typename MatrixT, template < typename > class TConcrete, typename ArgumentType >
class ObserverClassWrapperBase
 : public ClassWrapperBase< observer::Filter< MatrixT, TConcrete, ArgumentType >, ArgumentTypeObserver >
{
    public:
    ObserverClassWrapperBase(){};
};

template < typename MatrixT, template < typename > class FilterType >
class ObserverClassWrapperTwoPort
 : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};
};

template < typename MatrixT, template < typename > class FilterType >
class ObserverClassWrapperThermal
 : public ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >
{
    public:
    ObserverClassWrapperThermal()
        : ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >(){};
};


/// Classwrapper for observer::DecimateFilter
template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::DecimateFilterTwoPort >
 : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );

        return boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >(
         new observer::DecimateFilterTwoPort< MatrixT >( param->GetElementDoubleValue( "TimeDelay" ) ) );
    }
};

template < typename MatrixT >
class ObserverClassWrapperThermal< MatrixT, observer::DecimateFilterThermal >
 : public ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >
{
    public:
    ObserverClassWrapperThermal()
        : ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );

        return boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >(
         new observer::DecimateFilterThermal< MatrixT >( param->GetElementDoubleValue( "TimeDelay" ) ) );
    }
};


/// Classwrapper for observer::CsvFilter
template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::CsvFilterTwoPort >
 : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );
        bool printHeader = true;
        if ( param->HasElement( "PrintHeader" ) )
            printHeader = param->GetElementBoolValue( "PrintHeader" );

        return boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >(
         new observer::CsvFilterTwoPort< MatrixT >( param->GetElementStringValue( "filename" ), printHeader ) );
    }
};

template < typename MatrixT >
class ObserverClassWrapperThermal< MatrixT, observer::CsvFilterThermal >
 : public ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >
{
    public:
    ObserverClassWrapperThermal()
        : ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );
        bool printHeader = true;
        if ( param->HasElement( "PrintHeader" ) )
        {
            printHeader = param->GetElementBoolValue( "PrintHeader" );
        }

        std::string fileNameVertices(
         param->GetElementStringValueWithDefaultValue( "VerticesFile", "Patch_Vertices.csv" ) );
        std::string fileNameAreas( param->GetElementStringValueWithDefaultValue( "AreasFile", "Patch_Areas.csv" ) );
        std::string fileNameVolumes(
         param->GetElementStringValueWithDefaultValue( "VolumesFile", "Patch_Volumes.csv" ) );
        std::string fileNameVolumeNames(
         param->GetElementStringValueWithDefaultValue( "VolumesNameFile", "Patch_VolumeNames.csv" ) );
        return boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >(
         new observer::CsvFilterThermal< MatrixT >( param->GetElementStringValue( "Filename" ), printHeader, fileNameVertices,
                                                    fileNameAreas, fileNameVolumes, fileNameVolumeNames ) );
    }
};

/// Classwrapper for observer::ElementCounterFilterTwoPort
template < typename MatrixT >
class ObserverClassWrapperThermal< MatrixT, observer::ElementCounterFilterThermal >
 : public ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >
{
    public:
    ObserverClassWrapperThermal()
        : ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( param );
        UNUSED( arg );
        return boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >(
         new observer::ElementCounterFilterThermal< MatrixT >() );
    }
};


/// Classwrapper for observer::ElementCounterFilterTwoPort
template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::ElementCounterFilterTwoPort >
 : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( param );
        UNUSED( arg );
        return boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >(
         new observer::ElementCounterFilterTwoPort< MatrixT >() );
    }
};


/// Classwrapper for observer::BenchmarkFilterTwoPort
template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::BenchmarkFilterTwoPort >
 : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( param );
        UNUSED( arg );
        return boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >(
         new observer::BenchmarkFilterTwoPort< MatrixT >() );
    }
};

/// Classwrapper for observer::BenchmarkFilterTwoPort
template < typename MatrixT >
class ObserverClassWrapperThermal< MatrixT, observer::BenchmarkFilterThermal >
 : public ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >
{
    public:
    ObserverClassWrapperThermal()
        : ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( param );
        UNUSED( arg );
        return boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >(
         new observer::BenchmarkFilterThermal< MatrixT >() );
    }
};


/// Classwrapper for observer::StdoutFilter
template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::StdoutFilterTwoPort >
 : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( param );
        UNUSED( arg );
        return boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >(
         new observer::StdoutFilterTwoPort< MatrixT >() );
    }
};

/// Classwrapper for observer::MatlabFilter
template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::MatlabFilterTwoPort >
 : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );

        return boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >(
         new observer::MatlabFilterTwoPort< MatrixT >( param->GetElementStringValue( "Filename" ) ) );
    }
};

template < typename MatrixT >
class ObserverClassWrapperThermal< MatrixT, observer::MatlabFilterThermal >
 : public ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >
{
    public:
    ObserverClassWrapperThermal()
        : ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );

        return boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >(
         new observer::MatlabFilterThermal< MatrixT >( param->GetElementStringValue( "Filename" ) ) );
    }
};

} /* namespace factory */
#endif /* STATECLASSWRAPPER_H_ */
