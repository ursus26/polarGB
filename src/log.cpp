/**
 * Copyright (C) 2018 Bart de Haan
 *
 * polarGB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * polarGB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include "polarGB/log.h"


bool Log::isVerbose = false;


void Log::printVerbose(string msg)
{
    if(isVerbose)
        cout << msg << endl;
}


void Log::printError(string msg)
{
    cerr << msg << endl;
}


void Log::setVerbose(bool verbose)
{
    isVerbose = verbose;
}
