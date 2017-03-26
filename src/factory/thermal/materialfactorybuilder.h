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
#ifndef MATERIALFACTORYBUILDER_H_
#define MATERIALFACTORYBUILDER_H_

#include "materialclasswrapper.h"

namespace factory
{

/// Builds a Factory for thermal::Material
template < typename ValueT >
Factory< thermal::Material< ValueT > > *BuildMaterialFactory()
{
    Factory< thermal::Material< ValueT > > *materialFactory = new Factory< thermal::Material< ValueT > >();
    materialFactory->AddWrapper( new MaterialClassWrapperIsotropic< ValueT >(), "Material" );
    materialFactory->AddWrapper( new MaterialClassWrapperCartesian< ValueT >(), "MaterialCartesian" );
    materialFactory->AddWrapper( new MaterialClassWrapperCylindric< ValueT >(), "MaterialCylindric" );
    return materialFactory;
}

} /* namespace factory */

#endif /* MATERIALFACTORYBUILDER_H_ */
