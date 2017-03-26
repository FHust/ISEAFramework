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
#include <vector>
#include <list>
#include <ctime>
#include <cstdio>
#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <boost/foreach.hpp>

#include "../src/thermal/jagged_array.h"



using namespace std;

class ThermalBenchmark
{
    struct _16Bytes {
        double m1; double m2;
    };
    struct _64Bytes {
        double m1; double m2; double m3; double m4; 
        double m5; double m6; double m7; double m8;
    };
    struct _128Bytes {
        double m1; double m2; double m3; double m4; 
        double m5; double m6; double m7; double m8;
        double m9; double m10; double m11; double m12; 
        double m13; double m14; double m15; double m16;
    };
    struct NonPod {
        boost::shared_ptr<double> m1; boost::shared_ptr<double> m2;
        boost::shared_ptr<double> m3; boost::shared_ptr<double> m4; 
    };
public:
    ThermalBenchmark();
    void SetLoopFactor(size_t loopFactor);
    void BenchmarkListSmallPod(double &construct, double &iterate, double &destruct);
    void BenchmarkVectorSmallPod(double &construct, double &iterate, double &destruct);
    void BenchmarkListBigPod(double &construct, double &iterate, double &destruct);
    void BenchmarkVectorBigPod(double &construct, double &iterate, double &destruct);   
    void BenchmarkListNonPod(double &construct, double &iterate, double &destruct);
    void BenchmarkVectorNonPod(double &construct, double &iterate, double &destruct);
    void BenchmarkList100double(double &construct, double &iterate, double &destruct);
    void BenchmarkVector100double(double &construct, double &iterate, double &destruct);
    void BenchmarkJaggedArraySmallPod(double &vecVecTime, double &jaggedArrayTime);
    void BenchmarkJaggedArrayMiddlePod(double &vecVecTime, double &jaggedArrayTime);
    void AlibiFunction(const void *vp) { };
private:
    boost::random::uniform_int_distribution<size_t> mTwo;
    boost::random::uniform_int_distribution<size_t> mZeroToThree;
    boost::random::uniform_int_distribution<size_t> mSixToTen;
    boost::random::uniform_real_distribution<double> mValue;
    boost::random::mt19937 mRng;
    size_t mLoopFactor;
};



ThermalBenchmark::ThermalBenchmark()
    : mTwo(0, 2)
    , mZeroToThree(0, 3)
    , mSixToTen(6, 10)
    , mValue(-150.0, 150.0)
    , mLoopFactor(0)
{}

void ThermalBenchmark::SetLoopFactor(size_t loopFactor)
{
    mLoopFactor = loopFactor;
}

void ThermalBenchmark::BenchmarkListSmallPod(double &construct, double &iterate, double &destruct)
{
    clock_t constructingDone;
    clock_t iteratingDone;
    double copyValues[2];
    
    
    
    clock_t start = std::clock();
    
    list<_16Bytes> lst;
    for(size_t i = 0; i < mLoopFactor * 8; ++i)
    {
        _16Bytes elem = { mValue(mRng), mValue(mRng) };
        lst.push_back(elem);
    }
    
    constructingDone = std::clock();
    
    for(size_t i = 0; i < 2; ++i)
        BOOST_FOREACH(const _16Bytes &elem, lst)
        {
                copyValues[0] = elem.m1;
                copyValues[1] = elem.m2;
        }
    
    iteratingDone = std::clock();
    
    lst.clear();
    
    destruct = static_cast<double>(std::clock() - iteratingDone) / CLOCKS_PER_SEC;
    iterate = static_cast<double>(iteratingDone - constructingDone) / CLOCKS_PER_SEC;
    construct = static_cast<double>(constructingDone - start) / CLOCKS_PER_SEC;
    AlibiFunction(copyValues);
}

