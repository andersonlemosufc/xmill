#include "util.h"

using namespace std;

Util::Util()
{

}

char *Util::convertIntToChar(int n, int &len)
{
    if(n>-65 && n<64) len = 1;
    else if(n>-8193 && n<8192) len = 2;
    else len = 4;
    char *res = new char[len];
    for (int i = 0; i < len; i++) {
        int k = len-1-i;
        res[k] = (n >> (i * 8));
    }
    if(len>1){
        char m = (len==2) ? 128 : 192;
        res[0] = res[0] | m;
    }
    return res;
}

std::string Util::trim(QStringRef s)
{
    return s.toString().trimmed().toStdString();
}

string Util::getOutputFilename(QString filename)
{
    int k=0;
    for(int t=filename.length();k<t;k++){
        if(filename[k]=='.') break;
    }
    return filename.left(k).toStdString()+".xmi";
}

void Util::printIntBinaty(int n)
{
    int m = 1 << 31;
    for(int bit=1;bit<33;bit++){
        cout << ((n & m) ? "1" : "0");
        n <<=1;
        if(bit%8==0) cout << " ";
    }
    cout << endl;
}

