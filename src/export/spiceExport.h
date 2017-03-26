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
* File Name : spiceExport.h
* Creation Date : 14-01-2014
* Last Modified : Mi 14 Okt 2015 14:02:54 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SPICEEXPORT_
#define _SPICEEXPORT_

#ifndef __NO_STRING__

#include "../misc/matrixInclude.h"

#include <sstream>
#include <utility>
#include <map>
#include <boost/lexical_cast.hpp>

#include "baseExport.h"

#include "../electrical/capacity.h"
#include "../electrical/cellelement.h"
#include "../electrical/parallelrc.h"
#include "../electrical/paralleltwoport.h"
#include "../electrical/serialtwoport.h"
#include "../electrical/zarc.h"

#include "../exceptions/error_proto.h"

/// This class produces a SpiceNetwork for all the used elements.
/// All values are frozen for one point in time and exported.
/// At this moment the initial condition is not set in the correct way.
/// This must be done manually.
/// The code is terrible and i should feel bad about it, but it was just a quick hack ...
template < typename matType = myMatrixType >
class SpiceExport : public BaseExport< matType >
{
    public:
    typedef std::pair< size_t, size_t > startEndPoint_t;

    SpiceExport( const electrical::TwoPort< matType > *rootTwoport, std::ostream *stream,
                 std::string title = "Simulated Batterie circuit\n", double dt_in_ms = 1.0, double t_start = 0.0,
                 double t_end = 1.0 );

    virtual ~SpiceExport(){};

    /// This function prints the first few lines which are needed for spice
    virtual void WritePreamble();

    /// This function is called for the root port as there is no Start or End point set for this function
    virtual void LoopOverAllTwoPorts( const electrical::TwoPort< matType > *twoPort );

    /// Every twoport besides the rootport calls this function with its parent for startEndPoint_t
    virtual void LoopOverAllTwoPorts( const electrical::TwoPort< matType > *twoPort, startEndPoint_t sep );

    /// This function writes the ending of a spice file
    virtual void WriteEnding();

    private:
    SpiceExport() { abort(); };    //< No default construktor

    const char *GetSpiceName( const char *name ) const;

    std::string GetNameOfTwoPort( const electrical::TwoPort< matType > *twoPort );

    /// This function appends a new size_t. The vector represents a virtual stack call, the traveling path
    /// through the tree
    std::vector< size_t > &GetNewNodeVector( std::vector< size_t > &newNodes, int newNodeCounter );

    int GetNodeCountAndDevideTwoPortWithChild( std::vector< const electrical::TwoPort< matType > * > &withoutChild,
                                               std::vector< const electrical::TwoPortWithChild< matType > * > &withChild,
                                               const electrical::SerialTwoPort< matType > *twoPort );

    /// Function that handles the special case for SerialTwoPorts
    void HandleTwoPort( const electrical::SerialTwoPort< matType > *twoPort, startEndPoint_t sep );

    /// Function that handles the special case for ParallelTwoPort
    void HandleTwoPort( const electrical::ParallelTwoPort< matType > *twoPort, startEndPoint_t sep );

    // This only checks the children of twoport argument, but not the childrens children
    size_t CheckTwoPortThatCountWithoutRecursion( const electrical::SerialTwoPort< matType > *twoPort ) const;

    size_t CountNewNodesForSerialTwoPort( const electrical::SerialTwoPort< matType > *twoPort ) const;

    size_t RunTwoPortWithChildren( std::vector< const electrical::TwoPortWithChild< matType > * > &tpWithoutChildren,
                                   startEndPoint_t sep, std::vector< size_t > &newNodes );

    void RunTwoPortWithoutChildren( std::vector< const electrical::TwoPort< matType > * > &tpWithoutChildren,
                                    startEndPoint_t sep, std::vector< size_t > &newNodes, size_t lastStartPoint );
    bool FindAtLeastOneTwoPortThatCounts( const electrical::TwoPortWithChild< matType > *twoPort ) const;

