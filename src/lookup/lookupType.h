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
* File Name : lookupType.h
* Creation Date : 24-05-2012
* Last Modified : Di 11 Feb 2014 14:19:00 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _LOOKUPTYPE_
#define _LOOKUPTYPE_

// STD
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>

class TestLookup;

/// This namespace is concerned with lookups 1D and 2D
namespace lookup
{
enum LookupType
{
    LINEAR_INTERPOLATION = 0,
    NEAREST_NEIGHBOUR = 1,
    CONSTANT_UNTIL_THRESHOLD = 2,
    SPLINE_INTERPOLATION = 3,
    NUMBER_OF_LOOKUPTYPES = 4
};

/// This class contains measurement point and data to determine the lookup
template < typename T >
class LookupType1D
{
    public:
    LookupType1D( const std::vector< T > &lookupData, const std::vector< T > &measurementPoints )
        : mLookup( lookupData )
        , mMeasurementPoints( measurementPoints )
        , mCurrentMeasurementPointIndex( 0 )
        , mMinValue( measurementPoints.front() )
        , mMaxValue( measurementPoints.back() ){};
    const std::vector< T > &GetLookup() const { return mLookup; };

    virtual ~LookupType1D(){};
    virtual T DoLookup( T lookupPoint ) = 0;
    virtual T GetMinValueOfLookup() const { return *std::min_element( mLookup.begin(), mLookup.end() ); }
    virtual T GetMaxValueOfLookup() const { return *std::max_element( mLookup.begin(), mLookup.end() ); }
    inline const std::vector< T > &GetPoints() const { return mMeasurementPoints; }

    protected:
    size_t GetLowerBound( T value )
    {
        while ( ( mCurrentMeasurementPointIndex < mMeasurementPoints.size() - 1 ) &&
                ( value > mMeasurementPoints[mCurrentMeasurementPointIndex + 1] ) )
            ++mCurrentMeasurementPointIndex;

        while ( ( mCurrentMeasurementPointIndex > 0 ) && ( value < mMeasurementPoints[mCurrentMeasurementPointIndex] ) )
            --mCurrentMeasurementPointIndex;

        return mCurrentMeasurementPointIndex;
    }
    std::vector< T > mLookup;
    std::vector< T > mMeasurementPoints;
    size_t mCurrentMeasurementPointIndex;
    T mMinValue;
    T mMaxValue;
};

/// This class contains the nearest neighbour behaviour for 1D lookups
template < typename T >
class NearestNeighbour1D : public LookupType1D< T >
{
    public:
    NearestNeighbour1D( const typename std::vector< T > &lookupData, const typename std::vector< T > &measurementPoints )
        : LookupType1D< T >( lookupData, measurementPoints ){};
    virtual ~NearestNeighbour1D(){};
    virtual T DoLookup( T lookupPoint )
    {
        // Boundary
        if ( lookupPoint <= this->mMinValue )
            return this->mLookup.front();
        else if ( lookupPoint >= this->mMaxValue )
            return this->mLookup.back();

        const size_t i = LookupType1D< T >::GetLowerBound( lookupPoint );
        if ( std::abs( this->mMeasurementPoints.at( i ) - lookupPoint ) < std::abs( this->mMeasurementPoints.at( i + 1 ) - lookupPoint ) )
            return this->mLookup.at( i );
        else
            return this->mLookup.at( i + 1 );
    };
};

/// This class contains an linear interpolation behaviour for 1D lookups
template < typename T >
class Linear1DInterpolation : public LookupType1D< T >
{
    public:
    Linear1DInterpolation( const std::vector< T > &lookupData, const std::vector< T > &measurementPoints )
        : LookupType1D< T >( lookupData, measurementPoints ){};
    virtual ~Linear1DInterpolation(){};
    virtual T DoLookup( T lookupPoint )
    {
        // Boundary
        if ( lookupPoint <= this->mMinValue )
            return this->mLookup.front();
        else if ( lookupPoint >= this->mMaxValue )
            return this->mLookup.back();

        const size_t i = LookupType1D< T >::GetLowerBound( lookupPoint );
        return this->mLookup.at( i ) +
               ( lookupPoint - this->mMeasurementPoints.at( i ) ) * ( this->mLookup.at( i + 1 ) - this->mLookup.at( i ) ) /
                ( this->mMeasurementPoints.at( i + 1 ) - this->mMeasurementPoints.at( i ) );
    };

