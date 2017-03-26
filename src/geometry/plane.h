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
#ifndef _PLANE_
#define _PLANE_


#if defined( _MSC_VER )
#pragma warning( push )
#pragma warning( disable : 4018 )
#endif

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/lu.hpp>

#if defined( _MSC_VER )
#pragma warning( pop )
#endif


#include <cmath>
#include "cartesian.h"
#include "two_dim.h"
#include "unit_vector.h"
#include "tolerance.h"
#include "../exceptions/error_proto.h"


class TestAreaClasses;


namespace geometry
{

/// Plane is formed by an emplacement vector and two direction vectors. At the moment only vertical or horizontal planes
/// are accepted, inclined plane are considered an error in constructor.
template < typename T = double >
class Plane
{
    friend class ::TestAreaClasses;

    typedef boost::numeric::ublas::matrix< T > Matrix;
    typedef boost::numeric::ublas::permutation_matrix< size_t > PermuationMatrix;
    typedef boost::numeric::ublas::vector< T > Vector;
    // enum PlaneType {XY_PLANE, XZ_PLANE, YZ_PLANE, VERTICAL_PLANE, NO_EASY_PLANE};
    enum PlaneType
    {
        HORIZONTAL_EASY_PLANE    ///<Any plane, that has (1,0,0) is first direction vector and (0,1,0) as second direction vector
        ,
        VERTICAL_EASY_PLANE    ///<Any Plane, that has (0,0,1) is second direction vector and whose first direction vector's z-component equals zero
        ,
        NO_EASY_PLANE
    };    ///<Any Plane, that does not fullfil any of the conditions of the previous enum-defines
    protected:
    /// y = m*x + delta, m is a 2X2 matrix, delta is 2X1 vector
    struct Transformation
    {
        T m1_1;
        T m1_2;
        T mDelta1;
        T m2_1;
        T m2_2;
        T mDelta2;
    };

    public:
    Plane( const Cartesian< T > &emplacementVector, const UnitVector< T > &directionVector_1, const UnitVector< T > &directionVector_2 );
    virtual ~Plane();
    bool IsCoplanarTo( const Plane< T > &rhs, const Tolerance< T > &tolerance ) const;
    /// Returns true if normal vector of this instance and normalVector has the same or the opposite direction by
    /// tolerance, otherwise false
    bool IsParallelTo( const UnitVector< T > &normalVector, const Tolerance< T > &tolerance ) const;
    bool IsParallelTo( const Plane< T > &rhs, const Tolerance< T > &tolerance ) const;
    bool IsHorizontal( const Tolerance< T > &tolerance ) const;
    const UnitVector< T > &GetNormalVector() const;
    /// Returns global coordinates of point, with first coordinate of point being interpreted as the coordinate of first
    /// direction vector of Plane, second coordinate respectively
    Cartesian< T > GlobalCoordinates( const TwoDim< T > &point ) const;

    protected:
    /// Returns data to transform coordinates in this instance's coordinate system to that of rhs as a parameter
    Transformation CoordinateTransformationData( const Plane &rhs, const Tolerance< T > &tolerance ) const;

