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
#include "testExceptions.h"

#include <cstring>

#include "../../exceptions/error_proto.h"

void TestExceptions::TestOutput()
{
    /*
        try
        {
            ErrorFunction<std::runtime_error> ( "rectangular_block", 100, "error-file", "ElementsDimensionsNegative",
       "blubb");
        }
        catch (std::runtime_error &e)
        {
            std::cout<< e.what() <<std::endl;
        }
    */

    TS_ASSERT_THROWS_EQUALS( ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                                  "ElementsDimensionsNegative", "blubb" ),
                             std::runtime_error & e,
                             strcmp( e.what(), "Every element of Dimensions must be bigger than zero in blubb.\n" ), 0 );
}
