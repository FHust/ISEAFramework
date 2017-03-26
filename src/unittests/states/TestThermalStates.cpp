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
#include "TestThermalStates.h"
#include "../thermal/exception_tester.h"

using namespace state;
static const double sDelta = 0.000001;



void TestState::TestThermalState()
{
    ThermalState<> thermalState(47.0);

    TS_ASSERT_EQUALS(thermalState.GetMaxDt(), std::numeric_limits<double>::max());

    TS_ASSERT_EQUALS(thermalState.GetPowerDissipation(12.0, 12.0), 0.0);
    thermalState.SetFixedPowerDissipation(-34.1);
    TS_ASSERT_EQUALS(thermalState.GetPowerDissipation(12.0, 12.0), -34.1);
    TS_ASSERT_EQUALS(thermalState.GetPowerDissipation(125.6, 12.5), -34.1);
    thermalState.AddPowerDissipation(0.0, 0.0);
    thermalState.AddPowerDissipation(13.2, 1.0);
    thermalState.AddPowerDissipation(4.5, 2.0);
    TS_ASSERT_DELTA(thermalState.GetPowerDissipation(1.0, 0.5), 0.0, sDelta);
    TS_ASSERT_DELTA(thermalState.GetPowerDissipation(2.0, 1.5), 13.2 / 1.5, sDelta);
    TS_ASSERT_DELTA(thermalState.GetPowerDissipation(3.0, 3.0), 13.2 / 3.0 + 4.5 / 3.0, sDelta);
    thermalState.ResetPowerDissipation();
    TS_ASSERT_DELTA(thermalState.GetPowerDissipation(2.546345324546, 0.2), 4.5, sDelta);

    thermalState.AddPowerDissipation(1.5, 3.0);
    thermalState.ResetPowerDissipation();
    thermalState.AddPowerDissipation(1.0, 4.0);
    thermalState.AddPowerDissipation(0.5, 5.0);
    thermalState.ResetPowerDissipationToTime(4.5);
    TS_ASSERT_DELTA(thermalState.GetPowerDissipation(6.0, 2.5), 1.5 * 0.2 + 1.0 * 0.8, sDelta);
    TS_ASSERT_DELTA(thermalState.GetPowerDissipation(6.0, 3.0), 1.5 / 3.0 + 1.0 * 2.0 / 3.0, sDelta);

    TS_ASSERT_DELTA(thermalState.GetValue(), 47.0, sDelta);
    thermalState.AddTemperature(12.4, 0.3);
    TS_ASSERT_DELTA(thermalState.GetValue(), 12.4, sDelta);
    thermalState.ResetTemperature();
    thermalState.AddTemperature(2.6, 0.3);
    thermalState.AddTemperature(2.0, 0.6);
    thermalState.AddTemperature(1.7, 0.6);
    TS_ASSERT_DELTA(thermalState.GetValue(), 2.0, sDelta);

    ThermalState<> thermalStateZero;
    TS_ASSERT_DELTA(thermalStateZero.GetValue(), 0.0, sDelta);

//#if defined(__EXCEPTIONS__)

    TS_ASSERT_THROWS_EQUALS(thermalState.AddTemperature(1.3, 0.0), std::runtime_error &e, strcmp( e.what(), "Volume must be bigger than zero."), 0);
    TS_ASSERT_THROWS_EQUALS(thermalState.GetPowerDissipation(6.0, 3.01), std::runtime_error &e, strcmp( e.what(),
                            "Time passed for GetPowerDissipation() must be equal or later than starting time of last electrical simulation."), 0);
/*
    TS_ASSERT_THROWS_EQUALS(thermalState.AddTemperature(1.3, 0.0), const std::runtime_error &e,
                            CmpInit(e, "volume must be bigger than zero", "AddTemperature"), 0);
    TS_ASSERT_THROWS_EQUALS(thermalState.GetPowerDissipation(6.0, 3.01), const std::runtime_error &e,
                            CmpUnexp(e, "Time passed for GetPowerDissipation() must be equal or later than starting time of last electrical simulation", "GetPowerDissipation"), 0);
*/

//#endif
}
