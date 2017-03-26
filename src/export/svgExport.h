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
* File Name : svgExport.h
* Creation Date : 24-03-2015
* Last Modified : Mo 06 Jun 2016 13:45:25 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SVGEXPORT_
#define _SVGEXPORT_

#ifndef __NO_STRING__

// STD
#include <map>

// BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "../electrical/capacity.h"
#include "../electrical/cellelement.h"
#include "../electrical/parallelrc.h"
#include "../electrical/paralleltwoport.h"
#include "../electrical/serialtwoport.h"
#include "../electrical/zarc.h"

#include "../misc/view.h"
#include "../misc/macros.h"

#include "baseExport.h"
#include "svgLibraryInterpreter.h"

enum DescriptionDepth
{
    CELLLEVEL_DEPTH = 0,
    ELECTRICALELEMENTS_DEPTH = 1,
    ALL = 2
};

class TestSvgExport;

/// This export creates a electrical equivalent circuit as an svg. The SVG is going to be put in a grid which has an
/// area of mX_placement *  mY_placement.
/// To be able to display the tree in a structured way, certain information have to be know, such as the deepest and
/// widest branch.
/// From this point on a maximum size of the SVG can be calculated and every single twoport and respectivly the
/// connections can be placed
/// The detail depth of the export can be changed through the second template parameter and can range from all
/// displayable twoports, over all electrical twoports including diffusion elements (warburg tanh, warburg cotanh) or on
/// cellemente level
template < typename matType = myMatrixType, DescriptionDepth descriptionType = CELLLEVEL_DEPTH >
class SvgExport : public BaseExport< matType >
{
    friend class ::TestSvgExport;

    typedef electrical::TwoPort< matType > twoPort_t;

    public:
    /// Constructor
    /// \param rootTwoport the root of the network that should be mapped to a svg
    /// \param stream output all the information to this stream
    /// \param SvgLibraryInterpreter use the following library interpreter for dumping all information of the network
    SvgExport( const twoPort_t *rootTwoport, std::ostream *stream, const std::string &lib = "" );

    /// Destructor
    virtual ~SvgExport(){};

    private:
    /// This function returns true if the electrical network should not be evaluated in depth any further
    /// The function is strongly dependant on the template Parameter DescriptionDepth
    /// \param name of twoport
    bool StopEvaluateDepth( const char *name ) const;

    /// This functions write the several parts of the SVG on the outstream
    virtual void WritePreamble();
    void WriteBody();
    virtual void WriteEnding();

    /// Starts the creation of the SVG Body
    /// \param twoPort_t references the twoport where we start to build our 2D grid
    void LoopOverAllTwoPorts( const twoPort_t *twoPort );

    /// Builds our 2D grid which is the body of the SVG
    /// \param twoPort references the twoport where we start to build our 2D grid
    /// \param rowIndex should be 0 in all cases
    /// \param columnIndex should be 0 in all cases
    void CreateSvgBody( const twoPort_t *twoPort, double &rowIndex, double &columnIndex );

    /// Get the height of a given twoport
    /// \param twoPort
    size_t GetHeightOfTwoPort( const twoPort_t *twoPort ) const;

    /// Get the length of a given twoport
    /// \param twoPort
    size_t GetWidthOfTwoPort( const twoPort_t *twoPort ) const;

    // Member
    std::ostream *mOutputStream;

    size_t mMaxWidth;
    size_t mMaxHeight;

    const double mX_placement = 65;
    const double mY_placement = 65;

    const boost::shared_ptr< svg::SvgLibraryInterpreter > mSvgLibrary;

    std::string mSvgBody;
};

template < typename matType, DescriptionDepth descriptionType >
SvgExport< matType, descriptionType >::SvgExport( const twoPort_t *rootTwoport, std::ostream *stream, const std::string &lib )
    : BaseExport< matType >( rootTwoport )
    , mOutputStream( stream )
    , mMaxWidth( 0 )
    , mMaxHeight( 0 )
    , mSvgLibrary( new svg::SvgLibraryInterpreter( lib ) )
    , mSvgBody( "" )

{
    if ( !mOutputStream )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedStream" );

    if ( !this->mRootPort )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedNetwork" );

    mMaxHeight = GetHeightOfTwoPort( this->mRootPort );
    mMaxWidth = GetWidthOfTwoPort( this->mRootPort );

    LoopOverAllTwoPorts( this->mRootPort );

    WritePreamble();
    WriteBody();
    WriteEnding();
}

template < typename matType, DescriptionDepth descriptionType >
void SvgExport< matType, descriptionType >::WritePreamble()
{
    *mOutputStream << "\
<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?> \n \
<svg \n \
    version=\"1.1\" \n \
    width=\"" << mMaxWidth *mX_placement << "\" \n \
    height=\"" << mMaxHeight *mY_placement << "\" \n \
    id=\"SVG_export ISEA\"> \n";
}