    size_t CreateNewNode();

    std::map< const electrical::TwoPort< matType > *, std::string > mTwoPort2Name;

    std::ostream *mStream;

    size_t mNodeCount;
    size_t mTwoPortCounter;

    std::string mTitle;
    double mCurrent;

    double mDt_in_ms, mTStart, mTEnd;

    protected:
};

template < typename matType >
SpiceExport< matType >::SpiceExport( const electrical::TwoPort< matType > *rootTwoport, std::ostream *stream,
                                     std::string title, double dt_in_ms, double t_start, double t_end )
    : BaseExport< matType >( rootTwoport )
    , mStream( stream )
    , mNodeCount( 1 )
    , mTwoPortCounter( 1 )
    , mTitle( title )
    , mCurrent( 0.0 )
    , mDt_in_ms( dt_in_ms )
    , mTStart( t_start )
    , mTEnd( t_end )
{
    WritePreamble();

    matType curVec = this->mRootPort->GetCurrent();
    mCurrent = curVec( 0, curVec.n_cols - 1 );
    *mStream << "Iroot 0 1 DC " << mCurrent << "\n";
    //    *mStream << ".IC V(1) " << this->mRootPort->GetVoltageValue() << "V\n"; // <-- Anfangsbedingung für das System
    //    muss jedoch von t-1 der Wert sein und nicht wie hier im code t
    LoopOverAllTwoPorts( this->mRootPort );
    WriteEnding();
}


template < typename matType >
std::vector< size_t > &SpiceExport< matType >::GetNewNodeVector( std::vector< size_t > &newNodes, int newNodeCounter )
{
    if ( static_cast< int >( newNodes.size() ) != newNodeCounter )
        newNodes.resize( newNodeCounter );
    for ( int i = 0; i < newNodeCounter; ++i )
        newNodes[newNodeCounter - i - 1] = CreateNewNode();
    return newNodes;
}

template < typename matType >
size_t SpiceExport< matType >::CreateNewNode()
{
    return ++mNodeCount;
}

/// Seperate TwoPorts in two groups
/// With Children
/// Without Children
template < typename matType >
int SpiceExport< matType >::GetNodeCountAndDevideTwoPortWithChild( std::vector< const electrical::TwoPort< matType > * > &tpWithoutChildren,
                                                                   std::vector< const electrical::TwoPortWithChild< matType > * > &tpWithChildren,
                                                                   const electrical::SerialTwoPort< matType > *twoPort )
{

    int newNodeCounter = CheckTwoPortThatCountWithoutRecursion( twoPort );

    for ( size_t i = 0; i < twoPort->size(); ++i )
    {
        if ( twoPort->at( i )->CanHaveChildren() )
        {
            const electrical::TwoPortWithChild< matType > *twoPortWithchild =
             static_cast< const electrical::TwoPortWithChild< matType > * >( twoPort->at( i ) );
            tpWithChildren.push_back( twoPortWithchild );
            newNodeCounter += FindAtLeastOneTwoPortThatCounts( twoPortWithchild );
        }
        else
            tpWithoutChildren.push_back( twoPort->at( i ) );
    }
    newNodeCounter -= 1;
    return newNodeCounter;
}