void ThermalBenchmark::BenchmarkVectorSmallPod(double &construct, double &iterate, double &destruct)
{
    clock_t constructingDone;
    clock_t iteratingDone;
    double copyValues[2];
    
    
    
    clock_t start = std::clock();
    
    vector<_16Bytes> vec;
    for(size_t i = 0; i < mLoopFactor * 8; ++i)
    {
        _16Bytes elem = { mValue(mRng), mValue(mRng) };
        vec.push_back(elem);
    }
    
    constructingDone = std::clock();
    
    for(size_t i = 0; i < 2; ++i)
        BOOST_FOREACH(const _16Bytes &elem, vec)
        {
            copyValues[0] = elem.m1;
            copyValues[1] = elem.m2;
        }
    
    iteratingDone = std::clock();
    
    vec.clear();
    vector<_16Bytes>(vec).swap(vec);
    
    destruct = static_cast<double>(std::clock() - iteratingDone) / CLOCKS_PER_SEC;
    iterate = static_cast<double>(iteratingDone - constructingDone) / CLOCKS_PER_SEC;
    construct = static_cast<double>(constructingDone - start) / CLOCKS_PER_SEC;
    AlibiFunction(copyValues);
}

void ThermalBenchmark::BenchmarkListBigPod(double &construct, double &iterate, double &destruct)
{
    clock_t constructingDone;
    clock_t iteratingDone;
    double copyValues[16];
    
    
    
    clock_t start = std::clock();
    
    list<_128Bytes> lst;
    for(size_t i = 0; i < mLoopFactor; ++i)
    {
        _128Bytes elem = { mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng)
                         , mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng)
                         , mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng) };
        lst.push_back(elem);
    }
    
    constructingDone = std::clock();
    
    for(size_t i = 0; i < 2; ++i)
        BOOST_FOREACH(const _128Bytes &elem, lst)
        {
            copyValues[0] = elem.m1;
            copyValues[1] = elem.m2;
            copyValues[2] = elem.m3;
            copyValues[3] = elem.m4;
            copyValues[4] = elem.m5;
            copyValues[5] = elem.m6;
            copyValues[6] = elem.m7;
            copyValues[7] = elem.m8;
            copyValues[8] = elem.m9;
            copyValues[9] = elem.m10;
            copyValues[10] = elem.m11;
            copyValues[11] = elem.m12;
            copyValues[12] = elem.m13;
            copyValues[13] = elem.m14;
            copyValues[14] = elem.m15;
            copyValues[15] = elem.m16;
        }
    
    iteratingDone = std::clock();
    
    lst.clear();
    
    destruct = static_cast<double>(std::clock() - iteratingDone) / CLOCKS_PER_SEC;
    iterate = static_cast<double>(iteratingDone - constructingDone) / CLOCKS_PER_SEC;
    construct = static_cast<double>(constructingDone - start) / CLOCKS_PER_SEC;
    AlibiFunction(copyValues);
}

void ThermalBenchmark::BenchmarkVectorBigPod(double &construct, double &iterate, double &destruct)
{
    clock_t constructingDone;
    clock_t iteratingDone;
    double copyValues[16];
    
    
    
    clock_t start = std::clock();
    
    vector<_128Bytes> vec;
    for(size_t i = 0; i < mLoopFactor; ++i)
    {
        _128Bytes elem = { mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng)
                         , mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng)
                         , mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng) };
        vec.push_back(elem);
    }
    
    constructingDone = std::clock();
    
    for(size_t i = 0; i < 2; ++i)
        BOOST_FOREACH(const _128Bytes &elem, vec)
        {
            copyValues[0] = elem.m1;
            copyValues[1] = elem.m2;
            copyValues[2] = elem.m3;
            copyValues[3] = elem.m4;
            copyValues[4] = elem.m5;
            copyValues[5] = elem.m6;
            copyValues[6] = elem.m7;
            copyValues[7] = elem.m8;
            copyValues[8] = elem.m9;
            copyValues[9] = elem.m10;
            copyValues[10] = elem.m11;
            copyValues[11] = elem.m12;
            copyValues[12] = elem.m13;
            copyValues[13] = elem.m14;
            copyValues[14] = elem.m15;
            copyValues[15] = elem.m16;
        }
    
    iteratingDone = std::clock();
    
    vec.clear();
    vector<_128Bytes>(vec).swap(vec);
    
    destruct = static_cast<double>(std::clock() - iteratingDone) / CLOCKS_PER_SEC;
    iterate = static_cast<double>(iteratingDone - constructingDone) / CLOCKS_PER_SEC;
    construct = static_cast<double>(constructingDone - start) / CLOCKS_PER_SEC;
    AlibiFunction(copyValues);
}

