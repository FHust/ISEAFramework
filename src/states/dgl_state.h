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
* File Name : dgl_state.h
* Creation Date : 11-11-2012
* Last Modified : Mo 08 Sep 2014 15:27:35 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _DGL_STATE_
#define _DGL_STATE_


namespace state
{
/// Basic Interface for a state
class Dgl_state
{
    public:
    Dgl_state(){};
    virtual ~Dgl_state(){};

    /// Deprecated
    virtual double GetMaxDt() const = 0;    // returns the Maximum dt step that can be done before any major change in
                                            // the state/lookup occures

    virtual double GetValue() const = 0;
    virtual double operator()() const { return GetValue(); };

    private:
    protected:
};
} /* END NAMESPACE */

#endif /* _DGL_STATE_ */