    private:
    protected:
};

/// This class contains constant until threshold behaviour for 1D lookups
template < typename T >
class ConstantUntilThreshold1D : public LookupType1D< T >
{
    public:
    ConstantUntilThreshold1D( const typename std::vector< T > &lookupData, const typename std::vector< T > &measurementPoints )
        : LookupType1D< T >( lookupData, measurementPoints ){};
    virtual ~ConstantUntilThreshold1D(){};
    virtual T DoLookup( T lookupPoint )
    {
        // Boundary
        if ( lookupPoint <= this->mMinValue )
            return this->mLookup.front();
        else if ( lookupPoint >= this->mMaxValue )
            return this->mLookup.back();

        // Get index of lower bound
        const size_t i = LookupType1D< T >::GetLowerBound( static_cast< long >(
         lookupPoint + 1 ) );    // cast is used to get only integers, e.g. lookupPoint = 9.9 -> lookupPoint 9.0

        // Return lookup data at this very index
        return this->mLookup[i];
    };
};

/** This class contains spline interpolation for 1D lookups.
Algorithm is based on
en.wikipedia.org/w/index.php?title=Spline_%28mathematics%29&oldid=288288033#Algorithm_for_computing_natural_cubic_splines
*/
template < typename T >
class SplineInterpolation1D : public LookupType1D< T >
{

    // friend class
    friend class ::TestLookup;

    public:
    SplineInterpolation1D( const typename std::vector< T > &lookupData, const typename std::vector< T > &measurementPoints )
        : LookupType1D< T >( lookupData, measurementPoints )

        // Coefficients
        , mCoefficientB( measurementPoints.size() - 1, 0 )
        , mCoefficientC( measurementPoints.size(), 0 )
        , mCoefficientD( measurementPoints.size() - 1, 0 )

        // Auxiliary variables
        , mAuxiliaryAlpha( measurementPoints.size() - 1, 0 )    // alpha in Wiki
        , mAuxiliaryBeta( measurementPoints.size() - 1, 0 )     // h in Wiki
        , mAuxiliaryGamma( measurementPoints.size(), 1 )        // l in Wiki
        , mAuxiliaryDelta( measurementPoints.size(), 0 )        // my in Wiki
        , mAuxiliaryEpsilon( measurementPoints.size(), 0 )      // z in Wiki

    {
        // Constructor functions
        GetAuxiliaryVariables();
        GetCoefficients();
    };

    virtual ~SplineInterpolation1D(){// Destructor functions: empty
    };

    /// Get auxiliary variables alpha, beta, gamma, delta, epsilon
    void GetAuxiliaryVariables()
    {
        for ( size_t i = 0; i < this->mMeasurementPoints.size() - 1; ++i )
        {
            this->mAuxiliaryBeta.at( i ) = this->mMeasurementPoints.at( i + 1 ) - this->mMeasurementPoints.at( i );    // Guaranteed not zero because mMeasurementPoints.at(i+1) > mMeasurementPoints.at(i)
        }

        for ( size_t i = 1; i < this->mMeasurementPoints.size() - 1; ++i )
        {
            this->mAuxiliaryAlpha.at( i ) =
             3 / this->mAuxiliaryBeta.at( i ) * ( this->mLookup.at( i + 1 ) - this->mLookup.at( i ) ) -
             3 / this->mAuxiliaryBeta.at( i - 1 ) * ( this->mLookup.at( i ) - this->mLookup.at( i - 1 ) );

            this->mAuxiliaryGamma.at( i ) = 2 * ( this->mMeasurementPoints.at( i + 1 ) - this->mMeasurementPoints.at( i - 1 ) ) -
                                            this->mAuxiliaryBeta.at( i - 1 ) * this->mAuxiliaryDelta.at( i - 1 );    // Guaranteed not zero because mMeasurementPoints.at(i+1) > mMeasurementPoints.at(i)

            this->mAuxiliaryDelta.at( i ) = this->mAuxiliaryBeta.at( i ) / this->mAuxiliaryGamma.at( i );

            this->mAuxiliaryEpsilon.at( i ) =
             ( this->mAuxiliaryAlpha.at( i ) - this->mAuxiliaryBeta.at( i - 1 ) * this->mAuxiliaryEpsilon.at( i - 1 ) ) /
             this->mAuxiliaryGamma.at( i );
        }
    }

