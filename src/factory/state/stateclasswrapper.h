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
 * stateclasswrapper.h
 *
 *  Created on: 17.09.2013
 *      Author: chris
 */

#ifndef STATECLASSWRAPPER_H_
#define STATECLASSWRAPPER_H_

// STD
#include <exception>
#include <algorithm>
#include <string>
#include <cctype>

// ETC
#include "../factory.h"
#include "../classwrapper.h"

#include "../../electrical/electrical_data_struct.h"

// ETC
#include "../../states/soc.h"
#include "../../states/surface_soc.h"
#include "../../states/thermal_state.h"
#include "../../states/valueStateWrapper.h"


using namespace ::state;

namespace factory
{

struct ArgumentTypeState
{
    ArgumentTypeState()
        : mCellDiscretization( 1 ){};
    size_t mCellDiscretization = 1;
    boost::shared_ptr< ElectricalDataStruct< double > > mElectricalDataStruct =
     boost::shared_ptr< ElectricalDataStruct< double > >( new ElectricalDataStruct< double > );
};

/// Classwrapper for ::state namespace. This template class has to be specialized in order to create an instance of a
/// particular class.
template < class ValueT >
class StateClassWrapperBase : public ClassWrapperBase< Dgl_state, ArgumentTypeState >
{
    public:
    StateClassWrapperBase( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : mStateFactory( stateFactory ){};

    protected:
    /// Get a ::state Factory.
    Factory< ::state::Dgl_state, ArgumentTypeState >* GetStateFactory() { return mStateFactory; }

    private:
    Factory< ::state::Dgl_state, ArgumentTypeState >* const mStateFactory;
};

template < class TConcrete >
class StateClassWrapper : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};
};

/// Classwrapper for electrical::state::Soc
template <>
class StateClassWrapper< electrical::state::Soc > : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};

    boost::shared_ptr< Dgl_state > CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeState* arg = 0 )
    {
        double maxCapacity = param->GetElementDoubleValue( "MaxCapacity" );
        double currentState = param->GetElementDoubleValue( "InitialSoc" );
        std::vector< double > measurmentpoints = param->GetElementDoubleVecValue( "MeasurementPoints" );

        if ( arg )
            maxCapacity /= arg->mCellDiscretization;

        return boost::shared_ptr< Dgl_state >( new electrical::state::Soc( maxCapacity, currentState, measurmentpoints ) );
    }
};

/// Classwrapper for ::state::ThermalState
template <>
class StateClassWrapper< ::state::ThermalState< double > > : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};
    boost::shared_ptr< Dgl_state > CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeState* /* arg */ = 0 )
    {
        if ( param->HasElement( "InitialTemperature" ) )
        {
            double initTemp = param->GetElementDoubleValue( "InitialTemperature" );
            return boost::shared_ptr< Dgl_state >( new ::state::ThermalState< double >( initTemp ) );
        }
        else
            return boost::shared_ptr< Dgl_state >( new ::state::ThermalState< double >() );
    }
};

/// Classwrapper for electrical::state::SurfaceSoc
template <>
class StateClassWrapper< electrical::state::SurfaceSoc > : public StateClassWrapperBase< double >
{

    public:
    StateClassWrapper( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};
    boost::shared_ptr< Dgl_state > CreateInstance( const xmlparser::XmlParameter* /* param */, const ArgumentTypeState* /* arg */ = 0 )
    {
        return boost::shared_ptr< Dgl_state >( new electrical::state::SurfaceSoc() );
    }
};

/// Classwrapper for state::ValueStateWrapper

template <>
class StateClassWrapper< ::state::ValueStateWrapper< double > > : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< ::state::Dgl_state, ArgumentTypeState >* stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};
    boost::shared_ptr< Dgl_state > CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeState* arg = 0 )
    {
        if ( !arg )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoArgumentForStateFactory" );
        }

        std::string stateType( param->GetElementAttribute( "StateType" ) );
        std::transform( stateType.begin(), stateType.end(), stateType.begin(), ::tolower );

        if ( stateType.find( "current" ) != std::string::npos )
        {
            return boost::shared_ptr< Dgl_state >(
             new ::state::ValueStateWrapper< double >( &( arg->mElectricalDataStruct->mCurrentValue ) ) );
        }
        else if ( stateType.find( "voltage" ) != std::string::npos )
        {
            return boost::shared_ptr< Dgl_state >(
             new ::state::ValueStateWrapper< double >( &( arg->mElectricalDataStruct->mVoltageValue ) ) );
        }
        else if ( stateType.find( "power" ) != std::string::npos )
        {
            return boost::shared_ptr< Dgl_state >(
             new ::state::ValueStateWrapper< double >( &( arg->mElectricalDataStruct->mPowerValue ) ) );
        }
        // ERROR
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedStateType", stateType.c_str() );

        // should never reach here, but for compiler warnings this line is added
        return boost::shared_ptr< Dgl_state >();
    }
};


} /* namespace factory */
#endif /* STATECLASSWRAPPER_H_ */
