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
* File Name : matio_file.h
* Creation Date : 06-07-2015
* Last Modified : Mo 06 Jun 2016 13:24:08 CEST
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MATIO_FILE_
#define _MATIO_FILE_

// BOOST
#include <boost/scoped_ptr.hpp>

// ETC
#include "matio_data.h"
#include "matioWriteTree.h"

namespace matlab
{

/// This struct gives a basic access to a matfile. The variables inside the mat file can be accessed through the []
/// operator
/// This member should not be copied, due to ownership problems with mRootVar --> boost::noncopyable
struct MatFile : private boost::noncopyable
{
    MatFile();
    /// This c-tor allows to access a matfile for RO or RW access. Currently on RO is implemented
    /// \param filename filename to be accessed including .mat file ending
    /// \param op operation on the e.g. RO or RW
    MatFile( std::string filename, mat_acc op = MAT_ACC_RDONLY );

    // The Destructor clean up after file IO. Especially it writes provides data to the file here.
    ~MatFile();

    /// This function acceses the mat file and tries to access the struct variableName
    /// \param variableName is the name of the struct that should be accessed
    MatioData &operator[]( std::string variableName );

    /// Write all Data to file. This is automatically triggered in the destructor if data is present to be writte
    void WriteData();

    /// Feee MatioData to the file
    /// \param matData that should be written into the file
    MatFile &operator<<( MatioData matData );

    private:
    boost::scoped_ptr< MatioData > mReturnMember;
    boost::scoped_ptr< MatioWriteTree > mWriteTree;
    mat_t *mMatFile;
    matvar_t *mRootVar;
    std::vector< MatioData > mLazyWriteVec;
    mat_acc mFileOp;
};
} /* matlab */
#endif /* _MATIO_FILE_ */
