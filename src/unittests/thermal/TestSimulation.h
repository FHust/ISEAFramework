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
#ifndef _TESTSIMULATION_
#define _TESTSIMULATION_

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4018 )
#endif

#include <boost/numeric/odeint.hpp>
#include <boost/ref.hpp>

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#include <cxxtest/TestSuite.h>
#include <cstring>


class TestSimulation : public CxxTest::TestSuite
{
    public:
    void TestScenario();
    void TestParallelRC();
    void TestErrorHandlingAtConstruction();
    void EvaluateCsvFile( const std::string &line, size_t &i, double &timeEl, double &cur, double &volt, double &soc,
                          double &heat, double &temp );

    private:
    void RunScenario( double electricalStopCriterion, double thermalStopCriterion, bool stopCriterionTriggered );
    struct observerOutput
    {
        size_t mI;
        double mTimeEl;
        double mCur;
        double mVolt;
        double mSoc;
        double mHeat;
        double mTemp;
    };

    protected:
};

#endif
