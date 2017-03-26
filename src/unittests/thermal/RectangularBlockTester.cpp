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
#include "RectangularBlockTester.h"
#include "../../exceptions/error_proto.h"

using namespace geometry;
using namespace state;
using namespace thermal;
static const double sDelta = 0.000001;

/// Used to access list elements by index
template < typename T >
const T &At( const list< T > &listToIndex, size_t index )
{
    if ( index >= listToIndex.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "OutOfBound", index, listToIndex.size() );

    typename list< T >::const_iterator it = listToIndex.begin();
    for ( size_t i = 0; i < index; ++i )
        ++it;

    return *it;
}

/// Used to access list elements by index
template < typename T >
T &At( list< T > &listToIndex, size_t index )
{
    if ( index >= listToIndex.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "OutOfBound", index, listToIndex.size() );

    typename list< T >::iterator it = listToIndex.begin();
    for ( size_t i = 0; i < index; ++i )
        ++it;

    return *it;
}


void RectangularBlockTester::ThermalElementsTest()
{
    TS_ASSERT_EQUALS( mThermalElements.size(), mNX * mNY * mNZ );

    BOOST_FOREACH ( const shared_ptr< ThermalElement<> > &thermalElement, mThermalElements )
    {
        TS_ASSERT_EQUALS( thermalElement->GetTemperature(), mTemperature );
        TS_ASSERT_DELTA( thermalElement->GetPowerDissipation( 2.0, 1.0 ), mPowerDissipation, sDelta );
        TS_ASSERT_DELTA( thermalElement->GetVolume(), mDimX * mDimY * mDimZ, sDelta );
        TS_ASSERT_EQUALS( thermalElement->GetMaterial(), mMaterial );
#ifdef __EXCEPTIONS__
        RectangularElement<> *nullPtr = 0;
        TS_ASSERT_DIFFERS( dynamic_cast< RectangularElement<> * >( thermalElement.get() ), nullPtr )
#endif
    }

    for ( size_t i = 0; i < mNX; ++i )
        for ( size_t j = 0; j < mNY; ++j )
            for ( size_t k = 0; k < mNZ; ++k )
            {
                TS_ASSERT( mThermalElements.at( i + j * mNX + k * mNY * mNX )->GetGridVertex().Equals(
                 Cartesian<>( mDimX * ( i + 0.5 ), mDimY * ( j + 0.5 ), mDimZ * ( k + 0.5 ) ) + mOrigin, sDelta ) )
            }
}