template < typename matType >
void SpiceExport< matType >::LoopOverAllTwoPorts( const electrical::TwoPort< matType > *twoPort, startEndPoint_t sep )
{

    const char *name = twoPort->GetName();
    if ( !twoPort->CanHaveChildren() )
    {
        if ( strcmp( name, "OhmicResistance" ) == 0 || strcmp( name, "VoltageSource" ) == 0 )
        {
            const electrical::ElectricalElement< matType > *eb =
             static_cast< const electrical::ElectricalElement< matType > * >( twoPort );
            *mStream << GetNameOfTwoPort( twoPort ) << " " << sep.first << " " << sep.second << " " << eb->GetValue() << "\n";
        }
        else if ( strcmp( name, "Capacity" ) == 0 )
        {
            const electrical::ElectricalElement< matType > *eb =
             static_cast< const electrical::ElectricalElement< matType > * >( twoPort );
            *mStream << GetNameOfTwoPort( twoPort ) << " " << sep.first << " " << sep.second << " " << eb->GetValue()
                     << " IC=" << twoPort->GetVoltageValue() << "V\n";
        }
        else if ( strcmp( name, "ParallelRC" ) == 0 )
        {
            const electrical::ParallelRC< matType > *eb = static_cast< const electrical::ParallelRC< matType > * >( twoPort );
            *mStream << "R" << GetNameOfTwoPort( twoPort ) << " " << sep.first << " " << sep.second << " "
                     << eb->GetValue() << "\n";
            *mStream << "C" << GetNameOfTwoPort( twoPort ) << " " << sep.first << " " << sep.second << " "
                     << eb->GetValueC() << "\n";
        }

        else if ( strcmp( name, "Zarc" ) == 0 )
        {

            const electrical::Zarc< matType > *eb = static_cast< const electrical::Zarc< matType > * >( twoPort );

            size_t newNodeCounter =
             eb->GetNumberOfElements();    // There are three elements in one zarc, therefore we need two nodes

            std::vector< size_t > newNodes( newNodeCounter );
            GetNewNodeVector( newNodes, newNodeCounter );

            const std::vector< double > &rValues( eb->GetRValues() );
            const std::vector< double > &cValues( eb->GetCValues() );
            const std::vector< double > &initValues( eb->GetVoltageValues() );

            // Iterate over ohmic Resistance

            size_t numberOfCaps = cValues.size();
            size_t lastStartPoint = sep.first;


            if ( numberOfCaps > 0 )
                *mStream << "C" << GetNameOfTwoPort( twoPort ) << "_" << 0 << " " << lastStartPoint << " "
                         << newNodes.back() << " " << cValues.at( 0 ) << " IC=" << initValues.at( 0 ) << "V\n";
            else
            {
                *mStream << "R" << GetNameOfTwoPort( twoPort ) << "_" << 0 << " " << lastStartPoint << " " << sep.second
                         << " " << rValues.at( 0 ) << "\n";
                return;
            }

            *mStream << "R" << GetNameOfTwoPort( twoPort ) << "_" << 0 << " " << lastStartPoint << " "
                     << newNodes.back() << " " << rValues.at( 0 ) << "\n";
            lastStartPoint = newNodes.back();
            newNodes.pop_back();

            if ( numberOfCaps > 1 )
                *mStream << "C" << GetNameOfTwoPort( twoPort ) << "_" << 1 << " " << lastStartPoint << " "
                         << newNodes.back() << " " << cValues.at( 1 ) << " IC=" << initValues.at( 1 ) << "V\n";
            else
            {
                *mStream << "R" << GetNameOfTwoPort( twoPort ) << "_" << 1 << " " << lastStartPoint << " " << sep.second
                         << " " << rValues.at( 1 ) << "\n";
                return;
            }

            *mStream << "R" << GetNameOfTwoPort( twoPort ) << "_" << 1 << " " << lastStartPoint << " "
                     << newNodes.back() << " " << rValues.at( 1 ) << "\n";
            lastStartPoint = newNodes.back();

            if ( numberOfCaps > 2 )
                *mStream << "C" << GetNameOfTwoPort( twoPort ) << "_" << 2 << " " << lastStartPoint << " " << sep.second
                         << " " << cValues.at( 2 ) << " IC=" << initValues.at( 2 ) << "V\n";
            else
            {
                *mStream << "R" << GetNameOfTwoPort( twoPort ) << "_" << 2 << " " << lastStartPoint << " " << sep.second
                         << " " << rValues.at( 2 ) << "\n";
                return;
            }

            *mStream << "R" << GetNameOfTwoPort( twoPort ) << "_" << 2 << " " << lastStartPoint << " " << sep.second
                     << " " << rValues.at( 2 ) << "\n";
        }
        else
        {
            *mStream << name << std::endl;
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnknownTwoPort", name );
        }
        return;
    }
    else
    {
        const electrical::TwoPortWithChild< matType > *twoPortWithChild =
         static_cast< const electrical::TwoPortWithChild< matType > * >( twoPort );
        if ( FindAtLeastOneTwoPortThatCounts( twoPortWithChild ) )
        {
            if ( twoPortWithChild->IsSerialTwoPort() )
                HandleTwoPort( static_cast< const electrical::SerialTwoPort< matType > * >( twoPort ), sep );
            else
                HandleTwoPort( static_cast< const electrical::ParallelTwoPort< matType > * >( twoPort ), sep );
        }
    }
}