    /// Get coefficients b, c, d. a is not needed, since a = lookupData
    void GetCoefficients()
    {
        for ( int i = this->mMeasurementPoints.size() - 2; i >= 0;
              --i )    // Cannot use size_t here because i will be < 0. Using int instead
        {
            this->mCoefficientC.at( i ) =
             this->mAuxiliaryEpsilon.at( i ) - this->mAuxiliaryDelta.at( i ) * this->mCoefficientC.at( i + 1 );

            this->mCoefficientB.at( i ) =
             ( this->mLookup.at( i + 1 ) - this->mLookup.at( i ) ) / this->mAuxiliaryBeta.at( i ) -
             this->mAuxiliaryBeta.at( i ) * ( this->mCoefficientC.at( i + 1 ) + 2 * this->mCoefficientC.at( i ) ) / 3;

            this->mCoefficientD.at( i ) =
             ( this->mCoefficientC.at( i + 1 ) - this->mCoefficientC.at( i ) ) / ( 3 * mAuxiliaryBeta.at( i ) );
        }
    }

    /// Lookup function SplineInterpolation1D
    virtual T DoLookup( T lookupPoint )
    {
        // Boundary
        if ( lookupPoint <= this->mMinValue )
            return this->mLookup.front();
        else if ( lookupPoint >= this->mMaxValue )
            return this->mLookup.back();

        // Get index of lower bound
        const size_t i = LookupType1D< T >::GetLowerBound( lookupPoint );

        // Calculate value at lookup point
        return this->mLookup.at( i ) + mCoefficientB.at( i ) * ( lookupPoint - this->mMeasurementPoints[i] ) +
               mCoefficientC.at( i ) * std::pow( ( lookupPoint - this->mMeasurementPoints[i] ), 2.0 ) +
               mCoefficientD.at( i ) * std::pow( ( lookupPoint - this->mMeasurementPoints[i] ), 3.0 );
    };

    private:
    protected:
    /// Attributes
    std::vector< T > mCoefficientB, mCoefficientC, mCoefficientD;    // Coefficients
    std::vector< T > mAuxiliaryAlpha, mAuxiliaryBeta, mAuxiliaryGamma, mAuxiliaryDelta,
     mAuxiliaryEpsilon;    // Auxiliary variables
};

//-------------------------------------------------
//                    2D
//-------------------------------------------------

/// This class contains measurement points and data to determine the lookup
template < typename T >
class LookupType2D
{
    // friend class
    friend class ::TestLookup;

    public:
    LookupType2D( const std::vector< std::vector< T > > &lookupData, const std::vector< T > &measurementPointsRow,
                  const std::vector< T > &measurementPointsColumn )
     // Input data
     : mLookupData( lookupData ),
       mMeasurementPointsRow( measurementPointsRow ),
       mMeasurementPointsColumn( measurementPointsColumn ),
       mCurrentMeasurementPointRowIndex( 0 ),
       mCurrentMeasurementPointColumnIndex( 0 )

       {};

    virtual ~LookupType2D(){};
    virtual T DoLookup( T lookupPointRow, T lookupPointColumn ) = 0;

    const std::vector< std::vector< T > > &GetLookup() const { return mLookupData; };

    virtual T GetMinValueOfLookup() const
    {
        T minvalue = mLookupData.at( 0 ).at( 0 );
        for ( size_t i = 0; i < mLookupData.size(); ++i )
        {
            T minValueOfLookupVector = *std::min_element( mLookupData[i].begin(), mLookupData[i].end() );
            if ( minValueOfLookupVector < minvalue )
                minvalue = minValueOfLookupVector;
        }
        return minvalue;
    }

    virtual T GetMaxValueOfLookup() const
    {
        T maxvalue = mLookupData.at( 0 ).at( 0 );
        for ( size_t i = 0; i < mLookupData.size(); ++i )
        {
            T maxValueOfLookupVector = *std::max_element( mLookupData[i].begin(), mLookupData[i].end() );
            if ( maxValueOfLookupVector > maxvalue )
                maxvalue = maxValueOfLookupVector;
        }
        return maxvalue;
    }

    inline const std::vector< T > &GetPointsRow() const { return mMeasurementPointsRow; }
    inline const std::vector< T > &GetPointsCol() const { return mMeasurementPointsColumn; }

