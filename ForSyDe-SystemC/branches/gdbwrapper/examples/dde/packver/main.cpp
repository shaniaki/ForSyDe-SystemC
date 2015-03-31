/**********************************************************************
    * main.cpp -- the main file and testbench for the packet verifier *
    *                                                                 *
    * Author:  Hosein Attarzadeh (shan2@kth.se)                       *
    *                                                                 *
    * Purpose: Demonstration of a simple DDE system.                  *
    *                                                                 *
    * Usage:   Packet Verifier example                                *
    *                                                                 *
    * License: BSD3                                                   *
    *******************************************************************/

#include "top.hpp"

int sc_main(int argc, char **argv)
{
    top top1("top1");

    sc_start(100,SC_MS);
        
    return 0;
}