template < typename matType >
void SpiceExport< matType >::HandleTwoPort( const electrical::SerialTwoPort< matType > *twoPort, startEndPoint_t sep )
{
    // Alle Kinder durchlaufen, die keine Kinder haben können

    std::vector< const electrical::TwoPort< matType > * > tpWithoutChildren;
    std::vector< const electrical::TwoPortWithChild< matType > * > tpWithChildren;

    int newNodeCounter = GetNodeCountAndDevideTwoPortWithChild( tpWithoutChildren, tpWithChildren, twoPort );

    if ( newNodeCounter <= 0 )
    {
        for ( size_t i = 0; i < twoPort->size(); ++i )
            LoopOverAllTwoPorts( twoPort->at( i ), sep );
        return;
    }


    std::vector< size_t > newNodes( newNodeCounter );
    GetNewNodeVector( newNodes, newNodeCounter );

    size_t lastStartPoint = RunTwoPortWithChildren( tpWithChildren, sep, newNodes );
    RunTwoPortWithoutChildren( tpWithoutChildren, sep, newNodes, lastStartPoint );
}

template < typename matType >
void SpiceExport< matType >::HandleTwoPort( const electrical::ParallelTwoPort< matType > *twoPort, startEndPoint_t sep )
{
    for ( size_t i = 0; i < twoPort->size(); ++i )
        LoopOverAllTwoPorts( twoPort->at( i ), sep );
}

template < typename matType >
void SpiceExport< matType >::LoopOverAllTwoPorts( const electrical::TwoPort< matType > *twoPort )
{
    startEndPoint_t rootStarPoint( 1, 0 );
    LoopOverAllTwoPorts( twoPort, rootStarPoint );
}

template < typename matType >
size_t SpiceExport< matType >::CountNewNodesForSerialTwoPort( const electrical::SerialTwoPort< matType > *twoPort ) const
{
    int countOfNewNodes = CheckTwoPortThatCountWithoutRecursion( twoPort );
    for ( size_t i = 0; i < twoPort->size(); ++i )
    {
        if ( twoPort->at( i )->CanHaveChildren() )
        {
            countOfNewNodes += FindAtLeastOneTwoPortThatCounts( twoPort );
        }
    }
    --countOfNewNodes;
    if ( countOfNewNodes < 0 )
        countOfNewNodes = 0;
    return countOfNewNodes;
}

template < typename matType >
size_t SpiceExport< matType >::CheckTwoPortThatCountWithoutRecursion( const electrical::SerialTwoPort< matType > *twoPort ) const
{
    size_t returnvalue = 0;

    for ( size_t i = 0; i < twoPort->size(); ++i )
    {
        if ( !twoPort->at( i )->CanHaveChildren() )
            ++returnvalue;
    }
    return returnvalue;
}