    private:
    /**Solves overdetermined equation with Gaussian normal equation for least squares estimation
     *
     * A*x=b
     * @param[in] A 3X2-matrix
     * @param[in] b 2dim-vector
     * @param[out] x1 Solution of equation
     * @param[out] x2 Solution of equation
     */
    void SolveEquationOverdetermindByOne( const Matrix &A, const Vector &b, T &x1, T &x2 ) const;
    T DistanceToEmplacementVectorOf( const Plane &rhs ) const;
    Cartesian< T > mEmplacementVector;
    UnitVector< T > mDirectionVector_1;
    UnitVector< T > mDirectionVector_2;
    UnitVector< T > mNormalVector;
    PlaneType mPlaneType;
};


template < typename T >
Plane< T >::Plane( const Cartesian< T > &emplacementVector, const UnitVector< T > &directionVector_1, const UnitVector< T > &directionVector_2 )
    : mEmplacementVector( emplacementVector )
    , mDirectionVector_1( directionVector_1 )
    , mDirectionVector_2( directionVector_2 )
    , mNormalVector( directionVector_1.CrossProduct( directionVector_2 ) )
{
    if ( mDirectionVector_1.GetY() == 0.0 && mDirectionVector_1.GetZ() == 0.0 && mDirectionVector_1.GetX() > 0.0 &&
         mDirectionVector_2.GetX() == 0.0 && mDirectionVector_2.GetZ() == 0.0 && mDirectionVector_2.GetY() > 0.0 )
        mPlaneType = HORIZONTAL_EASY_PLANE;
    else if ( mDirectionVector_1.GetZ() == 0.0 && mDirectionVector_2.GetX() == 0.0 &&
              mDirectionVector_2.GetY() == 0.0 && mDirectionVector_2.GetZ() > 0.0 )
        mPlaneType = VERTICAL_EASY_PLANE;
    else
        mPlaneType = NO_EASY_PLANE;


    if ( mPlaneType == NO_EASY_PLANE )
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NotYet" );
        // TODO: Implement and allow evaluation of any Plane ?
    }
}

template < typename T >
Plane< T >::~Plane()
{
}

template < typename T >
Cartesian< T > Plane< T >::GlobalCoordinates( const TwoDim< T > &point ) const
{
    return mEmplacementVector + mDirectionVector_1 * point.Get1() + mDirectionVector_2 * point.Get2();
};

template < typename T >
typename Plane< T >::Transformation Plane< T >::CoordinateTransformationData( const Plane &rhs, const Tolerance< T > &tolerance ) const
{
    Transformation transformation;

    if ( mPlaneType != NO_EASY_PLANE )
    {
        transformation.m1_1 = 1.0;
        transformation.m1_2 = 0.0;
        transformation.m2_1 = 0.0;
        transformation.m2_2 = 1.0;

        if ( mPlaneType == VERTICAL_EASY_PLANE && mDirectionVector_1.IsOppositeDirection( rhs.mDirectionVector_1, tolerance.mAngle ) )
            transformation.m1_1 = -1.0;

        Cartesian< T > deltaEmplacement = rhs.mEmplacementVector - mEmplacementVector;
        switch ( mPlaneType )
        {
            case HORIZONTAL_EASY_PLANE:
                transformation.mDelta1 = deltaEmplacement.GetX();
                transformation.mDelta2 = deltaEmplacement.GetY();
                break;
            case VERTICAL_EASY_PLANE:
                if ( fabs( deltaEmplacement.GetX() ) < tolerance.mLength && fabs( deltaEmplacement.GetY() ) < tolerance.mLength )
                {
                    transformation.mDelta1 = 0;
                }
                else if ( fabs( deltaEmplacement.GetX() ) > fabs( deltaEmplacement.GetY() ) )
                    transformation.mDelta1 = deltaEmplacement.GetX() / mDirectionVector_1.GetX();
                else
                    transformation.mDelta1 = deltaEmplacement.GetY() / mDirectionVector_1.GetY();

                transformation.mDelta2 = deltaEmplacement.GetZ();
                break;
            case NO_EASY_PLANE:
            default:
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NotYet" );
                break;
        }
    }
    else
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NotYet" );
        // TODO: Allow evaluation of any plane, uncomment and unittest the below outcommented code
        // use tolerance to put a boundary on max least squares error of approximation of numerical solving?
        /*Matrix vectorsRhsSystem(3, 2);
        vectorsRhsSystem(0, 0) = rhs.mDirectionVector_1.GetX();
        vectorsRhsSystem(0, 1) = rhs.mDirectionVector_1.GetY();
        vectorsRhsSystem(0, 2) = rhs.mDirectionVector_1.GetZ();
        vectorsRhsSystem(1, 0) = rhs.mDirectionVector_2.GetX();
        vectorsRhsSystem(1, 1) = rhs.mDirectionVector_2.GetY();
        vectorsRhsSystem(1, 2) = rhs.mDirectionVector_2.GetZ();

        Vector vector1(3);
        vector1(0) = mDirectionVector_1.GetX();
        vector1(1) = mDirectionVector_1.GetY();
        vector1(2) = mDirectionVector_1.GetZ();

        Vector vector2(3);
        vector2(0) = mDirectionVector_2.GetX();
        vector2(1) = mDirectionVector_2.GetY();
        vector2(2) = mDirectionVector_2.GetZ();

        SolveEquationOverdetermindByOne(vectorsRhsSystem, vector1, transformation.m1_1, transformation.m1_2);
        SolveEquationOverdetermindByOne(vectorsRhsSystem, vector2, transformation.m2_1, transformation.m2_2);



        Matrix vectorsThisSystem(3, 2);
        vectorsThisSystem(0, 0) = mDirectionVector_1.GetX();
        vectorsThisSystem(0, 1) = mDirectionVector_1.GetY();
        vectorsThisSystem(0, 2) = mDirectionVector_1.GetZ();
        vectorsThisSystem(1, 0) = mDirectionVector_2.GetX();
        vectorsThisSystem(1, 1) = mDirectionVector_2.GetY();
        vectorsThisSystem(1, 2) = mDirectionVector_2.GetZ();

        Cartesian<T> deltaEmplacement = mEmplacementVector- rhs.mEmplacementVector;
        Vector vectorDelta(3);
        vectorDelta(0) = deltaEmplacement.GetX();
        vectorDelta(1) = deltaEmplacement.GetY();
        vectorDelta(2) = deltaEmplacement.GetZ();


        SolveEquationOverdetermindByOne(vectorsThisSystem, vectorDelta, transformation.mDelta1,
        transformation.mDelta2);*/
    }

    return transformation;
}

