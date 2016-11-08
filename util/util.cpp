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

    unsigned char m = 0;
    int index = len;
    int nn = (n<0) ? -n-1 : n;
    while(nn>0){
        if(m==0){
            m = 1;
            index--;
            res[index] = 0;
        }
        if(nn%2==1) res[index] = (m | res[index]);
        nn/=2;
        m<<=1;
    }
    index--;
    while(index>=0){res[index--]=0;}
    if(n<0){
        m = (len==1) ? 64 : 32;
        res[0] = (m | res[0]);
    }

    if(len>1){
        unsigned char m = (len==2) ? 128 : 192;
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
    int k = filename.lastIndexOf("/");
    for(int t=filename.length();k<t;k++){
        if(filename[k]=='.') break;
    }
    return filename.left(k).toStdString()+".xmi";
}

string Util::getDecompressedFilename(QString filename)
{
    int k = filename.lastIndexOf("/");
    for(int t=filename.length();k<t;k++){
        if(filename[k]=='.') break;
    }
    return filename.left(k).toStdString()+".xdemi";
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
int Util::nextInt(char *ch, int *firstPosition)
{
    int res = 0;
        bool isNegative = false;
        unsigned char m = 128;
        int len = 1;

        int k = *firstPosition;
        if(m & ch[k]){
            m>>=1;
            if(m & ch[k]){
                len = 4;
            } else {
                len = 2;
            }
        }
        m>>=1;
        if(m & ch[k]) isNegative = true;
        m>>=1;
        int aux = (len==1) ? 32 : (len==2) ? 4096 : 268435456;
        while(aux>0){
            if(m==0){
                m = 128;
                k++;
            }
            if(m & ch[k]) { res += aux;}
            m>>=1;
            aux/=2;
        }
        (*firstPosition)=k+1;
        return (isNegative) ? -(res+1) : res;
}

void Util::binario(char *ch, int n)
{

    string s = "";
    for(int k=0;k<n;k++){
        for(unsigned char m=128;m>0;m>>=1){
            s+=((m & ch[k]) ? "1" : "0");
        }
        s+=" ";
    }
    cout << s << endl;
}

int Util::nextInt(ifstream *in)
{
    char c;
    in->read(&c, 1);
    int res = 0;
    bool isNegative = false;
    unsigned char m = 128;
    int len = 1;

    int k = 0;
    if(m & c){
        m>>=1;
        if(m & c){
            len = 4;
        } else {
            len = 2;
        }
    }
    char *ch = new char[len];
    ch[0] = c;
    if(len>1) in->read(&ch[1], len-1);
    m>>=1;
    if(m & ch[k]) isNegative = true;
    m>>=1;
    int aux = (len==1) ? 32 : (len==2) ? 4096 : 268435456;
    while(aux>0){
        if(m==0){
            m = 128;
            k++;
        }
        if(m & ch[k]) { res += aux;}
        m>>=1;
        aux/=2;
    }
    return (isNegative) ? -(res+1) : res;
}
