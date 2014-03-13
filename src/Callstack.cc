/**
\file Callstack.cc
\author Shane Lillie
\brief Callstack debugging module source.

\verbatim
Copyright 2002 Energon Software

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
\endverbatim
*/


#include "Callstack.h"


namespace Callstack
{
    /*
     *  EnterFunction class variables
     *
     */

    Stack EnterFunction::callstack;
    long long EnterFunction::calls(0L);


    /*
     *  EnterFunction class methods
     *
     */


    void EnterFunction::dump(std::ostream& out) throw()
    {
#if defined DEBUG
        out << std::endl << "*** Begin Call Stack Dump ***" << std::endl
            << callstack
            << "*** End Call Stack Dump ***" << std::endl << std::endl
            << "Total function calls monitored: " << calls << std::endl << std::endl;
#else
        out << std::endl << "*** No Call Stack Available ***" << std::endl << std::endl;
#endif
    }


    /*
     *  functions
     *
     */


    void dump(std::ostream& out) throw()
    {
        EnterFunction::dump(out);
    }


    void dump(const std::string& filename) throw()
    {
        std::ofstream outfile(filename.c_str());
        if(!outfile) return;
        dump(outfile);
        outfile.clear();
        outfile.close();
    }
};
