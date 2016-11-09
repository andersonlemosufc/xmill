#ifndef UTIL_H
#define UTIL_H
#include <QString>
#include <string>
#include <iostream>
#include <fstream>

class Util
{
public:
    Util();
    static char* convertIntToChar(int n, int &len); //turn len to the lenght of the return
    static std::string trim(QStringRef s);
    static std::string getOutputFilename(QString filename);
    static std::string getDecompressedFilename(QString filename);
    static void printIntBinaty(int n);
    static int nextInt(std::ifstream* in);
    static int nextInt(char first, std::ifstream* in);
    static int nextInt(char *ch, int* firstPosition);
    static void binario(char *ch, int n);
};

#endif // UTIL_H
