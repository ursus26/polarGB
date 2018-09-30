#include <iostream>
#include "log.h"


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
