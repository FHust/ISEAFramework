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
* File Name : sphericalDiffusion.cpp
* Creation Date : 22-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "sphericalDiffusion.h"

template class electrical::SphericalDiffusion< myMatrixType >;


namespace electrical
{
// Heiko Witzenhause Dissertation page 107 numerical approximation by DRT
template < typename T >
const std::vector< double > SphericalDiffusion< T >::mRFactor =
 {0.991746408883833,    0.335189554500520,   0.172209582896012,   0.0990088488847815,  0.0894316108842422,
  0.0628533575690239,   0.0433650998346649,  0.0409465516111291,  0.0359600164087731,  0.0257087018345930,
  0.0206062060663296,   0.0163927855682163,  0.0133393793597222,  0.00963063557519788, 0.0105619640377753,
  0.00625486628390168,  0.00466427366388678, 0.00489302085372321, 0.00377038387403826, 0.00272828796945697,
  0.00220461281834244,  0.00172611255496559, 0.00122500955193401, 0.00100641130984464, 0.000988915520199850,
  0.000990703772136943, 0.00101843891449268, 0.00109746077161406, 0.00134411783912767};

template < typename T >
const std::vector< double > SphericalDiffusion< T >::mTauFactor =
 {0.991451359646436,    0.335870153447181,    0.167579701571178,    0.0979200017777679,   0.0616838415192040,
  0.0396617079878360,   0.0257407628044021,   0.0166771337841095,   0.0106849233907334,   0.00681822631765891,
  0.00435433073209765,  0.00278113078679821,  0.00177898142175253,  0.00113938740056861,  0.000731596152250355,
  0.000467596673254973, 0.000300414861021452, 0.000192385090821147, 0.000122045399317404, 7.75570733968097e-05,
  4.97188886786378e-05, 3.21196950002819e-05, 2.08531377797455e-05, 1.35775500596413e-05, 8.84548015906210e-06,
  5.76662104649152e-06, 3.76592279815960e-06, 2.46389055469806e-06, 1.61518180144545e-06};
}
