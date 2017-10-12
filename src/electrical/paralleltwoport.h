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
* File Name : paralleltwoport.h
* Creation Date : 30-10-2012
* Last Modified : Di 08 Mär 2016 15:48:42 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _PARALLELTWOPORT_
#define _PARALLELTWOPORT_

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "twoport_withchild.h"

class TestTwoPortsWithChildren;

namespace electrical
{

/// This TwoPort represents the parallel branch of a circuit and can have children. For every child above 1, we have to
/// add anoterh equation to our algebraic system
template < typename T = myMatrixType >
class ParallelTwoPort : public TwoPortWithChild< T >
{
    friend class ::TestDaeSystem;
    friend class ::TestTwoPortsWithChildren;

    public:
    explicit ParallelTwoPort( const bool observable = false,
                              typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType(new ElectricalDataStruct< ScalarUnit >));
    virtual ~ParallelTwoPort(){};

    virtual void SetCurrent( const T current );                ///< Set current for all the children. For "n" branchee in the parallel circuit we need "n-1" Variables to represent the current
    virtual void SetCurrent( const ScalarUnit currentval );    ///< Sets the current value

    size_t GetParallelChildren() const;    ///< Count the children of this TwoPort

    virtual T* GetVoltage();

    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );    ///< For every child above 1 Increase the count of parallel UIDs

    virtual const char* GetName() const;

    private:
    protected:
    std::vector< size_t > mUIDs;
};

template < typename T >
ParallelTwoPort< T >::ParallelTwoPort( const bool observable, typename TwoPort< T >::DataType dataValues )
    : TwoPortWithChild< T >( observable, dataValues )
{
}

template < typename T >
void ParallelTwoPort< T >::SetCurrent( const T current )
{
    TwoPort< T >::SetCurrent( current );

    if ( this->mChildren.empty() )
        return;

    T cur = current;
    for ( size_t i = 0; i < this->mChildren.size() - 1; ++i )
        cur( 0, this->mStateSystemGroup->mDglStateSystem.GetEquationCount() + mUIDs[i] ) = -1;

    ( *( this->mChildren.begin() ) )->SetCurrent( cur );


    for ( size_t i = 1; i < this->mChildren.size(); ++i )
    {
        cur.zeros( current.n_rows, current.n_cols );
        cur( 0, this->mStateSystemGroup->mDglStateSystem.GetEquationCount() + mUIDs[i - 1] ) = 1;
        this->mChildren[i]->SetCurrent( cur );
    }
}

template < typename T >
void ParallelTwoPort< T >::SetCurrent( const ScalarUnit currentval )
{
    TwoPort< T >::SetCurrent( currentval );
    if ( !this->mChildren.empty() )
        this->mChildren.front()->SetCurrent( currentval );
}

template < typename T >
size_t ParallelTwoPort< T >::GetParallelChildren() const
{
    return this->mChildren.size();
}

template < typename T >
void ParallelTwoPort< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{
    for ( size_t i = 0; i < this->mChildren.size(); ++i )
    {
        if ( i > 0 )
            mUIDs.push_back( stateSystemGroup->mAlgStateSystem.GetNewEquation() );
        this->mChildren[i]->SetSystem( stateSystemGroup );
    }
    TwoPort< T >::SetSystem( stateSystemGroup );
}

template < typename T >
T* ParallelTwoPort< T >::GetVoltage()
{
    if ( this->mChildren.size() == 0 )
        return TwoPort< T >::GetVoltage();
    this->mVoltage = *( this->mChildren.back()->GetVoltage() );
    for ( size_t i = 1; i < this->mChildren.size() - 1; ++i )
        this->mStateSystemGroup->mAlgStateSystem.AddEquations( mUIDs[i - 1],
                                                               this->mVoltage - *( this->mChildren[i]->GetVoltage() ) );
    if ( this->mChildren.size() > 1 )
        this->mStateSystemGroup->mAlgStateSystem.AddEquations( mUIDs.back(),
                                                               this->mVoltage - *( this->mChildren[0]->GetVoltage() ) );

    return TwoPort< T >::GetVoltage();
}

template < typename T >
const char* ParallelTwoPort< T >::GetName() const
{
    return "ParallelTwoPort";
}
} /* END NAMESPACE */
#endif /* _PARALLELTWOPORT_ */