void ThermalBenchmark::BenchmarkListNonPod(double &construct, double &iterate, double &destruct)
{
    clock_t constructingDone;
    clock_t iteratingDone;
    double copyValues[4];
    
    
    
    clock_t start = std::clock();
    
    list<NonPod> lst;
    for(size_t i = 0; i < mLoopFactor; ++i)
    {
        NonPod elem = { boost::shared_ptr<double>(new double(mValue(mRng))), boost::shared_ptr<double>(new double(mValue(mRng)))
                      , boost::shared_ptr<double>(new double(mValue(mRng))), boost::shared_ptr<double>(new double(mValue(mRng))) };
        lst.push_back(elem);
    }
    
    constructingDone = std::clock();
    
    for(size_t i = 0; i < 2; ++i)
        BOOST_FOREACH(const NonPod &elem, lst)
        {
            copyValues[0] = *elem.m1;
            copyValues[1] = *elem.m2;
            copyValues[2] = *elem.m3;
            copyValues[3] = *elem.m4;
        }
    
    iteratingDone = std::clock();
    
    lst.clear();
    
    destruct = static_cast<double>(std::clock() - iteratingDone) / CLOCKS_PER_SEC;
    iterate = static_cast<double>(iteratingDone - constructingDone) / CLOCKS_PER_SEC;
    construct = static_cast<double>(constructingDone - start) / CLOCKS_PER_SEC;
    AlibiFunction(copyValues);
}

void ThermalBenchmark::BenchmarkVectorNonPod(double &construct, double &iterate, double &destruct)
{
    clock_t constructingDone;
    clock_t iteratingDone;
    double copyValues[4];
    
    
    
    clock_t start = std::clock();
    
    vector<NonPod> vec;
    for(size_t i = 0; i < mLoopFactor; ++i)
    {
        NonPod elem = { boost::shared_ptr<double>(new double(mValue(mRng))), boost::shared_ptr<double>(new double(mValue(mRng)))
                      , boost::shared_ptr<double>(new double(mValue(mRng))), boost::shared_ptr<double>(new double(mValue(mRng))) };
        vec.push_back(elem);
    }
    
    constructingDone = std::clock();
    
    for(size_t i = 0; i < 2; ++i)
        BOOST_FOREACH(const NonPod &elem, vec)
        {
            copyValues[0] = *elem.m1;
            copyValues[1] = *elem.m2;
            copyValues[2] = *elem.m3;
            copyValues[3] = *elem.m4;
        }
    
    iteratingDone = std::clock();
    
    vec.clear();
    vector<NonPod>(vec).swap(vec);
    
    destruct = static_cast<double>(std::clock() - iteratingDone) / CLOCKS_PER_SEC;
    iterate = static_cast<double>(iteratingDone - constructingDone) / CLOCKS_PER_SEC;
    construct = static_cast<double>(constructingDone - start) / CLOCKS_PER_SEC;
    AlibiFunction(copyValues);
}

void ThermalBenchmark::BenchmarkList100double(double &construct, double &iterate, double &destruct)
{
    clock_t constructingDone;
    clock_t iteratingDone;
    double copyValue;
    
    
    
    clock_t start = std::clock();
    
    list<vector<double> > lst;
    for(size_t i = 0; i < mLoopFactor / 1000; ++i)
    {
        lst.push_back(vector<double>());
        lst.back().resize(10000);
        for(size_t j = 0; j < 10000; ++j)
            lst.back().push_back(mValue(mRng));
    }
    
    constructingDone = std::clock();
    
    for(size_t i = 0; i < 2; ++i)
        BOOST_FOREACH(const vector<double> &elemVec, lst)
            BOOST_FOREACH(const double &elem, elemVec)
                copyValue = elem;
    
    iteratingDone = std::clock();
    
    lst.clear();
    
    destruct = static_cast<double>(std::clock() - iteratingDone) / CLOCKS_PER_SEC;
    iterate = static_cast<double>(iteratingDone - constructingDone) / CLOCKS_PER_SEC;
    construct = static_cast<double>(constructingDone - start) / CLOCKS_PER_SEC;
    AlibiFunction(&copyValue);
}