    protected:
    // Get lower bound of row or column, return the index
    size_t GetLowerBound( const T value, size_t &currentMeasurementPointIndex, const std::vector< T > &measurementPoints )
    {
        while ( ( currentMeasurementPointIndex < measurementPoints.size() - 1 ) &&
                ( value > measurementPoints[currentMeasurementPointIndex + 1] ) )
            ++currentMeasurementPointIndex;

        while ( ( currentMeasurementPointIndex > 0 ) && ( value < measurementPoints[currentMeasurementPointIndex] ) )
            --currentMeasurementPointIndex;

        return currentMeasurementPointIndex;
    }

    // Attributes
    std::vector< std::vector< T > > mLookupData;
    std::vector< T > mMeasurementPointsRow;
    std::vector< T > mMeasurementPointsColumn;
    size_t mCurrentMeasurementPointRowIndex;
    size_t mCurrentMeasurementPointColumnIndex;
};

/// This class contains the nearest neighbour behaviour for 2D lookups
template < typename T >
class NearestNeighbour2D : public LookupType2D< T >
{
    public:
    NearestNeighbour2D( const typename std::vector< std::vector< T > > &lookupData,
                        const typename std::vector< T > &measurementPointsRow, const typename std::vector< T > &measurementPointsColumn )
        : LookupType2D< T >( lookupData, measurementPointsRow, measurementPointsColumn )

        {};
    virtual ~NearestNeighbour2D(){};
    virtual T DoLookup( T lookupPointRow, T lookupPointColumn )
    {
        size_t i = LookupType2D< T >::GetLowerBound( lookupPointRow, this->mCurrentMeasurementPointRowIndex, this->mMeasurementPointsRow );
        size_t j = LookupType2D< T >::GetLowerBound( lookupPointColumn, this->mCurrentMeasurementPointColumnIndex,
                                                     this->mMeasurementPointsColumn );

        if ( std::abs( this->mMeasurementPointsRow.at( i + 1 ) - lookupPointRow ) <
             std::abs( this->mMeasurementPointsRow.at( i ) - lookupPointRow ) )
            i++;

        if ( std::abs( this->mMeasurementPointsColumn.at( j + 1 ) - lookupPointColumn ) <
             std::abs( this->mMeasurementPointsColumn.at( j ) - lookupPointColumn ) )
            j++;
        return this->mLookupData.at( j ).at( i );
    };

    private:
    protected:
};

/// This class contains a linear interpolation behaviour for 2D lookups
template < typename T >
class Linear2DInterpolation : public LookupType2D< T >
{
    // friend class
    friend class ::TestLookup;

    public:
    Linear2DInterpolation( const std::vector< std::vector< T > > &lookupData,
                           const std::vector< T > &measurementPointsRow, const std::vector< T > &measurementPointsColumn )
        : LookupType2D< T >( lookupData, measurementPointsRow, measurementPointsColumn )

        // Extended input data
        , mExtendedMeasurementPointsRow( measurementPointsRow.size() + 2, 0 )    // + 2 because two points are added
        , mExtendedMeasurementPointsColumn( measurementPointsColumn.size() + 2,
                                            0 )    // + 2 because two points are added
        , mExtendedLookupData( measurementPointsColumn.size() + 2, std::vector< double >( measurementPointsRow.size() + 2, 0 ) )    // + 2 because two points are added

        // Slopes which are calculated only once
        , mRowSlopes( measurementPointsColumn.size() + 2, std::vector< T >( measurementPointsRow.size() + 1, 0 ) )    // + 1 because there are (n - 1) row slopes for n row points
        , mColumnSlopes( measurementPointsColumn.size() + 1, std::vector< T >( measurementPointsRow.size() + 2, 0 ) )    // + 1 because there are (n - 1) column slopes for n column points
        , mDifferenceOfQuotientOfSlopes( measurementPointsColumn.size() + 2, std::vector< T >( measurementPointsRow.size() + 1, 0 ) )    // + 1 because there are (n - 1) quotients of column slopes for n row points

    {
        // Constructor functions
        CreateExtendedRowsOrColumns( this->mExtendedMeasurementPointsRow,
                                     this->mMeasurementPointsRow );    // Create extended row vector
        CreateExtendedRowsOrColumns( this->mExtendedMeasurementPointsColumn,
                                     this->mMeasurementPointsColumn );    // Create extended column vector
        CreateExtendedLookupData();
        CalculateRowSlopes();
        CalculateColumnSlopes();
        CalculateDifferenceOfQuotientsOfSlopes();
    };

