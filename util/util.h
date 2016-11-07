#ifndef UTIL_H
#define UTIL_H
#include <QString>
#include <string>
#include <iostream>

class Util
{
public:
    Util();
    static char* convertIntToChar(int n, int &len); //turn len to the lenght of the return
    static std::string trim(QStringRef s);
    static std::string getOutputFilename(QString filename);
    static void printIntBinaty(int n);
};

#endif // UTIL_H