void ThermalBenchmark::BenchmarkVector100double(double &construct, double &iterate, double &destruct)
{
    clock_t constructingDone;
    clock_t iteratingDone;
    double copyValue;
    
    
    
    clock_t start = std::clock();
    
    vector<vector<double> > vec;
    for(size_t i = 0; i < mLoopFactor / 1000; ++i)
    {
        vec.push_back(vector<double>());
        vec.back().resize(10000);
        for(size_t j = 0; j < 10000; ++j)
            vec.back().push_back(mValue(mRng));
    }
    
    constructingDone = std::clock();
    
    for(size_t i = 0; i < 2; ++i)
        BOOST_FOREACH(const vector<double> &elemVec, vec)
            BOOST_FOREACH(const double &elem, elemVec)
                copyValue = elem;
    
    iteratingDone = std::clock();
    
    vec.clear();
    vector<vector<double> >(vec).swap(vec);
    
    destruct = static_cast<double>(std::clock() - iteratingDone) / CLOCKS_PER_SEC;
    iterate = static_cast<double>(iteratingDone - constructingDone) / CLOCKS_PER_SEC;
    construct = static_cast<double>(constructingDone - start) / CLOCKS_PER_SEC;
    AlibiFunction(&copyValue);
}

void ThermalBenchmark::BenchmarkJaggedArraySmallPod(double &vecVecTime, double &jaggedArrayTime)
{
     double copyValues[2];
     vector<vector<_16Bytes> > vecVec(mLoopFactor * 2);
     for(size_t i = 0; i < mLoopFactor * 2; ++i)
     {
         size_t length = mSixToTen(mRng);
         for(size_t j = 0; j < length; ++j)
         {
             _16Bytes elem = { mValue(mRng), mValue(mRng) };
             vecVec[i].push_back(elem);
         }
     }
     thermal::JaggedArray<_16Bytes> jaggedArray(vecVec);
     
     

     clock_t start = std::clock();
     
     for(size_t k = 0; k < 100; ++k)
         BOOST_FOREACH(const vector<_16Bytes> &vec, vecVec)
             BOOST_FOREACH(const _16Bytes &elem, vec)
             {
                 AlibiFunction(&elem);
                 //copyValues[0] = elem.m1;
                 //copyValues[1] = elem.m2;
             }
     
     vecVecTime = static_cast<double>(std::clock() - start) / CLOCKS_PER_SEC;
     start = std::clock();
     
     for(size_t k = 0; k < 100; ++k)
         for(size_t i = 0; i < mLoopFactor; ++i)
         {
             for(_16Bytes *ptr = jaggedArray.Begin(i); ptr != jaggedArray.Begin(i + 1); ++ptr)
             {
                 AlibiFunction(ptr);
                 //copyValues[0] = ptr->m1;
                 //copyValues[1] = ptr->m2;
             }
         }
     
     jaggedArrayTime = static_cast<double>(std::clock() - start) / CLOCKS_PER_SEC;
     AlibiFunction(copyValues);
}

void ThermalBenchmark::BenchmarkJaggedArrayMiddlePod(double &vecVecTime, double &jaggedArrayTime)
{
    double copyValues[8];
    vector<vector<_64Bytes> > vecVec(mLoopFactor * 2);
    for(size_t i = 0; i < mLoopFactor * 2; ++i)
    {
        size_t length = mZeroToThree(mRng);
        for(size_t j = 0; j < length; ++j)
        {
            _64Bytes elem = { mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng)
                            , mValue(mRng), mValue(mRng), mValue(mRng), mValue(mRng) };
            vecVec[i].push_back(elem);
        }
    }
    thermal::JaggedArray<_64Bytes> jaggedArray(vecVec);
    
    

    clock_t start = std::clock();
    
    for(size_t k = 0; k < 100; ++k)
        BOOST_FOREACH(const vector<_64Bytes> &vec, vecVec)
            BOOST_FOREACH(const _64Bytes &elem, vec)
            {
                AlibiFunction(&elem);
                //copyValues[0] = elem.m1;
                //copyValues[1] = elem.m2;
            }
    
    vecVecTime = static_cast<double>(std::clock() - start) / CLOCKS_PER_SEC;
    start = std::clock();
    
    for(size_t k = 0; k < 100; ++k)
        for(size_t i = 0; i < mLoopFactor; ++i)
        {
            for(_64Bytes *ptr = jaggedArray.Begin(i); ptr != jaggedArray.Begin(i + 1); ++ptr)
            {
                AlibiFunction(ptr);
                //copyValues[0] = ptr->m1;
                //copyValues[1] = ptr->m2;
            }
        }
    
    jaggedArrayTime = static_cast<double>(std::clock() - start) / CLOCKS_PER_SEC;
    AlibiFunction(copyValues);
}