    virtual ~Linear2DInterpolation(){// Destructor functions: empty
    };

    /// Create extended row vector or column vector
    void CreateExtendedRowsOrColumns( std::vector< T > &extendedMeasurementPoints, const std::vector< T > &measurementPoints )
    {
        // Boundary points for row and column points
        const T mySmallNumber = -std::numeric_limits< T >::max();
        const T myBigNumber = std::numeric_limits< T >::max();

        extendedMeasurementPoints.front() = mySmallNumber;         // Fill first element
        for ( size_t i = 0; i < measurementPoints.size(); ++i )    // Fill all but first and last element
        {
            extendedMeasurementPoints.at( i + 1 ) = measurementPoints.at( i );
        }
        extendedMeasurementPoints.back() = myBigNumber;    // Fill last element
    }

    /// Create extended lookup data
    void CreateExtendedLookupData()
    {
        // Copy the rows and add two values, one at the beginning, one at the and
        for ( size_t i = 0; i < this->mMeasurementPointsColumn.size(); ++i )
        {
            this->mExtendedLookupData.at( i + 1 ).front() = this->mLookupData.at( i ).front();    // Copy first element
            for ( size_t j = 0; j < this->mMeasurementPointsRow.size();
                  ++j )    // Copy all elements except first and last
            {
                this->mExtendedLookupData.at( i + 1 ).at( j + 1 ) = this->mLookupData.at( i ).at( j );
            }
            this->mExtendedLookupData.at( i + 1 ).back() = this->mLookupData.at( i ).back();    // Copy last element
        }

        // Copy the first and the last row
        this->mExtendedLookupData.front().assign( this->mExtendedLookupData.at( 1 ).begin(),
                                                  this->mExtendedLookupData.at( 1 ).end() );    // Copy first row
        const size_t extendedColumnSizeMinus2 = this->mExtendedMeasurementPointsColumn.size() - 2;
        this->mExtendedLookupData.back().assign( this->mExtendedLookupData.at( extendedColumnSizeMinus2 ).begin(),
                                                 this->mExtendedLookupData.at( extendedColumnSizeMinus2 ).end() );    // Copy last row
    }

    /// Calculate row slopes
    void CalculateRowSlopes()
    {
        for ( size_t i = 0; i < this->mExtendedMeasurementPointsColumn.size(); ++i )
        {
            for ( size_t j = 0; j < this->mExtendedMeasurementPointsRow.size() - 1;
                  ++j )    // - 1 because there are n - 1 row slopes for n row points
            {
                this->mRowSlopes.at( i ).at( j ) =
                 ( this->mExtendedLookupData.at( i ).at( j + 1 ) - this->mExtendedLookupData.at( i ).at( j ) ) /
                 ( this->mExtendedMeasurementPointsRow.at( j + 1 ) - this->mExtendedMeasurementPointsRow.at( j ) );
            }
        }
    }

    /// Calculate column slopes
    void CalculateColumnSlopes()
    {
        for ( size_t i = 0; i < this->mExtendedMeasurementPointsColumn.size() - 1; ++i )
        {
            // Calculate column slopes
            for ( size_t j = 0; j < this->mExtendedMeasurementPointsRow.size();
                  ++j )    // - 1 because there are n - 1 row slopes for n row points
            {
                this->mColumnSlopes.at( i ).at( j ) =
                 ( this->mExtendedLookupData.at( i + 1 ).at( j ) - this->mExtendedLookupData.at( i ).at( j ) ) /
                 ( this->mExtendedMeasurementPointsColumn.at( i + 1 ) - this->mExtendedMeasurementPointsColumn.at( i ) );
            }
        }
    }

    /// Calculate difference of quotients of slopes
    void CalculateDifferenceOfQuotientsOfSlopes()
    {
        for ( size_t i = 0; i < this->mExtendedMeasurementPointsColumn.size() - 1; ++i )
        {
            for ( size_t j = 0; j < this->mExtendedMeasurementPointsRow.size() - 1; ++j )
            {
                this->mDifferenceOfQuotientOfSlopes.at( i ).at( j ) =
                 ( this->mColumnSlopes.at( i ).at( j + 1 ) - this->mColumnSlopes.at( i ).at( j ) ) /
                 ( this->mExtendedMeasurementPointsRow.at( j + 1 ) - this->mExtendedMeasurementPointsRow.at( j ) );
            }
        }
    }

