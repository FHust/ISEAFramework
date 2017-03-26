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
 * strcombine.cpp
 *
 *  Created on: 29.08.2013
 *      Author: chris
 */

#include "strlcombine.h"
#include "strlcat.h"
#include <stdlib.h>

char* strlcombine(const char *src1, const char *src2, size_t siz)
{
    char* res = (char*)malloc(siz + 1);
    size_t tmplen = strlcat(res,src1,siz);
    strlcat(res + tmplen, src2, siz - tmplen);
    return res;
}
