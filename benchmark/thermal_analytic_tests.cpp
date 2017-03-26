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
#include "../src/thermal/thermal_simulation.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include "../src/geometry/math/bessel.h"
#include <fstream>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4018)
#endif
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/ref.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif



//forward declarations
const double thermal::globalMaxValue = 1000000000.0;
const double thermal::globalMinValue = - thermal::globalMaxValue;

using namespace thermal;
using namespace geometry;



template<typename T = double>
class CylindricBlockBuilder
{
public:
    CylindricBlockBuilder(T rhoStart
                        , T rhoEnd
                        , T deltaPhi
                        , T dZ
                        , size_t nRho
                        , size_t nPhi
                        , const Material<T> *material
                        , T k_r
                        , std::ofstream &analyticCsv
                        , std::ofstream &simulatedCsv
                        , bool twoDimensional)
        : mNRho(nRho)
    {
        const T dPhi = deltaPhi / nPhi;
        const T dRho = (rhoEnd - rhoStart) / nRho;
        vector<T> phiCoordinates;

    
        analyticCsv << "Rho";
        simulatedCsv << "Rho";
        for(size_t j = 0; j < nPhi; ++j)
            for(size_t i = 0; i < nRho; ++i)
            {
                mThermalElements.push_back(shared_ptr<ThermalElement<T> >(new CylindricElement<>(
                        Cartesian<>(0.0, 0.0, 0.0)
                      , Cylindric<>(rhoStart + dRho * i, Angle<>::Rad(dPhi * j), 0.0)
                      , Cylindric<>(dRho, Angle<>::Rad(dPhi), dZ)
                      , 25.0
                      , material)));
                const T rho = rhoStart + (0.5 + i) * dRho;
                const T phi = (0.5 + j) * dPhi;

                if(twoDimensional)
                    mThermalElements.back()->SetTemperature(25.0 + 50.0 * Bessel2(k_r * rho) * cos(2.0 * phi));
                else
                    mThermalElements.back()->SetTemperature(25.0 + 25.0 * Bessel0(k_r * rho));

                analyticCsv << ", " << rho;
                simulatedCsv << ", " << rho;
                phiCoordinates.push_back(phi);
            }
        analyticCsv << "\n";
        simulatedCsv << "\n";

        if(twoDimensional)
        {
            analyticCsv << "Phi";
            simulatedCsv << "Phi";
            BOOST_FOREACH(const double &coordinate, phiCoordinates)
            {
                analyticCsv << ", " << coordinate;
                simulatedCsv << ", " << coordinate;
            }
            analyticCsv << "\n";
            simulatedCsv << "\n";
        }
            
        
        mConductivityMatrix.resize(mThermalElements.size());
        for(size_t j = 0; j < nPhi; ++j)
            for(size_t i = 0; i < nRho; ++i)
                {
                    T conductivity = 0.0;
                    T totalConductivity = 0.0;
                    const T lambda = material->GetConductivity(0);
                
                    if(i > 0)
                    {
                        conductivity = lambda * dZ * (rhoStart + dRho * i) * dPhi / dRho;
                        totalConductivity += conductivity;
                        mConductivityMatrix[Index(i, j)].push_back(MakeIndexedValue<T>(Index(i - 1, j), conductivity));
                    }
                    if(i < nRho - 1)
                    {
                        conductivity = lambda * dZ * (rhoStart + dRho * (i+1)) * dPhi / dRho;
                        totalConductivity += conductivity;
                        mConductivityMatrix[Index(i, j)].push_back(MakeIndexedValue<T>(Index(i + 1, j), conductivity));
                    }
                    if(j > 0)
                    {
                        conductivity = lambda * dZ * dRho / ((rhoStart + (0.5 + i) * dRho) * dPhi);
                        totalConductivity += conductivity;
                        mConductivityMatrix[Index(i, j)].push_back(MakeIndexedValue<T>(Index(i, j - 1), conductivity));
                    }
                    if(j < nPhi - 1)
                    {
                        conductivity = lambda * dZ * dRho / ((rhoStart + (0.5 + i) * dRho) * dPhi);
                        totalConductivity += conductivity;
                        mConductivityMatrix[Index(i, j)].push_back(MakeIndexedValue<T>(Index(i, j + 1), conductivity));
                    }

                    mConductivityMatrix[Index(i, j)].push_back(MakeIndexedValue<T>(Index(i, j), - totalConductivity));
                }
    }

    vector<shared_ptr<ThermalElement<T> > > mThermalElements;
    vector<vector<IndexedValue<double> > > mConductivityMatrix;
private:
    size_t Index(size_t rhoIndex, size_t phiIndex)
    {
        return rhoIndex + phiIndex * mNRho;
    }
    
    const size_t mNRho;
};

double MakeStationaryValue(double x, double length, double conductivity, double heatGeneration)
{
    if(x < 0.0 || x > length)
        throw std::exception("MakeStationaryValue - invalid x parameter");
    

    double returnValue;

    if(x < 0.5 * length)
        returnValue = heatGeneration / conductivity * (- 0.5  * x * x + 0.125 * length * length);
    else
        returnValue = heatGeneration / conductivity * (0.5  * x * x - x * length + 0.375 * length * length);
    
    return returnValue;
}





