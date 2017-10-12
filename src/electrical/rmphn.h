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
* File Name : rmphn.h
* Creation Date : 24-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _RMN_
#define _RMN_

#include <cmath>
#include <numeric>
#include "serialtwoport.h"
#include "../object/object.h"

extern template class electrical::SerialTwoPort< myMatrixType >;
extern template class object::Object< double >;

namespace electrical
{

/// DESCRIPTION
template < typename T = myMatrixType >
class Rmphn : public SerialTwoPort< T >
{
    public:
    explicit Rmphn( boost::shared_ptr< object::Object< double > > rCtObj,
                    boost::shared_ptr< object::Object< double > > rMpObj, const bool observable = false,
                    typename SerialTwoPort< T >::DataType dataValues = typename SerialTwoPort< T >::DataType(new ElectricalDataStruct< ScalarUnit >))
        : SerialTwoPort< T >( observable, dataValues )
        , mR_Ct( rCtObj )
        , mRMp( rMpObj ){};


    virtual T* GetVoltage();    ///< returns the voltage over the RC element  Urc  = 1

    virtual ~Rmphn(){};

    private:
    boost::shared_ptr< object::Object< double > > mR_Ct;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
    boost::shared_ptr< object::Object< double > > mRMp;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
    protected:
};

template < typename T >
T* Rmphn< T >::GetVoltage()
{
    const double r_CT = mR_Ct->GetValue();
    const double r_MP = mRMp->GetValue();

    const double r_SER = std::pow( r_MP * r_CT, 0.5 ) * ( 1.0 / tanh( std::pow( r_MP / r_CT, 0.5 ) ) ) -
                         r_CT;    // Could be helpful https://en.wikipedia.org/wiki/Fast_inverse_square_root

    // \omega = 0
    /*     R_ser = R_ser+real((R_MP.*(R_ct./(1+1i.*w.*Tau_dl).^Phi_HN)).^0.5 ...
                 .*coth((R_MP./(R_ct./(1+1i.*w.*Tau_dl).^Phi_HN)).^0.5)...
                     -R_ct);
                     */

    this->mVoltage = *SerialTwoPort< T >::GetVoltage() + this->mCurrent * r_SER;
    return &this->mVoltage;
}
}
#endif /* _RMN_ */
