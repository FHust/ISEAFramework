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
* File Name : electrical_data_struct.h
* Creation Date : 29-07-2014
* Last Modified : Mo 07 Mär 2016 16:45:36 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ELECTRICAL_DATA_STRUCT_
#define _ELECTRICAL_DATA_STRUCT_


/// this struct describes the information that are stored by the electrical::TwoPort class
template < typename valueType >
struct ElectricalDataStruct
{
    ElectricalDataStruct( valueType current = 0, valueType voltage = 0, valueType power = 0 )
        : mCurrentValue( current )
        , mVoltageValue( voltage )
        , mPowerValue( power )
    {
    }

    valueType mCurrentValue = 0;
    valueType mVoltageValue = 0;
    valueType mPowerValue = 0;
};
#endif /* _ELECTRICAL_DATA_STRUCT_ */