int main()
{
    std::ofstream summaryTxt("summary.txt");
    

    
    {
        std::ofstream simulatedCsv("comsol_simulated.csv");
        double maxError = 0.0;

        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromFile("./ComsolTest.xml");
        thermal::Tolerance<> tolerance(0.000001, geometry::Angle<>::Deg(0.001), 0.1);
        simulation::ThermalSimulation< myMatrixType, double > simulation(parser.GetRoot(), tolerance, 0.0, 0.0, std::numeric_limits<double>::max(), 0, 0);
        

        vector<double> temperatureVector, initialVector, dxdt;
        simulation.mThermalSystem->GetTemperatureVector(temperatureVector);
        simulation.mThermalSystem->GetTemperatureVector(initialVector);
        

        printf("Grid vertices of comsol test: n, x, y, z\n");
        Cartesian<> gridVertex = simulation.mThermalSystem->mThermalElements[0]->GetGridVertex();
        printf("%i, %.6f, %.6f, %.6f\n", 1, gridVertex.GetX(), gridVertex.GetY(), gridVertex.GetZ());
        gridVertex = simulation.mThermalSystem->mThermalElements[2*24]->GetGridVertex();
        printf("%i, %.6f, %.6f, %.6f\n", 2, gridVertex.GetX(), gridVertex.GetY(), gridVertex.GetZ());
        gridVertex = simulation.mThermalSystem->mThermalElements[4*24]->GetGridVertex();
        printf("%i, %.6f, %.6f, %.6f\n", 3, gridVertex.GetX(), gridVertex.GetY(), gridVertex.GetZ());
        gridVertex = simulation.mThermalSystem->mThermalElements[5*24 + 12]->GetGridVertex();
        printf("%i, %.6f, %.6f, %.6f\n", 4, gridVertex.GetX(), gridVertex.GetY(), gridVertex.GetZ());
        gridVertex = simulation.mThermalSystem->mThermalElements[5*24 + 12 + 2*25]->GetGridVertex();
        printf("%i, %.6f, %.6f, %.6f\n", 5, gridVertex.GetX(), gridVertex.GetY(), gridVertex.GetZ());
        gridVertex = simulation.mThermalSystem->mThermalElements[5*24 + 12 + 4*25]->GetGridVertex();
        printf("%i, %.6f, %.6f, %.6f\n", 6, gridVertex.GetX(), gridVertex.GetY(), gridVertex.GetZ());
        gridVertex = simulation.mThermalSystem->mThermalElements[5*24 + ((5+2) * 25) + 10]->GetGridVertex();
        printf("%i, %.6f, %.6f, %.6f\n", 7, gridVertex.GetX(), gridVertex.GetY(), gridVertex.GetZ());
        gridVertex = simulation.mThermalSystem->mThermalElements[5*24 + ((5+2) * 25) + 10 + 2]->GetGridVertex();
        printf("%i, %.6f, %.6f, %.6f\n", 8, gridVertex.GetX(), gridVertex.GetY(), gridVertex.GetZ());
        gridVertex = simulation.mThermalSystem->mThermalElements[5*24 + ((5+2) * 25) + 10 + 4]->GetGridVertex();
        printf("%i, %.6f, %.6f, %.6f\n", 9, gridVertex.GetX(), gridVertex.GetY(), gridVertex.GetZ());



        simulatedCsv << 0.0 << ", ";
        simulatedCsv << simulation.mThermalSystem->mThermalElements[0]->GetTemperature() + 273.13 << ", ";
        simulatedCsv << simulation.mThermalSystem->mThermalElements[2*24]->GetTemperature()  + 273.13 << ", ";
        simulatedCsv << simulation.mThermalSystem->mThermalElements[4*24]->GetTemperature()  + 273.13 << ", ";
        simulatedCsv << simulation.mThermalSystem->mThermalElements[5*24 + 12]->GetTemperature()  + 273.13 << ", ";
        simulatedCsv << simulation.mThermalSystem->mThermalElements[5*24 + 12 + 2*25]->GetTemperature()  + 273.13 << ", ";
        simulatedCsv << simulation.mThermalSystem->mThermalElements[5*24 + 12 + 4*25]->GetTemperature()  + 273.13 << ", ";
        simulatedCsv << simulation.mThermalSystem->mThermalElements[5*24 + ((5+2) * 25) + 10]->GetTemperature()  + 273.13 << ", ";
        simulatedCsv << simulation.mThermalSystem->mThermalElements[5*24 + ((5+2) * 25) + 10 + 2]->GetTemperature()  + 273.13 << ", ";
        simulatedCsv << simulation.mThermalSystem->mThermalElements[5*24 + ((5+2) * 25) + 10 + 4]->GetTemperature()  + 273.13 << "\n";



        double time = 0.000;
        double dt = 100.0;
        boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> > >::type stepper = make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> >() );
        simulation.mThermalSystem->Update(1.0);
        
        for(size_t i = 0; i < 200; ++i)
        {
            for(;;)
            {
                double restTimeOfSimulationStep = 100.0 * (i+1) - time;
                if(dt < restTimeOfSimulationStep)
                {
                    while(stepper.try_step(boost::ref(*simulation.mThermalSystem), temperatureVector, time, dt) != boost::numeric::odeint::success)
                    {}
                }
                else
                {
                    while(stepper.try_step(boost::ref(*simulation.mThermalSystem), temperatureVector, time, restTimeOfSimulationStep) != boost::numeric::odeint::success)
                    {}
                    if(time >= 100.0 * (i+1))
                        break;
                    else
                        dt = restTimeOfSimulationStep;
                }
            }

            simulation.mThermalSystem->SetTemperatureVector(temperatureVector);
            

            simulatedCsv << time << ", ";

            simulatedCsv << simulation.mThermalSystem->mThermalElements[0]->GetTemperature() + 273.13 << ", ";
            simulatedCsv << simulation.mThermalSystem->mThermalElements[2*24]->GetTemperature()  + 273.13 << ", ";
            simulatedCsv << simulation.mThermalSystem->mThermalElements[4*24]->GetTemperature()  + 273.13 << ", ";
            simulatedCsv << simulation.mThermalSystem->mThermalElements[5*24 + 12]->GetTemperature()  + 273.13 << ", ";
            simulatedCsv << simulation.mThermalSystem->mThermalElements[5*24 + 12 + 2*25]->GetTemperature()  + 273.13 << ", ";
            simulatedCsv << simulation.mThermalSystem->mThermalElements[5*24 + 12 + 4*25]->GetTemperature()  + 273.13 << ", ";
            simulatedCsv << simulation.mThermalSystem->mThermalElements[5*24 + ((5+2) * 25) + 10]->GetTemperature()  + 273.13 << ", ";
            simulatedCsv << simulation.mThermalSystem->mThermalElements[5*24 + ((5+2) * 25) + 10 + 2]->GetTemperature()  + 273.13 << ", ";
            simulatedCsv << simulation.mThermalSystem->mThermalElements[5*24 + ((5+2) * 25) + 10 + 4]->GetTemperature()  + 273.13 << "\n";

            printf("%i,", i);
        }

    
        printf("\nTesting of comsol done\n\n");
        summaryTxt << "Testing of comsol done " << std::endl;
    }


    
    {
        std::ofstream analyticCsv("cart_analytic.csv");
        std::ofstream simulatedCsv("cart_simulated.csv");
        double maxError = 0.0;
        
        const double length = 1.0;
        thermal::Material<> material(250.0, 1000.0, 50.0, 50.0, 50.0);
        RectangularBlock<> block(Cartesian<>(0.0, 0.0, 0.0)
                               , length, length, 0.05
                               , 40, 40, 1
                               , &material
                               , 25.0);
        
        vector<shared_ptr<ThermalElement<> > > thermalElements;
        vector<vector<IndexedValue<double> > > conductivityMatrix;
        vector<IndexedArea<double> > surfaceElements;
        shared_ptr<BlockGeometry<> > blockGeometry;
        block.CreateData(thermalElements, conductivityMatrix, surfaceElements, blockGeometry);
        
        const double k_x = Angle<>::pi / length;
        const double k_t = k_x * k_x * material.GetConductivity(0) / (material.GetDensity() * material.GetSpecificCapacity());
        analyticCsv << "X";
        simulatedCsv << "X";
        BOOST_FOREACH(shared_ptr<ThermalElement<> > &thermalElement, thermalElements)
        {
            thermalElement->SetTemperature(25.0 + 25.0 * cos(k_x * thermalElement->GetGridVertex().GetX()));
            analyticCsv << ", " << thermalElement->GetGridVertex().GetX();
            simulatedCsv << ", " << thermalElement->GetGridVertex().GetX();
        }
        analyticCsv << "\n";
        simulatedCsv << "\n";

        OdeSystemThermal<> system(thermalElements
                                , conductivityMatrix
                                , vector<vector<TaylorData<double> > >(thermalElements.size())
                                , vector<shared_ptr<DefaultConvection<> > >(3)
                                , shared_ptr<Radiation<> >()
                                , 25.0
                                , vector<shared_ptr< ::state::ThermalState<double> > > ());
                
        vector<double> temperatureVector, initialVector, dxdt(thermalElements.size());
        system.GetTemperatureVector(temperatureVector);
        system.GetTemperatureVector(initialVector);
        

        
        double time = 0.000;
        double dt = 100.0;
        boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> > >::type stepper = make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> >() );
        size_t outputCounter = 0;
        while(time < 7000.0)
        {
            while(stepper.try_step(boost::ref(system), temperatureVector, time, dt) != boost::numeric::odeint::success)
            {}

            system.SetTemperatureVector(temperatureVector);
            bool IsOutput = time / 100 > outputCounter;
            
            if(IsOutput)
            {
                analyticCsv << time << ", ";
                simulatedCsv << time << ", ";
                printf("%i,", outputCounter);
            }
            
            for(size_t i = 0; i < system.GetOdeSystemSize(); ++i)
            {
                const double analytic = 25.0 + (initialVector[i] - 25.0) * exp(- k_t * time);
                
                double error = fabs(analytic - temperatureVector[i]);
                if(error > maxError)
                    maxError = error;
                
                if(IsOutput)
                {
                    if(i != 0)
                    {
                        analyticCsv << ", ";
                        simulatedCsv << ", ";
                    }
                    analyticCsv << analytic;
                    simulatedCsv << temperatureVector[i];
                }
            }

            if(IsOutput)
            {
                analyticCsv << "\n";
                simulatedCsv << "\n";
                ++outputCounter;
            }
        }

        printf("\nTesting of cartesian conductivity, maximal error %.3f %% of 25K\n\n", maxError / 25.0 * 100.0);
        summaryTxt << "Testing of cartesian conductivity, maximal error " << maxError / 25.0 * 100.0 <<  " % of 25K" << std::endl;
    }



    {
        std::ofstream analyticCsv("cart2D_analytic.csv");
        std::ofstream simulatedCsv("cart2D_simulated.csv");
        double maxError = 0.0;
        
        const double length = 1.0;
        thermal::Material<> material(250.0, 1000.0, 50.0, 50.0, 50.0);
        RectangularBlock<> block(Cartesian<>(0.0, 0.0, 0.0)
                               , length, length, 0.05
                               , 40, 40, 1
                               , &material
                               , 25.0);
        
        vector<shared_ptr<ThermalElement<> > > thermalElements;
        vector<vector<IndexedValue<double> > > conductivityMatrix;
        vector<IndexedArea<double> > surfaceElements;
        shared_ptr<BlockGeometry<> > blockGeometry;
        block.CreateData(thermalElements, conductivityMatrix, surfaceElements, blockGeometry);
        
        const double k_x = Angle<>::pi / length;
        const double k_y = Angle<>::pi / length;
        const double k_t = (k_x * k_x + k_y * k_y) * material.GetConductivity(0) / (material.GetDensity() * material.GetSpecificCapacity());
        analyticCsv << "X";
        simulatedCsv << "X";
        BOOST_FOREACH(shared_ptr<ThermalElement<> > &thermalElement, thermalElements)
        {
            thermalElement->SetTemperature(25.0 + 25.0 * cos(k_x * thermalElement->GetGridVertex().GetX()) * cos(k_y * thermalElement->GetGridVertex().GetY()));
            analyticCsv << ", " << thermalElement->GetGridVertex().GetX();
            simulatedCsv << ", " << thermalElement->GetGridVertex().GetX();
        }
        analyticCsv << "\n" << "Y";
        simulatedCsv << "\n" << "Y";
                BOOST_FOREACH(shared_ptr<ThermalElement<> > &thermalElement, thermalElements)
        {
            analyticCsv << ", " << thermalElement->GetGridVertex().GetY();
            simulatedCsv << ", " << thermalElement->GetGridVertex().GetY();
        }
        analyticCsv << "\n";
        simulatedCsv << "\n";

        OdeSystemThermal<> system(thermalElements
                                , conductivityMatrix
                                , vector<vector<TaylorData<double> > >(thermalElements.size())
                                , vector<shared_ptr<DefaultConvection<> > >(3)
                                , shared_ptr<Radiation<> >()
                                , 25.0
                                , vector<shared_ptr< ::state::ThermalState<double> > > ());
                
        vector<double> temperatureVector, initialVector, dxdt(thermalElements.size());
        system.GetTemperatureVector(temperatureVector);
        system.GetTemperatureVector(initialVector);
        

        
        double time = 0.000;
        double dt = 100.0;
        boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> > >::type stepper = make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> >() );
        size_t outputCounter = 0;
        while(time < 3500.0)
        {
            while(stepper.try_step(boost::ref(system), temperatureVector, time, dt) != boost::numeric::odeint::success)
            {}

            system.SetTemperatureVector(temperatureVector);
            bool IsOutput = time / 100 > outputCounter;
            
            if(IsOutput)
            {
                analyticCsv << time << ", ";
                simulatedCsv << time << ", ";
                printf("%i,", outputCounter);
            }
            
            for(size_t i = 0; i < system.GetOdeSystemSize(); ++i)
            {
                const double analytic = 25.0 + (initialVector[i] - 25.0) * exp(- k_t * time);
                
                double error = fabs(analytic - temperatureVector[i]);
                if(error > maxError)
                    maxError = error;
                
                if(IsOutput)
                {
                    if(i != 0)
                    {
                        analyticCsv << ", ";
                        simulatedCsv << ", ";
                    }
                    analyticCsv << analytic;
                    simulatedCsv << temperatureVector[i];
                }
            }

            if(IsOutput)
            {
                analyticCsv << "\n";
                simulatedCsv << "\n";
                ++outputCounter;
            }
        }

        printf("\nTesting of two dimensional cartesian conductivity, maximal error %.3f %% of 25K\n\n", maxError / 25.0 * 100.0);
        summaryTxt << "Testing of two dimensional cartesian conductivity, maximal error " << maxError / 25.0 * 100.0 <<  " % of 25K" << std::endl;
    }

    
    
    {
        std::ofstream analyticCsv("cyl_analytic.csv");
        std::ofstream simulatedCsv("cyl_simulated.csv");
        double maxError = 0.0;
        
        const double angular = 2 * Angle<>::pi;
        const double radial = 1.0;
        const double startRadial = 0.0;
        const size_t nPhi = 10;
        thermal::Material<> material(250.0, 1000.0, 50.0, 50.0, 50.0);
        const double k_r = bessel0ZP / radial;
        const double k_t = k_r * k_r * material.GetConductivity(0) / (material.GetDensity() * material.GetSpecificCapacity());
        
        CylindricBlockBuilder<> block(startRadial, radial, angular, 0.05, 40, nPhi, &material, k_r, analyticCsv, simulatedCsv, false);

        OdeSystemThermal<> system(block.mThermalElements
                                , block.mConductivityMatrix
                                , vector<vector<TaylorData<double> > >(block.mThermalElements.size())
                                , vector<shared_ptr<DefaultConvection<> > >(3)
                                , shared_ptr<Radiation<> >()
                                , 25.0
                                , vector<shared_ptr< ::state::ThermalState<> > > ());

        vector<size_t> vectorIndices;
        for(size_t i = 0; i < nPhi; ++i)
            vectorIndices.push_back(i * 40 + 39);
        system.SetDirichletBoundaryCondition(vectorIndices, 25.0, material.GetConductivity(0)
            * 0.05 * (radial - (radial - startRadial) / 40.0 / 4) * angular / nPhi / ((radial - startRadial) / 40.0 / 2));
                
        vector<double> temperatureVector, initialVector, dxdt;
        system.GetTemperatureVector(temperatureVector);
        system.GetTemperatureVector(initialVector);
        

        
        double time = 0.000;
        double dt = 100.0;
        boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> > >::type stepper = make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> >() );
        size_t outputCounter = 0;
        while(time < 10000.0)
        {
            while(stepper.try_step(boost::ref(system), temperatureVector, time, dt) != boost::numeric::odeint::success)
            {}

            system.SetTemperatureVector(temperatureVector);
            bool IsOutput = time / 100 > outputCounter;
            
            if(IsOutput)
            {
                analyticCsv << time << ", ";
                simulatedCsv << time << ", ";
                printf("%i,", outputCounter);
            }
            
            for(size_t i = 0; i < system.GetOdeSystemSize(); ++i)
            {
                const double analytic = 25.0 + (initialVector[i] - 25.0) * exp(- k_t * time);
                
                double error = fabs(analytic - temperatureVector[i]);
                if(error > maxError)
                    maxError = error;
                
                if(IsOutput)
                {
                    if(i != 0)
                    {
                        analyticCsv << ", ";
                        simulatedCsv << ", ";
                    }
                    analyticCsv << analytic;
                    simulatedCsv << temperatureVector[i];
                }
            }

            if(IsOutput)
            {
                analyticCsv << "\n";
                simulatedCsv << "\n";
                ++outputCounter;
            }
        }
    
        printf("\nTesting of cylindric conductivity, maximal error %.3f %% of 25K\n\n", maxError / 25.0 * 100.0);
        summaryTxt << "Testing of cylindric conductivity, maximal error " << maxError / 25.0 * 100.0 <<  " % of 25K" << std::endl;
    }

    
    
    {
        std::ofstream analyticCsv("cyl2D_analytic.csv");
        std::ofstream simulatedCsv("cyl2D_simulated.csv");
        double maxError = 0.0;
        
        const double angular = Angle<>::pi / 2;
        const double radial = 1.0;
        const double startRadial = radial * bessel2Max / bessel2ZP;
        const size_t nPhi = 40;
        thermal::Material<> material(250.0, 1000.0, 50.0, 50.0, 50.0);
        const double k_r = bessel2ZP / radial;
        const double k_t = k_r * k_r * material.GetConductivity(0) / (material.GetDensity() * material.GetSpecificCapacity());
        
        CylindricBlockBuilder<> block(startRadial, radial, angular, 0.05, 40, nPhi, &material, k_r, analyticCsv, simulatedCsv, true);

        OdeSystemThermal<> system(block.mThermalElements
                                , block.mConductivityMatrix
                                , vector<vector<TaylorData<double> > >(block.mThermalElements.size())
                                , vector<shared_ptr<DefaultConvection<> > >(3)
                                , shared_ptr<Radiation<> >()
                                , 25.0
                                , vector<shared_ptr< ::state::ThermalState<> > > ());

        vector<size_t> vectorIndices;
        for(size_t i = 0; i < nPhi; ++i)
            vectorIndices.push_back(i * 40 + 39);
        system.SetDirichletBoundaryCondition(vectorIndices, 25.0, material.GetConductivity(0)
            * 0.05 * (radial - (radial - startRadial) / 40.0 / 4) * angular / nPhi / ((radial - startRadial) / 40.0 / 2));
                
        vector<double> temperatureVector, initialVector, dxdt;
        system.GetTemperatureVector(temperatureVector);
        system.GetTemperatureVector(initialVector);
        

        
        double time = 0.000;
        double dt = 100.0;
        boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> > >::type stepper = make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> >() );
        size_t outputCounter = 0;
        while(time < 2600.0)
        {
            while(stepper.try_step(boost::ref(system), temperatureVector, time, dt) != boost::numeric::odeint::success)
            {}

            system.SetTemperatureVector(temperatureVector);
            bool IsOutput = time / 100 > outputCounter;
            
            if(IsOutput)
            {
                analyticCsv << time << ", ";
                simulatedCsv << time << ", ";
                printf("%i,", outputCounter);
            }
            
            for(size_t i = 0; i < system.GetOdeSystemSize(); ++i)
            {
                const double analytic = 25.0 + (initialVector[i] - 25.0) * exp(- k_t * time);
                
                double error = fabs(analytic - temperatureVector[i]);
                if(error > maxError)
                    maxError = error;
                
                if(IsOutput)
                {
                    if(i != 0)
                    {
                        analyticCsv << ", ";
                        simulatedCsv << ", ";
                    }
                    analyticCsv << analytic;
                    simulatedCsv << temperatureVector[i];
                }
            }

            if(IsOutput)
            {
                analyticCsv << "\n";
                simulatedCsv << "\n";
                ++outputCounter;
            }
        }
    
        printf("\nTesting of two dimensional cylindric conductivity, maximal error %.3f %% of 25K\n\n", maxError / 25.0 * 100.0);
        summaryTxt << "Testing of two dimensional cylindric conductivity, maximal error " << maxError / 25.0 * 100.0 <<  " % of 25K" << std::endl;
    }

    
    
    {
        std::ofstream analyticCsv("displaced_analytic.csv");
        std::ofstream simulatedCsv("displaced_simulated.csv");
        double maxError = 0.0;

        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromFile("./DisplacedRectangularblocks.xml");
        thermal::Tolerance<> tolerance(0.000001, geometry::Angle<>::Deg(0.001), 0.1);
        simulation::ThermalSimulation< myMatrixType, double > simulation(parser.GetRoot(), tolerance, 0.0, 0.0, std::numeric_limits<double>::max(), 0, 0);

        double length = 1.0;//Take care, that this corresponds to data in xml-file
        thermal::Material<> material(250.0, 1000.0, 50.0, 50.0, 50.0);//Take care, that this corresponds to data in xml-file
        const double k_x = Angle<>::pi / length;
        const double k_t = k_x * k_x * material.GetConductivity(0) / (material.GetDensity() * material.GetSpecificCapacity());
        analyticCsv << "X";
        simulatedCsv << "X";
        BOOST_FOREACH(shared_ptr<ThermalElement<> > &thermalElement, simulation.mThermalSystem->mThermalElements)
        {
            thermalElement->SetTemperature(25.0 + 25.0 * cos(k_x * thermalElement->GetGridVertex().GetX()));
            analyticCsv << ", " << thermalElement->GetGridVertex().GetX();
            simulatedCsv << ", " << thermalElement->GetGridVertex().GetX();
        }
        analyticCsv << "\n";
        simulatedCsv << "\n";

        vector<double> temperatureVector, initialVector, dxdt;
        simulation.mThermalSystem->GetTemperatureVector(temperatureVector);
        simulation.mThermalSystem->GetTemperatureVector(initialVector);
        

        
        double time = 0.000;
        double dt = 100.0;
        boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> > >::type stepper = make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> >() );
        size_t outputCounter = 0;
        while(time < 7000.0)
        {
            while(stepper.try_step(boost::ref(*simulation.mThermalSystem), temperatureVector, time, dt) != boost::numeric::odeint::success)
            {}

            simulation.mThermalSystem->SetTemperatureVector(temperatureVector);
            bool IsOutput = time / 100 > outputCounter;
            
            if(IsOutput)
            {
                analyticCsv << time << ", ";
                simulatedCsv << time << ", ";
                printf("%i,", outputCounter);
            }
            
            for(size_t i = 0; i < simulation.mThermalSystem->GetOdeSystemSize(); ++i)
            {
                const double analytic = 25.0 + (initialVector[i] - 25.0) * exp(- k_t * time);
                
                double error = fabs(analytic - temperatureVector[i]);
                if(error > maxError)
                    maxError = error;
                
                if(IsOutput)
                {
                    if(i != 0)
                    {
                        analyticCsv << ", ";
                        simulatedCsv << ", ";
                    }
                    analyticCsv << analytic;
                    simulatedCsv << temperatureVector[i];
                }
            }

            if(IsOutput)
            {
                analyticCsv << "\n";
                simulatedCsv << "\n";
                ++outputCounter;
            }
        }
    
        printf("\nTesting of displaced conductivity, maximal error %.3f %% of 25K\n\n", maxError / 25.0 * 100.0);
        summaryTxt << "Testing of displaced conductivity, maximal error " << maxError / 25.0 * 100.0 <<  " % of 25K" << std::endl;
    }
    

    
    {
        std::ofstream analyticCsv("displaced2D_analytic.csv");
        std::ofstream simulatedCsv("displaced2D_simulated.csv");
        double maxError = 0.0;

        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromFile("./DisplacedRectangularblocks.xml");
        thermal::Tolerance<> tolerance(0.000001, geometry::Angle<>::Deg(0.001), 0.1);
        simulation::ThermalSimulation< myMatrixType, double > simulation(parser.GetRoot(), tolerance, 0.0, 0.0, std::numeric_limits<double>::max(), 0, 0);

        double length = 1.0;//Take care, that this corresponds to data in xml-file
        thermal::Material<> material(250.0, 1000.0, 50.0, 50.0, 50.0);//Take care, that this corresponds to data in xml-file
        const double k_x = Angle<>::pi / length;
        const double k_y = Angle<>::pi / length;
        const double k_t = (k_x * k_x + k_y * k_y) * material.GetConductivity(0) / (material.GetDensity() * material.GetSpecificCapacity());
        analyticCsv << "X";
        simulatedCsv << "X";
        BOOST_FOREACH(shared_ptr<ThermalElement<> > &thermalElement, simulation.mThermalSystem->mThermalElements)
        {
            thermalElement->SetTemperature(25.0 + 25.0 * cos(k_x * thermalElement->GetGridVertex().GetX()) * cos(k_y * thermalElement->GetGridVertex().GetY()));
            analyticCsv << ", " << thermalElement->GetGridVertex().GetX();
            simulatedCsv << ", " << thermalElement->GetGridVertex().GetX();
        }
        analyticCsv << "\n" << "Y";
        simulatedCsv << "\n" << "Y";
        BOOST_FOREACH(shared_ptr<ThermalElement<> > &thermalElement, simulation.mThermalSystem->mThermalElements)
        {
            analyticCsv << ", " << thermalElement->GetGridVertex().GetY();
            simulatedCsv << ", " << thermalElement->GetGridVertex().GetY();
        }
        analyticCsv << "\n";
        simulatedCsv << "\n";

        vector<double> temperatureVector, initialVector, dxdt;
        simulation.mThermalSystem->GetTemperatureVector(temperatureVector);
        simulation.mThermalSystem->GetTemperatureVector(initialVector);
        

        
        double time = 0.000;
        double dt = 100.0;
        boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> > >::type stepper = make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> >() );
        size_t outputCounter = 0;
        while(time < 3500.0)
        {
            while(stepper.try_step(boost::ref(*simulation.mThermalSystem), temperatureVector, time, dt) != boost::numeric::odeint::success)
            {}

            simulation.mThermalSystem->SetTemperatureVector(temperatureVector);
            bool IsOutput = time / 100 > outputCounter;
            
            if(IsOutput)
            {
                analyticCsv << time << ", ";
                simulatedCsv << time << ", ";
                printf("%i,", outputCounter);
            }
            
            for(size_t i = 0; i < simulation.mThermalSystem->GetOdeSystemSize(); ++i)
            {
                const double analytic = 25.0 + (initialVector[i] - 25.0) * exp(- k_t * time);// 25.0 
                    //+ 12.5 * cos(k_x * simulation.mThermalSystem->mThermalElements[i]->GetGridVertex().GetX())  * exp(- k_t * time)
                    //+ 12.5 * cos(k_y * simulation.mThermalSystem->mThermalElements[i]->GetGridVertex().GetY())  * exp(- k_t * time);
                
                double error = fabs(analytic - temperatureVector[i]);
                if(error > maxError)
                    maxError = error;
                
                if(IsOutput)
                {
                    if(i != 0)
                    {
                        analyticCsv << ", ";
                        simulatedCsv << ", ";
                    }
                    analyticCsv << analytic;
                    simulatedCsv << temperatureVector[i];
                }
            }

            if(IsOutput)
            {
                analyticCsv << "\n";
                simulatedCsv << "\n";
                ++outputCounter;
            }
        }
    
        printf("\nTesting of displaced two dimensional conductivity, maximal error %.3f %% of 25K\n\n", maxError / 25.0 * 100.0);
        summaryTxt << "Testing of displaced two dimensional conductivity, maximal error " << maxError / 25.0 * 100.0 <<  " % of 25K" << std::endl;
    }
    
    
    
    {
        std::ofstream analyticCsv("cutcyl_analytic.csv");
        std::ofstream simulatedCsv("cutcyl_simulated.csv");
        double maxError = 0.0;

        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromFile("./SeveralQuadraticCellBlocks.xml");
        thermal::Tolerance<> tolerance(0.000001, geometry::Angle<>::Deg(0.001), 0.1);
        simulation::ThermalSimulation< myMatrixType, double > simulation(parser.GetRoot(), tolerance, 0.0, 0.0, std::numeric_limits<double>::max(), 0, 0);

        double length = 1.0;//Take care, that this corresponds to data in xml-file
        thermal::Material<> material(250.0, 1000.0, 50.0, 50.0, 50.0);//Take care, that this corresponds to data in xml-file
        const double k_x = Angle<>::pi / length;
        const double k_t = k_x * k_x * material.GetConductivity(0) / (material.GetDensity() * material.GetSpecificCapacity());
        analyticCsv << "X";
        simulatedCsv << "X";
        BOOST_FOREACH(shared_ptr<ThermalElement<> > &thermalElement, simulation.mThermalSystem->mThermalElements)
        {
            thermalElement->SetTemperature(25.0 + 25.0 * cos(k_x * thermalElement->GetGridVertex().GetX()));
            analyticCsv << ", " << thermalElement->GetGridVertex().GetX();
            simulatedCsv << ", " << thermalElement->GetGridVertex().GetX();
        }
        analyticCsv << "\n";
        simulatedCsv << "\n";

        vector<double> temperatureVector, initialVector, dxdt;
        simulation.mThermalSystem->GetTemperatureVector(temperatureVector);
        simulation.mThermalSystem->GetTemperatureVector(initialVector);



        double time = 0.000;
        double dt = 100.0;
        boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> > >::type stepper = make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> >() );
        size_t outputCounter = 0;
        while(time < 7400.0)
        {
            while(stepper.try_step(boost::ref(*simulation.mThermalSystem), temperatureVector, time, dt) != boost::numeric::odeint::success)
            {}

            simulation.mThermalSystem->SetTemperatureVector(temperatureVector);
            bool IsOutput = time / 100 > outputCounter;
            
            if(IsOutput)
            {
                analyticCsv << time << ", ";
                simulatedCsv << time << ", ";
                printf("%i,", outputCounter);
            }
            
            for(size_t i = 0; i < simulation.mThermalSystem->GetOdeSystemSize(); ++i)
            {
                const double analytic = 25.0 + (initialVector[i] - 25.0) * exp(- k_t * time);
                
                double error = fabs(analytic - temperatureVector[i]);
                if(error > maxError)
                    maxError = error;
                
                if(IsOutput)
                {
                    if(i != 0)
                    {
                        analyticCsv << ", ";
                        simulatedCsv << ", ";
                    }
                    analyticCsv << analytic;
                    simulatedCsv << temperatureVector[i];
                }
            }

            if(IsOutput)
            {
                analyticCsv << "\n";
                simulatedCsv << "\n";
                ++outputCounter;
            }
        }
    
        printf("\nTesting of cut cylindric conductivity, maximal error %.3f %% of 25K\n\n", maxError / 25.0 * 100.0);
        summaryTxt << "Testing of cut cylindric conductivity, maximal error " << maxError / 25.0 * 100.0 <<  " % of 25K" << std::endl;
    }

    

    {
        std::ofstream analyticCsv("cutcyl_analytic2DWithRectangular.csv");
        std::ofstream simulatedCsv("cutcyl_simulated2DWithRectangular.csv");
        double maxError = 0.0;

        xmlparser::tinyxml2::XmlParserImpl parser;
        boost::scoped_ptr< simulation::ThermalSimulation< myMatrixType, double > > simulation;
        try {
            parser.ReadFromFile("./SeveralQuadraticCellBlocksWithRectangular.xml");
            thermal::Tolerance<> tolerance(0.000001, geometry::Angle<>::Deg(0.001), 0.1);
            simulation.reset( new simulation::ThermalSimulation< myMatrixType, double >(parser.GetRoot(), tolerance, 0.0, 0.0, std::numeric_limits<double>::max(), 0, 0) );
        } catch(exception &e) {
            cout << e.what();
            return EXIT_FAILURE;
        } catch(...) {
            printf("unexpected");
        }

        double length = 1.0;//Take care, that this corresponds to data in xml-file
        thermal::Material<> material(250.0, 1000.0, 50.0, 50.0, 50.0);//Take care, that this corresponds to data in xml-file
        
        
        const double k_x = Angle<>::pi / length;
        const double k_z = Angle<>::pi / length;
        const double k_t = (k_x * k_x + k_z * k_z) * material.GetConductivity(0) / (material.GetDensity() * material.GetSpecificCapacity());
        analyticCsv << "X";
        simulatedCsv << "X";
        BOOST_FOREACH(shared_ptr<ThermalElement<> > &thermalElement, simulation->mThermalSystem->mThermalElements)
        {
            thermalElement->SetTemperature(25.0 + 25.0 * cos(k_x * thermalElement->GetGridVertex().GetX()) * cos(k_z * thermalElement->GetGridVertex().GetZ()));
            analyticCsv << ", " << thermalElement->GetGridVertex().GetX();
            simulatedCsv << ", " << thermalElement->GetGridVertex().GetX();
        }
        analyticCsv << "\n" << "Z";
        simulatedCsv << "\n" << "Z";
                BOOST_FOREACH(shared_ptr<ThermalElement<> > &thermalElement, simulation->mThermalSystem->mThermalElements)
        {
            analyticCsv << ", " << thermalElement->GetGridVertex().GetZ();
            simulatedCsv << ", " << thermalElement->GetGridVertex().GetZ();
        }
        analyticCsv << "\n";
        simulatedCsv << "\n";



        vector<double> temperatureVector, initialVector, dxdt;
        simulation->mThermalSystem->GetTemperatureVector(temperatureVector);
        simulation->mThermalSystem->GetTemperatureVector(initialVector);



        double time = 0.000;
        double dt = 100.0;
        boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> > >::type stepper = make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> >() );
        size_t outputCounter = 0;
        while(time < 7400.0)
        {
            while(stepper.try_step(boost::ref(*simulation->mThermalSystem), temperatureVector, time, dt) != boost::numeric::odeint::success)
            {}

            simulation->mThermalSystem->SetTemperatureVector(temperatureVector);
            bool IsOutput = time / 100 > outputCounter;
            
            if(IsOutput)
            {
                analyticCsv << time << ", ";
                simulatedCsv << time << ", ";
                printf("%i,", outputCounter);
            }
            
            for(size_t i = 0; i < simulation->mThermalSystem->GetOdeSystemSize(); ++i)
            {
                const double analytic = 25.0 + (initialVector[i] - 25.0) * exp(- k_t * time);
                
                double error = fabs(analytic - temperatureVector[i]);
                if(error > maxError)
                    maxError = error;
                
                if(IsOutput)
                {
                    if(i != 0)
                    {
                        analyticCsv << ", ";
                        simulatedCsv << ", ";
                    }
                    analyticCsv << analytic;
                    simulatedCsv << temperatureVector[i];
                }
            }

            if(IsOutput)
            {
                analyticCsv << "\n";
                simulatedCsv << "\n";
                ++outputCounter;
            }
        }
    
        printf("\nTesting of two dimensional cut cylindric conductivity with rectangular blocks, maximal error %.3f %% of 25K\n\n", maxError / 25.0 * 100.0);
        summaryTxt << "Testing of two dimensional cut cylindric conductivity with rectangular blocks, maximal error " << maxError / 25.0 * 100.0 <<  " % of 25K" << std::endl;
    }


    
    {
        std::ofstream analyticCsv("cooling_analytic.csv");
        std::ofstream simulatedCsv("cooling_simulated.csv");
        double maxError = 0.0;

        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromFile("./CoolingLoss.xml");
        thermal::Tolerance<> tolerance(0.000001, geometry::Angle<>::Deg(0.001), 0.1);
        simulation::ThermalSimulation< myMatrixType, double > simulation(parser.GetRoot(), tolerance, 0.0, 0.0, std::numeric_limits<double>::max(), 0, 0);
        
        thermal::Material<> material(250.0, 1000.0, 50.0, 50.0, 50.0);//Take care, that this corresponds to data in xml-file
        const double A = 0.05 * 0.05;//Take care, that this corresponds to data in xml-file
        const double V = 0.05 * 0.05 * 0.05;//Take care, that this corresponds to data in xml-file
        const double deltaX = 0.025;//Take care, that this corresponds to data in xml-file
        const double alpha = 10.0;//Take care, that this corresponds to data in xml-file
        const double k_t = A / (V * material.GetDensity() * material.GetSpecificCapacity()) / (1.0 / alpha + deltaX / material.GetConductivity(0));

        vector<double> temperatureVector, initialVector, dxdt;
        simulation.mThermalSystem->GetTemperatureVector(temperatureVector);
        simulation.mThermalSystem->GetTemperatureVector(initialVector);



        double time = 0.000;
        double dt = 100.0;
        boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> > >::type stepper = make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> >() );
        size_t outputCounter = 0;
        while(time < 10000.0)
        {
            while(stepper.try_step(boost::ref(*simulation.mThermalSystem), temperatureVector, time, dt) != boost::numeric::odeint::success)
            {}

            simulation.mThermalSystem->SetTemperatureVector(temperatureVector);
            bool IsOutput = time / 100 > outputCounter;
            
            if(IsOutput)
            {
                analyticCsv << time << ", ";
                simulatedCsv << time << ", ";
                printf("%i,", outputCounter);
            }
            
            for(size_t i = 0; i < simulation.mThermalSystem->GetOdeSystemSize(); ++i)
            {
                const double analytic = initialVector[i] * exp(- k_t * time);
                
                double error = fabs(analytic - temperatureVector[i]);
                if(error > maxError)
                    maxError = error;
                
                if(IsOutput)
                {
                    if(i != 0)
                    {
                        analyticCsv << ", ";
                        simulatedCsv << ", ";
                    }
                    analyticCsv << analytic;
                    simulatedCsv << temperatureVector[i];
                }
            }

            if(IsOutput)
            {
                analyticCsv << "\n";
                simulatedCsv << "\n";
                ++outputCounter;
            }
        }
    
        printf("\nTesting of cooling through surface area, maximal error %.3f %% of 100K\n\n", maxError / 100 * 100.0);
        summaryTxt << "Testing of cooling through surface area, maximal error " << maxError / 100.0 * 100.0 <<  " % of 100K" << std::endl;
    }


    
    {
        std::ofstream analyticCsv("generation_analytic.csv");
        std::ofstream simulatedCsv("generation_simulated.csv");
        double maxError = 0.0;
        

        const double length = 1.0;
        thermal::Material<> material(250.0, 1000.0, 50.0, 50.0, 50.0);
        shared_ptr< ::state::ThermalState<> > thermalState1( new ::state::ThermalState<> );
        thermalState1->SetFixedPowerDissipation(10000.0 * 0.05 / 2.0);
        shared_ptr< ::state::ThermalState<> > thermalState2( new ::state::ThermalState<> );
        thermalState2->SetFixedPowerDissipation(- 10000.0 * 0.05 / 2.0);

        RectangularBlock<> block1(Cartesian<>(0.0, 0.0, 0.0)
                                , length * 0.5, length, 0.05
                                , 20, 40, 1
                                , &material
                                , 25.0
                                , thermalState1);
        RectangularBlock<> block2(Cartesian<>(length * 0.5, 0.0, 0.0)
                                , length * 0.5, length, 0.05
                                , 20, 40, 1
                                , &material
                                , 25.0
                                , thermalState2);

        Tolerance<> tolerance(0.000001, geometry::Angle<>::Deg(0.001), 0.1);
        ThermalModel<> thermalModel(tolerance);
        thermalModel.ClearAndSetNumberOfBlocksAndCoolings(2,0);


        vector<shared_ptr<ThermalElement<> > > thermalElements;
        vector<vector<IndexedValue<double> > > conductivityMatrix;
        vector<IndexedArea<double> > surfaceElements;
        shared_ptr<BlockGeometry<> > blockGeometry;
        vector<vector<TaylorData<double> > > dummyRadiationConvectionData(thermalElements.size());
        
        block1.CreateData(thermalElements, conductivityMatrix, surfaceElements, blockGeometry);
        thermalModel.AddBlock(thermalElements, conductivityMatrix, surfaceElements, blockGeometry);
        block2.CreateData(thermalElements, conductivityMatrix, surfaceElements, blockGeometry);
        thermalModel.AddBlock(thermalElements, conductivityMatrix, surfaceElements, blockGeometry);
        thermalModel.CreateDataByFusingBlocks(thermalElements, conductivityMatrix, dummyRadiationConvectionData);
        dummyRadiationConvectionData.clear();


        
        const double k_x = Angle<>::pi / length;
        const double k_t = k_x * k_x * material.GetConductivity(0) / (material.GetDensity() * material.GetSpecificCapacity());
        analyticCsv << "X";
        simulatedCsv << "X";
        BOOST_FOREACH(shared_ptr<ThermalElement<> > &thermalElement, thermalElements)
        {
            const double x = thermalElement->GetGridVertex().GetX();
            thermalElement->SetTemperature(25.0 + 25.0 * cos(k_x * x) + MakeStationaryValue(x, length, 50.0, 10000.0));
            analyticCsv << ", " << thermalElement->GetGridVertex().GetX();
            simulatedCsv << ", " << thermalElement->GetGridVertex().GetX();
        }
        analyticCsv << "\n";
        simulatedCsv << "\n";

        OdeSystemThermal<> system(thermalElements
                                , conductivityMatrix
                                , vector<vector<TaylorData<double> > >(thermalElements.size())
                                , vector<shared_ptr<DefaultConvection<> > >(3)
                                , shared_ptr<Radiation<> >()
                                , 25.0
                                , vector<shared_ptr< ::state::ThermalState<double> > > ());
                
        vector<double> temperatureVector, initialVector, dxdt(thermalElements.size());
        system.GetTemperatureVector(temperatureVector);
        system.GetTemperatureVector(initialVector);
        

        
        double time = 0.000;
        double dt = 100.0;
        boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> > >::type stepper = make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector<double> >() );
        size_t outputCounter = 0;
        while(time < 7000.0)
        {
            while(stepper.try_step(boost::ref(system), temperatureVector, time, dt) != boost::numeric::odeint::success)
            {}

            system.SetTemperatureVector(temperatureVector);
            bool IsOutput = time / 100 > outputCounter;
            
            if(IsOutput)
            {
                analyticCsv << time << ", ";
                simulatedCsv << time << ", ";
                printf("%i,", outputCounter);
            }
            
            for(size_t i = 0; i < system.GetOdeSystemSize(); ++i)
            {
                const double x = system.mThermalElements[i]->GetGridVertex().GetX();
                const double analytic = 25.0 + 25.0 * cos(k_x * x) * exp(- k_t * time) + MakeStationaryValue(x, length, 50.0, 10000.0);
                
                double error = fabs(analytic - temperatureVector[i]);
                if(error > maxError)
                    maxError = error;
                
                if(IsOutput)
                {
                    if(i != 0)
                    {
                        analyticCsv << ", ";
                        simulatedCsv << ", ";
                    }
                    analyticCsv << analytic;
                    simulatedCsv << temperatureVector[i];
                }
            }

            if(IsOutput)
            {
                analyticCsv << "\n";
                simulatedCsv << "\n";
                ++outputCounter;
            }
        }

        printf("\nTesting of heat generation scenario, maximal error %.3f %% of 25K\n\n", maxError / 25.0 * 100.0);
        summaryTxt << "Testing of heat generation scenario, maximal error " << maxError / 25.0 * 100.0 <<  " % of 25K" << std::endl;
    }
}
