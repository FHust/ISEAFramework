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
#ifndef _CUT_CYL_ELEMENT_
#define _CUT_CYL_ELEMENT_

#include <cmath>
#include "thermal_element.h"
#include "../../../geometry/cylindric.h"
#include "../../../geometry/two_dim.h"
#include "../../../geometry/angle.h"
#include "../../../exceptions/error_proto.h"

// erase later
namespace thermal
{
extern const double globalMaxValue;
extern const double globalMinValue;
}

using namespace std;


namespace thermal
{
using namespace geometry;

/// CutCylElement is a ThermalElement like a CylindricElement, but with the outer front cut by a plane
template < typename T = double >
class CutCylElement : public ThermalElement< T >
{
    public:
    /**
     * Constructor
     *
     * Instead of a Delta rho-coordinate confining the volume in outer rho-direction, a cut line is used.
     * This line determines a vertical plane that must cut the element through the two element faces that are determined
     *by the phi-confinements.
     * @param[in] emplacementPoint Translation vector between global cartesian and cylindric coordinate system used in
     *this instance
     * @param[in] startVector One vertex of the inner edges of the cylindric inner confinement
     * @param[in] deltaPhi Delta phi-coordinate spanning volume from startVector
     * @param[in] deltaZ Delta z-coordinate spanning volume from startVector
     * @param[in] cutLineEmplacement Emplacement point of cut line
     * @param[in] cutLineDirection Direction vector of cut line
     */
    CutCylElement( const Cartesian< T > &emplacementPoint, const Cylindric< T > &startVector, const Angle< T > &deltaPhi,
                   T deltaZ, const TwoDim< T > &cutLineEmplacement, const TwoDim< T > &cutLineDirection, T initialTemperatureValue,
                   const Material< T > *material, ::state::ThermalState< T > *thermalState = 0, T thermalStateFactor = 1.0 );
    virtual ~CutCylElement();
    /// Returns vertex at cut line at (phi==startVector) in local xy-coordinates system with origin at emplacement point
    const TwoDim< T > &GetStraightEdgeVertexStart() const;
    /// Returns vertex at cut line at (phi==startVector.phi+deltaPhi) in local xy-coordinates system with origin at
    /// emplacement point
    const TwoDim< T > &GetStraightEdgeVertexDelta() const;
    T GetZAreaValue() const;        ///<Returns area value of base area
    T GetPhiNegDistance() const;    ///<Return distance from grid vertex to inner phi-front
    T GetPhiPosDistance() const;    ///<Return distance from grid vertex to outer phi-front
    T GetRhoNegDistance() const;    ///<Return distance from grid vertex to inner rho-front
    T GetRhoPosDistance() const;    ///<Return distance from grid vertex to outer rho-front, distance is not taken by a straight line, but by an arc
    T GetRhoNegAveragedDistance() const;    ///<Return distance from grid vertex to inner rho-front averaged over the phi-span of the element
    T GetRhoPosAveragedDistance() const;    ///<Return distance from grid vertex to outer rho-front averaged over the phi-span of the element
    private:
    /// Intersection point of an edge line and line that is going through the origin with slope of tan(phi) in the
    /// xy-plane
    const TwoDim< T > IntersectionRadialEdgeLine( const Angle< T > &phi, const TwoDim< T > &edgeEmplacementPoint,
                                                  const TwoDim< T > &edgeDirectionVector ) const;
    T MakeVolume() const;
    Cartesian< T > MakeGridVertex();
    bool IsCutLineInvalid( const TwoDim< T > &cutLineEmplacement, const TwoDim< T > &cutLineDirection ) const;
    const Cartesian< T > mEmplacementPoint;
    const Cylindric< T > mStartVector;
    const Angle< T > mDeltaPhi;
    const T mDeltaZ;
    const TwoDim< T > mCutLineEmplacement;
    const TwoDim< T > mCutLineDirection;
    const TwoDim< T > mStraightEdgeVertexStart;
    const TwoDim< T > mStraightEdgeVertexDelta;
    TwoDim< T > mTwoDimGridVertex;
};


template < typename T >
CutCylElement< T >::CutCylElement( const Cartesian< T > &emplacementPoint, const Cylindric< T > &startVector,
                                   const Angle< T > &deltaPhi, T deltaZ, const TwoDim< T > &cutLineEmplacement,
                                   const TwoDim< T > &cutLineDirection, T initialTemperatureValue, const Material< T > *material,
                                   ::state::ThermalState< T > *thermalState, T thermalStateFactor )
    : mEmplacementPoint( emplacementPoint )
    , mStartVector( startVector )
    , mDeltaPhi( deltaPhi )
    , mDeltaZ( deltaZ )
    , mCutLineEmplacement( cutLineEmplacement )
    , mCutLineDirection( cutLineDirection )
    , mStraightEdgeVertexStart( IntersectionRadialEdgeLine( startVector.GetPhi(), cutLineEmplacement, cutLineDirection ) )
    , mStraightEdgeVertexDelta( IntersectionRadialEdgeLine( startVector.GetPhi() + deltaPhi, cutLineEmplacement, cutLineDirection ) )
{
    if ( fabs( deltaPhi.GetDeg() ) > 90.0 )    // > 90.0 + 0.001)
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "deltaPhiBigger90deg" );
    if ( startVector.GetRho() < 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "errorRhoGreaterEqZero" );
    if ( cutLineDirection.Get1() == 0.0 && cutLineDirection.Get2() == 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "cutLineDirectionNoDirection" );
    if ( IsCutLineInvalid( cutLineEmplacement, cutLineDirection ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "cutLineCutsElementIncorrectly" );

    ThermalElement< T >::SetElement( initialTemperatureValue, material, MakeGridVertex(), MakeVolume(), thermalState, thermalStateFactor );
}


template < typename T >
CutCylElement< T >::~CutCylElement()
{
}

template < typename T >
const TwoDim< T > &CutCylElement< T >::GetStraightEdgeVertexStart() const
{
    return mStraightEdgeVertexStart;
}

template < typename T >
const TwoDim< T > &CutCylElement< T >::GetStraightEdgeVertexDelta() const
{
    return mStraightEdgeVertexDelta;
}

template < typename T >
T CutCylElement< T >::GetZAreaValue() const
{
    // triangle between emplacement point in cut edge vertices in xy-plane
    const T triangleArea = 0.5 * ( mStraightEdgeVertexStart.Get1() * mStraightEdgeVertexDelta.Get2() -
                                   mStraightEdgeVertexDelta.Get1() * mStraightEdgeVertexStart.Get2() );

    // circle section confined by emplacement point and start vector in xy-plane
    const T circleSectionArea = mStartVector.GetRho() * mStartVector.GetRho() * 0.5 * mDeltaPhi.GetRad();
    return fabs( triangleArea ) - fabs( circleSectionArea );
}


template < typename T >
T CutCylElement< T >::GetPhiNegDistance() const
{
    return mStartVector.GetPhi().AngleTo( mTwoDimGridVertex.AngleInXY() ).GetRad() * mTwoDimGridVertex.Length();
}

template < typename T >
T CutCylElement< T >::GetPhiPosDistance() const
{
    return mTwoDimGridVertex.AngleInXY().AngleTo( mStartVector.GetPhi() + mDeltaPhi ).GetRad() * mTwoDimGridVertex.Length();
}

template < typename T >
T CutCylElement< T >::GetRhoNegDistance() const
{
    return mTwoDimGridVertex.Length() - mStartVector.GetRho();
}

template < typename T >
T CutCylElement< T >::GetRhoPosAveragedDistance() const
{
    const T help1 = ( GetStraightEdgeVertexStart().Length() - mStartVector.GetRho() ) * 0.5;
    const T help2 = ( GetStraightEdgeVertexDelta().Length() - mStartVector.GetRho() ) * 0.5;
    const T help3 = GetRhoPosDistance();
    return pow( help1 * help2 * help3 * help3, 0.25 );
}

template < typename T >
T CutCylElement< T >::GetRhoNegAveragedDistance() const
{
    const T help1 = ( GetStraightEdgeVertexStart().Length() - mStartVector.GetRho() ) * 0.5;
    const T help2 = ( GetStraightEdgeVertexDelta().Length() - mStartVector.GetRho() ) * 0.5;
    const T help3 = GetRhoNegDistance();
    return pow( help1 * help2 * help3 * help3, 0.25 );
}

template < typename T >
T CutCylElement< T >::GetRhoPosDistance() const
{
    const TwoDim< T > helpTwoDim = mTwoDimGridVertex - mCutLineEmplacement;

    const T distance = fabs( ( helpTwoDim.Get1() * mCutLineDirection.Get2() - helpTwoDim.Get2() * mCutLineDirection.Get1() ) /
                             mCutLineDirection.Length() );

    T angleValue = mCutLineDirection.AngleInXY().GetRad() - mTwoDimGridVertex.AngleInXY().GetRad();
    while ( angleValue > Angle< T >::pi / 4.0 )
        angleValue -= Angle< T >::pi / 2.0;
    while ( angleValue < -Angle< T >::pi / 4.0 )
        angleValue += Angle< T >::pi / 2.0;
    angleValue = fabs( angleValue );

    const T sinValue = sin( angleValue );

    if ( sinValue == 0 )
        return distance;
    else
        return distance * angleValue / sinValue;
}

template < typename T >
const TwoDim< T > CutCylElement< T >::IntersectionRadialEdgeLine( const Angle< T > &phi, const TwoDim< T > &edgeEmplacementPoint,
                                                                  const TwoDim< T > &edgeDirectionVector ) const
{
    T v1, v2;

    if ( fabs( fabs( phi.GetDeg() ) - 90.0 ) < 0.000001 )
    {
        v1 = 0;
        v2 = edgeEmplacementPoint.Get2() - edgeEmplacementPoint.Get1() * edgeDirectionVector.Get2() / edgeDirectionVector.Get1();
    }
    else if ( edgeDirectionVector.Get1() == 0.0 || edgeDirectionVector.Get2() / edgeDirectionVector.Get1() > globalMaxValue ||
              edgeDirectionVector.Get2() / edgeDirectionVector.Get1() < globalMinValue )
    {
        v1 = edgeEmplacementPoint.Get1();
        v2 = tan( phi.GetRad() ) * edgeEmplacementPoint.Get1();
    }
    else
    {
        v1 = ( edgeEmplacementPoint.Get2() - edgeEmplacementPoint.Get1() * edgeDirectionVector.Get2() / edgeDirectionVector.Get1() ) /
             ( tan( phi.GetRad() ) - edgeDirectionVector.Get2() / edgeDirectionVector.Get1() );
        v2 = tan( phi.GetRad() ) * v1;
    }

    return TwoDim< T >( v1, v2 );
}

template < typename T >
T CutCylElement< T >::MakeVolume() const
{
    return GetZAreaValue() * fabs( mDeltaZ );
}

template < typename T >
Cartesian< T > CutCylElement< T >::MakeGridVertex()
{
    const T phi = ( mStartVector.GetPhi() + mDeltaPhi / 2.0 ).GetRad();
    const TwoDim< T > innerIntersection( mStartVector.GetRho() * cos( phi ), mStartVector.GetRho() * sin( phi ) );
    mTwoDimGridVertex = ( mStraightEdgeVertexStart + mStraightEdgeVertexDelta + innerIntersection * 2.0 ) / 4.0;

    return Cartesian< T >( mTwoDimGridVertex.Get1(), mTwoDimGridVertex.Get2(), mStartVector.GetZ() + mDeltaZ * 0.5 ) + mEmplacementPoint;
}

template < typename T >
bool CutCylElement< T >::IsCutLineInvalid( const TwoDim< T > &cutLineEmplacement, const TwoDim< T > &cutLineDirection ) const
{
    T m, b;
    if ( fabs( cutLineDirection.Get1() ) > ( cutLineDirection.Get2() ) )
    {
        m = cutLineDirection.Get2() / cutLineDirection.Get1();
        b = cutLineEmplacement.Get2() - m * cutLineEmplacement.Get1();
    }
    else
    {
        m = cutLineDirection.Get1() / cutLineDirection.Get2();
        b = cutLineEmplacement.Get1() - m * cutLineEmplacement.Get2();
    }

    const T r = mStartVector.GetRho();

    if ( r * r * ( m * m + 1 ) > b * b )
        return true;

    T angleDiff = fabs( mStraightEdgeVertexStart.AngleInXY().AngleTo( mStartVector.GetPhi() ).GetDeg() );
    if ( angleDiff > 0.01 && angleDiff < 359.99 )
        return true;

    return false;
}
}
#endif