template < typename matType, DescriptionDepth descriptionType >
void SvgExport< matType, descriptionType >::WriteBody()
{
    *mOutputStream << mSvgBody;
}

template < typename matType, DescriptionDepth descriptionType >
void SvgExport< matType, descriptionType >::WriteEnding()
{
    *mOutputStream << "</svg>" << std::endl;
}

template < typename matType, DescriptionDepth descriptionType >
size_t SvgExport< matType, descriptionType >::GetHeightOfTwoPort( const twoPort_t *twoPort ) const
{
    if ( twoPort->CanHaveChildren() )
    {
        const electrical::TwoPortWithChild< matType > *twoPortWithChild(
         static_cast< const electrical::TwoPortWithChild< matType > * >( twoPort ) );

        const size_t inputTwoPortSize = twoPortWithChild->size();

        if ( twoPortWithChild->IsSerialTwoPort() || twoPortWithChild->IsCellelement() )
        {
            size_t currentHeight = 1;

            for ( size_t i = 0; i < inputTwoPortSize; ++i )
            {
                const auto currentChild = twoPortWithChild->at( i );

                if ( !StopEvaluateDepth( currentChild->GetName() ) )
                {
                    const size_t elementHeight = GetHeightOfTwoPort( currentChild );

                    if ( elementHeight > currentHeight )
                        currentHeight = elementHeight;
                }
            }

            return currentHeight;
        }

        else
        {
            size_t currentHeight = inputTwoPortSize;

            for ( size_t i = 0; i < inputTwoPortSize; ++i )
            {
                const auto currentChild = twoPortWithChild->at( i );

                if ( !StopEvaluateDepth( currentChild->GetName() ) )
                    currentHeight += ( GetHeightOfTwoPort( currentChild ) - 1 );
            }

            return currentHeight;
        }
    }

    else
        return 1;
}

template < typename matType, DescriptionDepth descriptionType >
size_t SvgExport< matType, descriptionType >::GetWidthOfTwoPort( const twoPort_t *twoPort ) const
{
    if ( twoPort->CanHaveChildren() )
    {
        const electrical::TwoPortWithChild< matType > *twoPortWithChild(
         static_cast< const electrical::TwoPortWithChild< matType > * >( twoPort ) );

        const size_t inputTwoPortSize = twoPortWithChild->size();

        if ( twoPortWithChild->IsSerialTwoPort() || twoPortWithChild->IsCellelement() )
        {
            size_t currentWidth = inputTwoPortSize;

            for ( size_t i = 0; i < inputTwoPortSize; ++i )
            {
                const auto currentChild = twoPortWithChild->at( i );

                if ( !StopEvaluateDepth( currentChild->GetName() ) )
                    currentWidth += ( GetWidthOfTwoPort( currentChild ) - 1 );
            }

            return currentWidth;
        }

        else
        {
            size_t currentWidth;

            if ( inputTwoPortSize > 1 )
                currentWidth = 3;

            else
                currentWidth = 1;

            for ( size_t i = 0; i < inputTwoPortSize; ++i )
            {
                const auto currentChild = twoPortWithChild->at( i );

                if ( !StopEvaluateDepth( currentChild->GetName() ) )
                {
                    size_t elementWidth = GetWidthOfTwoPort( currentChild );

                    if ( inputTwoPortSize > 1 )
                        elementWidth += 2;

                    if ( elementWidth > currentWidth )
                        currentWidth = elementWidth;
                }
            }

            return currentWidth;
        }
    }

    else
        return 1;
}

template < typename matType, DescriptionDepth descriptionType >
bool SvgExport< matType, descriptionType >::StopEvaluateDepth( const char *name ) const
{
    if ( descriptionType - ELECTRICALELEMENTS_DEPTH == 0 )
    {
        if ( strcmp( name, "WarburgTanh" ) == 0 || strcmp( name, "WarburgCotanh" ) == 0 )
            return true;
    }
    else if ( descriptionType - CELLLEVEL_DEPTH == 0 )
    {
        if ( strcmp( name, "Cellelement" ) == 0 )
            return true;
    }

    return false;
}

template < typename matType, DescriptionDepth descriptionType >
void SvgExport< matType, descriptionType >::LoopOverAllTwoPorts( const twoPort_t *twoPort )
{
    double rowIndex = 0;
    double columnIndex = 0;

    CreateSvgBody( twoPort, rowIndex, columnIndex );
}