template < typename T >
void Plane< T >::SolveEquationOverdetermindByOne( const Matrix &A, const Vector &b, T &x1, T &x2 ) const
{
    // A * x ~= b , with Gaussian normal equation for least squares estimation of overdetermined systems
    // -> A^T * A * x = A^T * b (ATA * x = ATb -> x = ATA^-1 * ATb)

    Matrix transA( 2, 3 );
    transA = boost::numeric::ublas::trans( A );

    Matrix ATA( 2, 2 );
    boost::numeric::ublas::axpy_prod( transA, A, ATA, true );

    Vector ATb( 2 );
    boost::numeric::ublas::axpy_prod( transA, b, ATb, true );

    PermuationMatrix permMat( 2 );
    boost::numeric::ublas::lu_factorize( ATA, permMat );
    boost::numeric::ublas::lu_substitute( ATA, permMat, ATb );

    x1 = ATb( 0 );
    x2 = ATb( 1 );
}

template < typename T >
bool Plane< T >::IsParallelTo( const UnitVector< T > &normalVector, const Tolerance< T > &tolerance ) const
{
    return mNormalVector.IsEqualDirection( normalVector, tolerance.mAngle ) ||
           mNormalVector.IsOppositeDirection( normalVector, tolerance.mAngle );
}

template < typename T >
bool Plane< T >::IsParallelTo( const Plane< T > &rhs, const Tolerance< T > &tolerance ) const
{
    return IsParallelTo( rhs.mNormalVector, tolerance );
}

template < typename T >
T Plane< T >::DistanceToEmplacementVectorOf( const Plane< T > &rhs ) const
{
    return fabs( mNormalVector.DotProduct( rhs.mEmplacementVector - mEmplacementVector ) );
}

template < typename T >
bool Plane< T >::IsCoplanarTo( const Plane< T > &rhs, const Tolerance< T > &tolerance ) const
{
    return IsParallelTo( rhs, tolerance ) && DistanceToEmplacementVectorOf( rhs ) < tolerance.mLength;
}

template < typename T >
bool Plane< T >::IsHorizontal( const Tolerance< T > &tolerance ) const
{
    return IsParallelTo( UnitVector< T >( 0.0, 0.0, 1.0 ), tolerance );
}

template < typename T >
const UnitVector< T > &Plane< T >::GetNormalVector() const
{
    return mNormalVector;
}
}
#endif