    /// Calculate 2d lookup
    virtual T DoLookup( T lookupPointRow, T lookupPointColumn )
    {
        // Get the index of the relevant measurement points for this lookup point. Take the lower index, not the upper.
        const size_t j = LookupType2D< T >::GetLowerBound( lookupPointRow, this->mCurrentMeasurementPointRowIndex,
                                                           this->mExtendedMeasurementPointsRow );
        const size_t i = LookupType2D< T >::GetLowerBound( lookupPointColumn, this->mCurrentMeasurementPointColumnIndex,
                                                           this->mExtendedMeasurementPointsColumn );

        // Calculate the lookup
        const T res =
         this->mExtendedLookupData[i][j] + this->mRowSlopes[i][j] * ( lookupPointRow - this->mExtendedMeasurementPointsRow[j] ) +
         ( this->mColumnSlopes[i][j] +
           ( lookupPointRow - this->mExtendedMeasurementPointsRow[j] ) * this->mDifferenceOfQuotientOfSlopes[i][j] ) *
          ( lookupPointColumn - this->mExtendedMeasurementPointsColumn[i] );

        return res;
    };

    private:
    protected:
    // Attributes
    std::vector< T > mExtendedMeasurementPointsRow;
    std::vector< T > mExtendedMeasurementPointsColumn;
    std::vector< std::vector< T > > mExtendedLookupData;    // matrix

    std::vector< std::vector< T > > mRowSlopes;                       // matrix
    std::vector< std::vector< T > > mColumnSlopes;                    // matrix
    std::vector< std::vector< T > > mDifferenceOfQuotientOfSlopes;    // matrix
};

/// This class contains constant until threshhold behaviour for 2D lookups
template < typename T >
class ConstantUntilThreshold2D : public LookupType2D< T >
{
    public:
    ConstantUntilThreshold2D( const typename std::vector< std::vector< T > > &lookupData,
                              const typename std::vector< T > &measurementPointsRow,
                              const typename std::vector< T > &measurementPointsColumn )
        : LookupType2D< T >( lookupData, measurementPointsRow, measurementPointsColumn )

        {};
    virtual ~ConstantUntilThreshold2D(){};
    virtual T DoLookup( T lookupPointRow, T lookupPointColumn )
    {
        // cast is used to get only integers, e.g. lookupPoint = 9.9 -> lookupPoint 9.0
        size_t j = LookupType2D< T >::GetLowerBound( static_cast< long >( lookupPointRow + 1 ),
                                                     this->mCurrentMeasurementPointRowIndex, this->mMeasurementPointsRow );
        size_t i = LookupType2D< T >::GetLowerBound( static_cast< long >( lookupPointColumn + 1 ),
                                                     this->mCurrentMeasurementPointColumnIndex, this->mMeasurementPointsColumn );

        return this->mLookupData[i][j];
    };

    private:
    protected:
};

/// This class contains bicubic interpolation behaviour for 2D lookups. Algorithm is based on the book "Numerical
/// Recipes".
template < typename T >
class BicubicInterpolation2D : public LookupType2D< T >
{
    // friend class
    friend class ::TestLookup;

    public:
    BicubicInterpolation2D( const typename std::vector< std::vector< T > > &lookupData,
                            const typename std::vector< T > &measurementPointsRow, const typename std::vector< T > &measurementPointsColumn )
        : LookupType2D< T >( lookupData, measurementPointsRow, measurementPointsColumn )
        , myLinear2DInterpolation( lookupData, measurementPointsRow,
                                   measurementPointsColumn )    // Create Linear2DInterpolation for lookup outside the
                                                                // given measurement points.

        {};

    virtual ~BicubicInterpolation2D(){};

    /// Get knwonVectorBeta
    std::vector< T > GetKnownVectorBeta( size_t rowCoordinate, size_t columnCoordinate )
    {
        std::vector< T > knownVectorBeta( 4, 0 );

        knownVectorBeta.at( 0 ) = this->mLookupData[rowCoordinate][columnCoordinate];
        knownVectorBeta.at( 1 ) = this->mLookupData[rowCoordinate][columnCoordinate + 1];
        knownVectorBeta.at( 2 ) = this->mLookupData[rowCoordinate + 1][columnCoordinate + 1];
        knownVectorBeta.at( 3 ) = this->mLookupData[rowCoordinate + 1][columnCoordinate];

        return knownVectorBeta;
    }

