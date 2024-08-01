//===- svf-ex.cpp -- A driver example of SVF-------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // A driver program of SVF including usages of SVF APIs
 //
 // Author: Yulei Sui,
 */
#include "util.h"
#include "ec.h"
#include "crt.h"
#include "etsan.h"
bool EC = true;
bool TestPass = false;
bool CRT = false; bool emitAll = true;
bool ETSAN = false;
bool analyzecode = true;
int main(int argc, char ** argv) {
    int error_value = 0;
    parseArguments(argc,argv);

    //Temp for debugging and hacking
    return error_value;
}
