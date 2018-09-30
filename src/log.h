#ifndef LOG_H
#define LOG_H

#include <string.h>

using namespace std;

class Log
{
public:
    static void printVerbose(string msg);
    static void printError(string msg);

    static void setVerbose(bool verbose);
    static bool isVerbose;
};

#endif /* LOG_H */