    /// Get neededCoefficientsAlpha
    std::vector< T > GetNeededCoefficientsAlpha( const std::vector< T > &knownVectorBeta )
    {
        std::vector< T > neededCoefficientsAlpha( 9, 0 );

        neededCoefficientsAlpha[0] = ( knownVectorBeta[0] );
        neededCoefficientsAlpha[1] = ( -3 * knownVectorBeta[0] + 3 * knownVectorBeta[3] );
        neededCoefficientsAlpha[2] = ( 2 * knownVectorBeta[0] - 2 * knownVectorBeta[3] );

        neededCoefficientsAlpha[3] = ( -3 * knownVectorBeta[0] + 3 * knownVectorBeta[1] );
        neededCoefficientsAlpha[4] =
         ( 9 * knownVectorBeta[0] - 9 * knownVectorBeta[1] + 9 * knownVectorBeta[2] - 9 * knownVectorBeta[3] );
        neededCoefficientsAlpha[5] =
         ( -6 * knownVectorBeta[0] + 6 * knownVectorBeta[1] - 6 * knownVectorBeta[2] + 6 * knownVectorBeta[3] );

        neededCoefficientsAlpha[6] = ( 2 * knownVectorBeta[0] - 2 * knownVectorBeta[1] );
        neededCoefficientsAlpha[7] =
         ( -6 * knownVectorBeta[0] + 6 * knownVectorBeta[1] - 6 * knownVectorBeta[2] + 6 * knownVectorBeta[3] );
        neededCoefficientsAlpha[8] =
         ( 4 * knownVectorBeta[0] - 4 * knownVectorBeta[1] + 4 * knownVectorBeta[2] - 4 * knownVectorBeta[3] );

        return neededCoefficientsAlpha;
    }

    /// Calculate lookup
    virtual T DoLookup( T lookupPointRow, T lookupPointColumn )
    {
        // Get the index of the relevant measurement points for this lookup point. Take the lower index, not the upper.
        const size_t rowIndex = LookupType2D< T >::GetLowerBound( lookupPointRow, this->mCurrentMeasurementPointRowIndex,
                                                                  this->mMeasurementPointsRow );
        const size_t columnIndex = LookupType2D< T >::GetLowerBound( lookupPointColumn, this->mCurrentMeasurementPointColumnIndex,
                                                                     this->mMeasurementPointsColumn );

        // Bicubic interpolation inside the boundaries
        if ( lookupPointRow >= this->mMeasurementPointsRow.front() && lookupPointRow <= this->mMeasurementPointsRow.back() &&
             lookupPointColumn >= this->mMeasurementPointsColumn.front() &&
             lookupPointColumn <= this->mMeasurementPointsColumn.back() )
        {

            // Auxiliary Slopes
            T u = 0;
            T t = 0;
            t = ( lookupPointRow - this->mMeasurementPointsRow[rowIndex] ) /
                ( this->mMeasurementPointsRow[rowIndex + 1] - this->mMeasurementPointsRow[rowIndex] );
            u = ( lookupPointColumn - this->mMeasurementPointsColumn[columnIndex] ) /
                ( this->mMeasurementPointsColumn[columnIndex + 1] - this->mMeasurementPointsColumn[columnIndex] );

            const std::vector< T > beta = GetKnownVectorBeta( columnIndex, rowIndex );
            const std::vector< T > alpha = GetNeededCoefficientsAlpha( beta );

            const T powU2 = std::pow( u, 2.0 );    // Used in the following
            const T powU3 = powU2 * u;

            return alpha[0] + alpha[1] * powU2 + alpha[2] * powU3 +
                   std::pow( t, 2.0 ) * ( ( alpha[3] + alpha[4] * powU2 + alpha[5] * powU3 ) +
                                          t * ( alpha[6] + alpha[7] * powU2 + alpha[8] * powU3 ) );
        }

        else
        {
            // LinearInterpolation outside the boundaries
            return this->myLinear2DInterpolation.DoLookup( lookupPointRow, lookupPointColumn );
        }
    }

    private:
    protected:
    // Attributes
    lookup::Linear2DInterpolation< T > myLinear2DInterpolation;
};

};    // namespace lookup

#endif /* _LOOKUPTYPE_ */