template < typename matType >
bool SpiceExport< matType >::FindAtLeastOneTwoPortThatCounts( const electrical::TwoPortWithChild< matType > *twoPort ) const
{
    for ( size_t i = 0; i < twoPort->size(); ++i )
    {
        if ( !twoPort->at( i )->CanHaveChildren() )
            return true;
        else
        {
            const electrical::TwoPortWithChild< matType > *child =
             static_cast< const electrical::TwoPortWithChild< matType > * >( twoPort->at( i ) );
            return FindAtLeastOneTwoPortThatCounts( child );
        }
    }
    return false;
}

template < typename matType >
size_t SpiceExport< matType >::RunTwoPortWithChildren( std::vector< const electrical::TwoPortWithChild< matType > * > &tpWithChildren,
                                                       startEndPoint_t sep, std::vector< size_t > &newNodes )
{
    size_t lastStartPoint = sep.first;

    for ( size_t i = 0; i < tpWithChildren.size(); ++i )
    {
        if ( FindAtLeastOneTwoPortThatCounts( tpWithChildren.at( i ) ) )
        {
            startEndPoint_t newStartEndPoint;

            if ( newNodes.size() != 0 )
            {
                newStartEndPoint.second = newNodes.back();
                newNodes.pop_back();
            }
            else
                newStartEndPoint.second = sep.second;

            newStartEndPoint.first = lastStartPoint;

            LoopOverAllTwoPorts( tpWithChildren.at( i ), newStartEndPoint );
            lastStartPoint = newStartEndPoint.second;
        }
    }
    return lastStartPoint;
}

template < typename matType >
void SpiceExport< matType >::RunTwoPortWithoutChildren( std::vector< const electrical::TwoPort< matType > * > &tpWithoutChildren,
                                                        startEndPoint_t sep, std::vector< size_t > &newNodes, size_t lastStartPoint )
{
    for ( size_t i = 0; i < tpWithoutChildren.size(); ++i )
    {
        startEndPoint_t newStartEndPoint;

        if ( newNodes.size() != 0 )
        {
            newStartEndPoint.second = newNodes.back();
            newNodes.pop_back();
        }
        else
            newStartEndPoint.second = sep.second;

        newStartEndPoint.first = lastStartPoint;

        LoopOverAllTwoPorts( tpWithoutChildren.at( i ), newStartEndPoint );
        lastStartPoint = newStartEndPoint.second;
    }
}

template < typename matType >
const char *SpiceExport< matType >::GetSpiceName( const char *name ) const
{
    if ( strcmp( name, "OhmicResistance" ) == 0 )
        return "R";
    else if ( strcmp( name, "Capacity" ) == 0 )
        return "C";
    else if ( strcmp( name, "VoltageSource" ) == 0 )
        return "V";
    else if ( strcmp( name, "ParallelRC" ) == 0 )
        return "";
    else if ( strcmp( name, "Cellelement" ) == 0 )
        return "Bat";
    return "";
}

template < typename matType >
void SpiceExport< matType >::WritePreamble()
{
    *mStream << mTitle;
}

template < typename matType >
void SpiceExport< matType >::WriteEnding()
{
    *mStream << ".TRAN " << mDt_in_ms << "ms " << mTEnd << " " << mTStart
             << " UIC \n"    // .TRAN <Tstep> <Tstop> [Tstart [dTmax]] [modifiers]
             << ".END \n";
}

template < typename matType >
std::string SpiceExport< matType >::GetNameOfTwoPort( const electrical::TwoPort< matType > *twoPort )
{

    std::string returnstring;

    typedef typename std::map< const electrical::TwoPort< matType > *, std::string >::iterator iterator;
    iterator it = mTwoPort2Name.find( twoPort );

    if ( it != mTwoPort2Name.end() )
        returnstring = it->second;
    else
    {
        returnstring = GetSpiceName( twoPort->GetName() ) + boost::lexical_cast< std::string >( mTwoPortCounter++ );
        mTwoPort2Name[twoPort] = returnstring;
    }

    return returnstring;
}


#endif /* _SPICEEXPORT_ */
#endif /* __NO_STRING__ */
