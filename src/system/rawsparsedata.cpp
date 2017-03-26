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
/*
 * rawsparsedata.cpp
 *
 *  Created on: 11.01.2014
 *      Author: chris
 */

#include "rawsparsedata.h"

#ifdef _ARMADILLO_

namespace systm
{

RawSparseData::RawSparseData()
    : mLocations(arma::umat(2,1000)),
      mValues(arma::colvec(1000)),
      mElements(0)
{ }

RawSparseData::RawSparseData(const std::vector< RawSparseData >& vector)
    : mElements(0)
{
    for(size_t i = 0; i < vector.size(); ++i)
        mElements += vector.at(i).mElements;

    mLocations.set_size(2, mElements);
    mValues.set_size(mElements);

    size_t index = 0;
    for(size_t i = 0; i < vector.size(); ++i)
    {
        const RawSparseData& data = vector.at(i);
        const size_t rows = data.mElements;

        if(!rows)
            continue;

        mLocations.cols( index,  index + rows -1) = data.mLocations.cols(0, rows -1);
        mValues.rows( index,  index + rows -1) = data.mValues.rows(0, rows -1);
        index += rows;
    }
}

void RawSparseData::Clear()
{
    mElements = 0;
}

void RawSparseData::Add(const arma::SpMat<double>& matrix, size_t rowOffset)
{
    CheckAndAdaptSize( matrix.n_nonzero );

    for(arma::SpMat<double>::const_iterator it = matrix.begin(); it != matrix.end(); ++it)
    {
        mLocations(0, mElements) = it.row() + rowOffset;
        mLocations(1, mElements) = it.col();
        mValues(mElements) = *it;
        ++mElements;
    }
}

void RawSparseData::Add(const size_t row, const size_t col, const double value)
{
    CheckAndAdaptSize();

    mLocations(0, mElements) = row;
    mLocations(1, mElements) = col;
    mValues(mElements) = value;
    ++mElements;
}

void RawSparseData::Add(const arma::SpMat<double>::const_iterator& beginIt, const arma::SpMat<double>::const_iterator& endIt, size_t rowOffset)
{
    CheckAndAdaptSize();

    for(arma::SpMat<double>::const_iterator it = beginIt; it != endIt; ++it)
    {
        mLocations(0, mElements) = it.row() + rowOffset;
        mLocations(1, mElements) = it.col();
        mValues(mElements) = *it;
        ++mElements;
    }
}

void RawSparseData::FillMatrix(arma::SpMat<double>& matrix, const arma::uword rows, const arma::uword cols)
{
    matrix = arma::SpMat<double>(mLocations, mValues, rows, cols);
}

void RawSparseData::CheckAndAdaptSize(size_t additionalNeededElements)
{
    const size_t neededRows = mElements + additionalNeededElements + 500;
    if(mValues.n_rows < neededRows)
    {
        mLocations.resize(2, neededRows + 1000);
        mValues.resize(neededRows + 1000);
    }
}

} /* namespace systm */

#endif /* _ARMADILLO_ */