int main()
{
    printf("Container benchmarks for thermal part of the simulation\n");
    double vecVecTime, jaggedArrayTime;
    double construct = 0.0, iterate = 0.0, destruct = 0.0;
    ThermalBenchmark benchmark;
    
    
    
    double loopFactor = 1000000.1, total = 0.0;
   /* while(total < 1.5)
    {
        //if(total < 0.033)
        //    loopFactor *= 1.5;
        //else
        //    loopFactor *= 1.5 / total;
        loopFactor *= 1.5;
        
        benchmark.SetLoopFactor(static_cast<size_t>(loopFactor));
        benchmark.BenchmarkListSmallPod(construct, iterate, destruct);
        total = construct + iterate + destruct;
    }
    if(loopFactor > 500000.0)
        loopFactor = 500000.0;*/
    benchmark.SetLoopFactor(static_cast<size_t>(loopFactor));
    printf("Loop Factor is %.0f\n", loopFactor);
    //if(loopFactor < 2000.1)
    //    printf("Loop Factor is too low for valid vector-vector/jagged array benchmark\n");


    
    printf("\n\nBenchmark;          \tconstruction; iterate twice; destruction; total\n");
    
    benchmark.BenchmarkListSmallPod(construct, iterate, destruct);
    printf("List with small pod;\t%3.3f;\t%3.3f;\t%3.3f;\t%3.3f\n", construct, iterate, destruct, construct + iterate + destruct);
    benchmark.BenchmarkVectorSmallPod(construct, iterate, destruct);
    printf("Vector with small pod;\t%3.3f;\t%3.3f;\t%3.3f;\t%3.3f\n", construct, iterate, destruct, construct + iterate + destruct);
    benchmark.BenchmarkListBigPod(construct, iterate, destruct);
    printf("List with big pod;\t%3.3f;\t%3.3f;\t%3.3f;\t%3.3f\n", construct, iterate, destruct, construct + iterate + destruct);
    benchmark.BenchmarkVectorBigPod(construct, iterate, destruct);
    printf("Vector with big pod;\t%3.3f;\t%3.3f;\t%3.3f;\t%3.3f\n", construct, iterate, destruct, construct + iterate + destruct);
    benchmark.BenchmarkListNonPod(construct, iterate, destruct);
    printf("List with non pod;\t%3.3f;\t%3.3f;\t%3.3f;\t%3.3f\n", construct, iterate, destruct, construct + iterate + destruct);
    benchmark.BenchmarkVectorNonPod(construct, iterate, destruct);
    printf("Vector with non pod;\t%3.3f;\t%3.3f;\t%3.3f;\t%3.3f\n", construct, iterate, destruct, construct + iterate + destruct);
    benchmark.BenchmarkList100double(construct, iterate, destruct);
    printf("\nList with 100 double vector;\t%3.3f;\t%3.3f;\t%3.3f;\t%3.3f\n", construct, iterate, destruct, construct + iterate + destruct);
    benchmark.BenchmarkVector100double(construct, iterate, destruct);
    printf("Vector with 100 double vector;\t%3.3f;\t%3.3f;\t%3.3f;\t%3.3f\n", construct, iterate, destruct, construct + iterate + destruct);

    
    
    printf("\n\nBenchmark;                   \titeration time\n");
    
    benchmark.BenchmarkJaggedArraySmallPod(vecVecTime, jaggedArrayTime);
    printf("Vector-vector with small pod;\t%3.3f\n", vecVecTime);
    printf("Jagged array with small pod;\t%3.3f\n", jaggedArrayTime);
    benchmark.BenchmarkJaggedArrayMiddlePod(vecVecTime, jaggedArrayTime);
    printf("Vector-vector with middle pod;\t%3.3f\n", vecVecTime);
    printf("Jagged array with middle pod;\t%3.3f\n", jaggedArrayTime);
    
    
    
    printf("\n\nBenchmark done\n");
}
