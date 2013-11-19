/**********************************************************************
    * mul.hpp -- a multilpier process                                 *
    *                                                                 *
    * Author:  Hosien Attarzadeh (shan2@kth.se)                       *
    *                                                                 *
    * Purpose: Demonstration of a simple sequential processes.        *
    *                                                                 *
    * Usage:   MulAcc example                                         *
    *                                                                 *
    * License: BSD3                                                   *
    *******************************************************************/


#ifndef MUL_HPP
#define MUL_HPP

#include <forsyde.hpp>

using namespace ForSyDe;
using namespace ForSyDe::SY;

void mul_func(abst_ext<int>& out1,
              const abst_ext<int>& a, const abst_ext<int>& b)
{
    int inp1 = from_abst_ext(a,0);
    int inp2 = from_abst_ext(b,0);

#pragma ForSyDe begin mul_func  
    out1 = inp1 * inp2;
#pragma ForSyDe end
}

#endif