void RectangularBlockTester::SurfaceAreasTest()
{
    TS_ASSERT_EQUALS( mSurfaceAreas.size(), 2.0 * ( mNX * mNY + mNX * mNZ + mNY * mNZ ) );
    int counter = -1;


    for ( size_t i = 0; i < mNX; ++i )
        for ( size_t j = 0; j < mNY; ++j )
        {
            // Top areas
            ++counter;
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mIndex, i + j * mNX + ( mNZ - 1 ) * mNY * mNX );
            TS_ASSERT_DELTA( At( mSurfaceAreas, counter ).mArea.GetDistanceToGridVertex(), 0.5 * mDimZ, sDelta );
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mArea.GetConductivity(), mMaterial->GetConductivity( 2 ) );
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mArea.GetLocation(), TOP );

            vector< Cartesian<> > vertices;
            At( mSurfaceAreas, counter ).mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );

            TS_ASSERT( vertices.at( 0 )
                        .Equals( Cartesian<>( i * mDimX, j * mDimY, mNZ * mDimZ ) + mOrigin + Cartesian<>( 0.0, 0.0, 0.0 ), sDelta ) );
            TS_ASSERT( vertices.at( 1 )
                        .Equals( Cartesian<>( i * mDimX, j * mDimY, mNZ * mDimZ ) + mOrigin + Cartesian<>( mDimX, 0.0, 0.0 ), sDelta ) );
            TS_ASSERT( vertices.at( 2 )
                        .Equals( Cartesian<>( i * mDimX, j * mDimY, mNZ * mDimZ ) + mOrigin + Cartesian<>( mDimX, mDimY, 0.0 ), sDelta ) );
            TS_ASSERT( vertices.at( 3 )
                        .Equals( Cartesian<>( i * mDimX, j * mDimY, mNZ * mDimZ ) + mOrigin + Cartesian<>( 0.0, mDimY, 0.0 ), sDelta ) );


            // Bottom areas
            ++counter;
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mIndex, i + j * mNX + 0.0 );
            TS_ASSERT_DELTA( At( mSurfaceAreas, counter ).mArea.GetDistanceToGridVertex(), 0.5 * mDimZ, sDelta );
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mArea.GetConductivity(), mMaterial->GetConductivity( 2 ) );
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mArea.GetLocation(), BOTTOM );

            At( mSurfaceAreas, counter ).mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );

            TS_ASSERT( vertices.at( 0 )
                        .Equals( Cartesian<>( i * mDimX, j * mDimY, 0.0 ) + mOrigin + Cartesian<>( 0.0, 0.0, 0.0 ), sDelta ) );
            TS_ASSERT( vertices.at( 1 )
                        .Equals( Cartesian<>( i * mDimX, j * mDimY, 0.0 ) + mOrigin + Cartesian<>( 0.0, mDimY, 0.0 ), sDelta ) );
            TS_ASSERT( vertices.at( 2 )
                        .Equals( Cartesian<>( i * mDimX, j * mDimY, 0.0 ) + mOrigin + Cartesian<>( mDimX, mDimY, 0.0 ), sDelta ) );
            TS_ASSERT( vertices.at( 3 )
                        .Equals( Cartesian<>( i * mDimX, j * mDimY, 0.0 ) + mOrigin + Cartesian<>( mDimX, 0.0, 0.0 ), sDelta ) );
        }

    for ( size_t k = 0; k < mNZ; ++k )
    {
        for ( size_t i = 0; i < mNX; ++i )
        {
            // Side areas in X-direction
            ++counter;
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mIndex, i + ( mNY - 1 ) * mNX + k * mNY * mNX );
            TS_ASSERT_DELTA( At( mSurfaceAreas, counter ).mArea.GetDistanceToGridVertex(), 0.5 * mDimY, sDelta );
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mArea.GetConductivity(), mMaterial->GetConductivity( 1 ) );
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mArea.GetLocation(), SIDE );

            vector< Cartesian<> > vertices;
            At( mSurfaceAreas, counter ).mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );

            TS_ASSERT( vertices.at( 0 )
                        .Equals( Cartesian<>( i * mDimX, mNY * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( 0.0, 0.0, 0.0 ), sDelta ) );
            TS_ASSERT( vertices.at( 1 )
                        .Equals( Cartesian<>( i * mDimX, mNY * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( 0.0, 0.0, mDimZ ), sDelta ) );
            TS_ASSERT( vertices.at( 2 )
                        .Equals( Cartesian<>( i * mDimX, mNY * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( mDimX, 0.0, mDimZ ), sDelta ) );
            TS_ASSERT( vertices.at( 3 )
                        .Equals( Cartesian<>( i * mDimX, mNY * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( mDimX, 0.0, 0.0 ), sDelta ) );

            ++counter;
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mIndex, i + 0.0 * mNX + k * mNY * mNX );
            TS_ASSERT_DELTA( At( mSurfaceAreas, counter ).mArea.GetDistanceToGridVertex(), 0.5 * mDimY, sDelta );
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mArea.GetConductivity(), mMaterial->GetConductivity( 1 ) );
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mArea.GetLocation(), SIDE );

            At( mSurfaceAreas, counter ).mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );

            TS_ASSERT( vertices.at( 0 )
                        .Equals( Cartesian<>( i * mDimX, 0.0 * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( 0.0, 0.0, 0.0 ), sDelta ) );
            TS_ASSERT( vertices.at( 1 )
                        .Equals( Cartesian<>( i * mDimX, 0.0 * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( mDimX, 0.0, 0.0 ), sDelta ) );
            TS_ASSERT( vertices.at( 2 )
                        .Equals( Cartesian<>( i * mDimX, 0.0 * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( mDimX, 0.0, mDimZ ), sDelta ) );
            TS_ASSERT( vertices.at( 3 )
                        .Equals( Cartesian<>( i * mDimX, 0.0 * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( 0.0, 0.0, mDimZ ), sDelta ) );
        }


        for ( size_t j = 0; j < mNY; ++j )
        {
            // Side areas in Y-direction
            ++counter;
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mIndex, ( mNX - 1 ) + j * mNX + k * mNY * mNX );
            TS_ASSERT_DELTA( At( mSurfaceAreas, counter ).mArea.GetDistanceToGridVertex(), 0.5 * mDimX, sDelta );
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mArea.GetConductivity(), mMaterial->GetConductivity( 0 ) );
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mArea.GetLocation(), SIDE );

            vector< Cartesian<> > vertices;
            At( mSurfaceAreas, counter ).mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );

            TS_ASSERT( vertices.at( 0 )
                        .Equals( Cartesian<>( mNX * mDimX, j * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( 0.0, 0.0, 0.0 ), sDelta ) );
            TS_ASSERT( vertices.at( 1 )
                        .Equals( Cartesian<>( mNX * mDimX, j * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( 0.0, mDimY, 0.0 ), sDelta ) );
            TS_ASSERT( vertices.at( 2 )
                        .Equals( Cartesian<>( mNX * mDimX, j * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( 0.0, mDimY, mDimZ ), sDelta ) );
            TS_ASSERT( vertices.at( 3 )
                        .Equals( Cartesian<>( mNX * mDimX, j * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( 0.0, 0.0, mDimZ ), sDelta ) );

            ++counter;
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mIndex, 0.0 + j * mNX + k * mNY * mNX );
            TS_ASSERT_DELTA( At( mSurfaceAreas, counter ).mArea.GetDistanceToGridVertex(), 0.5 * mDimX, sDelta );
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mArea.GetConductivity(), mMaterial->GetConductivity( 0 ) );
            TS_ASSERT_EQUALS( At( mSurfaceAreas, counter ).mArea.GetLocation(), SIDE );

            At( mSurfaceAreas, counter ).mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );

            TS_ASSERT( vertices.at( 0 )
                        .Equals( Cartesian<>( 0.0 * mDimX, j * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( 0.0, 0.0, 0.0 ), sDelta ) );
            TS_ASSERT( vertices.at( 1 )
                        .Equals( Cartesian<>( 0.0 * mDimX, j * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( 0.0, 0.0, mDimZ ), sDelta ) );
            TS_ASSERT( vertices.at( 2 )
                        .Equals( Cartesian<>( 0.0 * mDimX, j * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( 0.0, mDimY, mDimZ ), sDelta ) );
            TS_ASSERT( vertices.at( 3 )
                        .Equals( Cartesian<>( 0.0 * mDimX, j * mDimY, k * mDimZ ) + mOrigin + Cartesian<>( 0.0, mDimY, 0.0 ), sDelta ) );
        }
    }
}

bool RectangularBlockTester::IsInsideBlock( const Cartesian<> &vertex )
{
    return vertex.GetX() >= mOrigin.GetX() && vertex.GetX() <= mOrigin.GetX() + mNX * mDimX &&
           vertex.GetY() >= mOrigin.GetY() && vertex.GetY() <= mOrigin.GetY() + mNY * mDimY &&
           vertex.GetZ() >= mOrigin.GetZ() && vertex.GetZ() <= mOrigin.GetZ() + mNZ * mDimZ;
}

void RectangularBlockTester::ConductivityMatrixTest()
{
    if ( mThermalElements.size() == 1 )
    {
        TS_ASSERT_EQUALS( mConductivityMatrix.size(), 1 );
        TS_ASSERT( mConductivityMatrix.at( 0 ).empty() );
        return;
    }


    const Cartesian<> xDistance( mDimX, 0.0, 0.0 );
    const Cartesian<> yDistance( 0.0, mDimY, 0.0 );
    const Cartesian<> zDistance( 0.0, 0.0, mDimZ );
    const double conductivityX = mDimY * mDimZ / mDimX * mMaterial->GetConductivity( 0 );
    const double conductivityY = mDimX * mDimZ / mDimY * mMaterial->GetConductivity( 1 );
    const double conductivityZ = mDimX * mDimY / mDimZ * mMaterial->GetConductivity( 2 );
    vector< unsigned int > connectionCount( mThermalElements.size(), 0 );

    for ( size_t i = 0; i < mThermalElements.size(); ++i )
    {
        const Cartesian<> gridVertex = mThermalElements.at( i )->GetGridVertex();

        if ( IsInsideBlock( gridVertex - xDistance ) )
        {
            bool found[2] = {false};
            for ( size_t j = 0; j < mThermalElements.size(); ++j )
                if ( mThermalElements.at( j )->GetGridVertex().Equals( gridVertex - xDistance, sDelta ) )
                {
                    ++connectionCount.at( i );
                    found[0] = true;
                    BOOST_FOREACH ( const IndexedValue< double > &conductivity, mConductivityMatrix.at( i ) )
                        if ( conductivity.mIndex == j )
                        {
                            TS_ASSERT_DELTA( conductivity.mValue, conductivityX, sDelta );
                            found[1] = true;
                            break;
                        }

                    TS_ASSERT( found[1] );
                }

            TS_ASSERT( found[0] );
        }

        if ( IsInsideBlock( gridVertex + xDistance ) )
        {
            bool found[2] = {false};
            for ( size_t j = 0; j < mThermalElements.size(); ++j )
                if ( mThermalElements.at( j )->GetGridVertex().Equals( gridVertex + xDistance, sDelta ) )
                {
                    ++connectionCount.at( i );
                    found[0] = true;
                    BOOST_FOREACH ( const IndexedValue< double > &conductivity, mConductivityMatrix.at( i ) )
                        if ( conductivity.mIndex == j )
                        {
                            TS_ASSERT_DELTA( conductivity.mValue, conductivityX, sDelta );
                            found[1] = true;
                            break;
                        }

                    TS_ASSERT( found[1] );
                }

            TS_ASSERT( found[0] );
        }

        if ( IsInsideBlock( gridVertex - yDistance ) )
        {
            bool found[2] = {false};
            for ( size_t j = 0; j < mThermalElements.size(); ++j )
                if ( mThermalElements.at( j )->GetGridVertex().Equals( gridVertex - yDistance, sDelta ) )
                {
                    ++connectionCount.at( i );
                    found[0] = true;
                    BOOST_FOREACH ( const IndexedValue< double > &conductivity, mConductivityMatrix.at( i ) )
                        if ( conductivity.mIndex == j )
                        {
                            TS_ASSERT_DELTA( conductivity.mValue, conductivityY, sDelta );
                            found[1] = true;
                            break;
                        }

                    TS_ASSERT( found[1] );
                }

            TS_ASSERT( found[0] );
        }

        if ( IsInsideBlock( gridVertex + yDistance ) )
        {
            bool found[2] = {false};
            for ( size_t j = 0; j < mThermalElements.size(); ++j )
                if ( mThermalElements.at( j )->GetGridVertex().Equals( gridVertex + yDistance, sDelta ) )
                {
                    ++connectionCount.at( i );
                    found[0] = true;
                    BOOST_FOREACH ( const IndexedValue< double > &conductivity, mConductivityMatrix.at( i ) )
                        if ( conductivity.mIndex == j )
                        {
                            TS_ASSERT_DELTA( conductivity.mValue, conductivityY, sDelta );
                            found[1] = true;
                            break;
                        }

                    TS_ASSERT( found[1] );
                }

            TS_ASSERT( found[0] );
        }

        if ( IsInsideBlock( gridVertex - zDistance ) )
        {
            bool found[2] = {false};
            for ( size_t j = 0; j < mThermalElements.size(); ++j )
                if ( mThermalElements.at( j )->GetGridVertex().Equals( gridVertex - zDistance, sDelta ) )
                {
                    ++connectionCount.at( i );
                    found[0] = true;
                    BOOST_FOREACH ( const IndexedValue< double > &conductivity, mConductivityMatrix.at( i ) )
                        if ( conductivity.mIndex == j )
                        {
                            TS_ASSERT_DELTA( conductivity.mValue, conductivityZ, sDelta );
                            found[1] = true;
                            break;
                        }

                    TS_ASSERT( found[1] );
                }

            TS_ASSERT( found[0] );
        }

        if ( IsInsideBlock( gridVertex + zDistance ) )
        {
            bool found[2] = {false};
            for ( size_t j = 0; j < mThermalElements.size(); ++j )
                if ( mThermalElements.at( j )->GetGridVertex().Equals( gridVertex + zDistance, sDelta ) )
                {
                    ++connectionCount.at( i );
                    found[0] = true;
                    BOOST_FOREACH ( const IndexedValue< double > &conductivity, mConductivityMatrix.at( i ) )
                        if ( conductivity.mIndex == j )
                        {
                            TS_ASSERT_DELTA( conductivity.mValue, conductivityZ, sDelta );
                            found[1] = true;
                            break;
                        }

                    TS_ASSERT( found[1] );
                }

            TS_ASSERT( found[0] );
        }


        TS_ASSERT_EQUALS( connectionCount.at( i ), mConductivityMatrix.at( i ).size() - 1 );


        double positive = 0, negative = 0;
        BOOST_FOREACH ( const IndexedValue< double > &conductivity, mConductivityMatrix.at( i ) )
        {
            if ( conductivity.mValue > 0.0 )
                positive += conductivity.mValue;
            else
                negative += -conductivity.mValue;
        }
        TS_ASSERT_DELTA( negative, positive, sDelta );
    }
}

vector< TwoDim<> > RectangularBlockTester::GetBaseVertices()
{
    const TwoDim<> origin2D( mOrigin.GetX(), mOrigin.GetY() );

    vector< TwoDim<> > vertices( 4 );
    vertices.at( 0 ) = origin2D + TwoDim<>( 0.0, 0.0 );
    vertices.at( 1 ) = origin2D + TwoDim<>( mNX * mDimX, 0.0 );
    vertices.at( 2 ) = origin2D + TwoDim<>( mNX * mDimX, mNY * mDimY );
    vertices.at( 3 ) = origin2D + TwoDim<>( 0.0, mNY * mDimY );

    return vertices;
}