template < typename matType, DescriptionDepth descriptionType >
void SvgExport< matType, descriptionType >::CreateSvgBody( const twoPort_t *twoPort, double &rowIndex, double &columnIndex )
{
    const char *inputTwoPortName = twoPort->GetName();
    const double inputTwoPortHeight = static_cast< double >( GetHeightOfTwoPort( twoPort ) );
    const double inputTwoPortWidth = static_cast< double >( GetWidthOfTwoPort( twoPort ) );

    if ( !twoPort )
        return;

    if ( !twoPort->CanHaveChildren() || StopEvaluateDepth( inputTwoPortName ) )
    {
        const std::string tag =
         mSvgLibrary->GetTagWithPosition( inputTwoPortName, rowIndex * mX_placement, columnIndex * mY_placement ) + "\n";
        mSvgBody += tag;

        ++rowIndex;
    }

    else if ( twoPort->CanHaveChildren() )
    {
        const electrical::TwoPortWithChild< matType > *twoPortWithChild(
         static_cast< const electrical::TwoPortWithChild< matType > * >( twoPort ) );

        const size_t inputTwoPortSize = twoPortWithChild->size();

        if ( inputTwoPortSize < 1 )
            return;

        if ( twoPortWithChild->IsSerialTwoPort() || twoPortWithChild->IsCellelement() )
        {
            const double tmpColumn = columnIndex;

            for ( size_t i = 0; i < inputTwoPortSize; ++i )
            {
                const auto currentChild = twoPortWithChild->at( i );

                if ( GetHeightOfTwoPort( currentChild ) < inputTwoPortHeight )
                    columnIndex += ( inputTwoPortHeight - 1 ) / 2.0;

                CreateSvgBody( currentChild, rowIndex, columnIndex );

                columnIndex = tmpColumn;
            }
        }

        else if ( strcmp( inputTwoPortName, "ParallelTwoPort" ) == 0 )
        {
            if ( inputTwoPortSize > 1 )
            {
                std::string serialConnection;
                std::string parallelConnection;

                const double firstChildHeight = double( GetHeightOfTwoPort( twoPortWithChild->at( 0 ) ) );

                // Längsverbindung vor Parallelschaltung
                serialConnection =
                 mSvgLibrary->GetSerialConnection( rowIndex * mX_placement,
                                                   ( columnIndex + ( inputTwoPortHeight - 1 ) / 2.0 ) * mY_placement, 1 ) +
                 "\n";

                mSvgBody += serialConnection;

                ++rowIndex;

                // Querverbindung vor Parallelschaltung
                const double lengthOfParallelConnection =
                 inputTwoPortHeight -
                 0.5 * ( firstChildHeight + double( GetHeightOfTwoPort( twoPortWithChild->at( twoPortWithChild->size() - 1 ) ) ) );

                parallelConnection = mSvgLibrary->GetParallelConnection( rowIndex * mX_placement,
                                                                         ( columnIndex + firstChildHeight / 2 ) * mY_placement,
                                                                         lengthOfParallelConnection );

                mSvgBody += parallelConnection;

                // Loop über Elemente der Parallelschaltung
                const double tmpColumn = columnIndex;
                const double tmpRow = rowIndex;

                for ( size_t i = 0; i < inputTwoPortSize; ++i )
                {
                    const auto currentChild = twoPortWithChild->at( i );
                    const double currentChildHeight = double( GetHeightOfTwoPort( currentChild ) );
                    const double currentChildWidth = double( GetWidthOfTwoPort( currentChild ) );

                    CreateSvgBody( twoPortWithChild->at( i ), rowIndex, columnIndex );

                    if ( rowIndex < tmpRow + inputTwoPortWidth - 2 )
                    {
                        serialConnection =
                         mSvgLibrary->GetSerialConnection( rowIndex * mX_placement,
                                                           ( columnIndex + ( currentChildHeight - 1 ) / 2.0 ) * mY_placement,
                                                           inputTwoPortWidth - 2 - currentChildWidth ) +
                         "\n";

                        mSvgBody += serialConnection;
                    }

                    columnIndex += currentChildHeight;

                    if ( i < inputTwoPortSize - 1 )
                        rowIndex = tmpRow;
                }

                columnIndex = tmpColumn;
                rowIndex = tmpRow + inputTwoPortWidth - 2;

                // Querverbindung nach Parallelschaltung
                parallelConnection = mSvgLibrary->GetParallelConnection( rowIndex * mX_placement,
                                                                         ( tmpColumn + firstChildHeight / 2.0 ) * mY_placement,
                                                                         lengthOfParallelConnection );

                mSvgBody += parallelConnection;

                // Längsverbindung nach Parallelschaltung
                serialConnection =
                 mSvgLibrary->GetSerialConnection( rowIndex * mX_placement,
                                                   ( tmpColumn + ( inputTwoPortHeight - 1 ) / 2.0 ) * mY_placement, 1 ) +
                 "\n";

                mSvgBody += serialConnection;

                ++rowIndex;
            }

            else
                CreateSvgBody( twoPortWithChild->at( 0 ), rowIndex, columnIndex );
        }
    }

    else
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedNetwork" );
    }
}

#endif /* __NO_STRING__ */
#endif /* _SVGEXPORT_ */
