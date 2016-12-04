#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

int comp = 1;

void print(int a){
    cout << a << endl;
}

QString getNome(int n, bool compress){
    QString aux = (compress) ? "entradas-comparativo" : "xmi";
    QString rr = "/home/anderson/Documentos/tcc/"+aux+"/";
    if(n<10) rr+="000";
    else if(n<100) rr+="00";
    else if(n<1000) rr+="0";
    rr = (compress) ? rr.append(QString("%1.svg").arg(n)) : rr.append(QString("%1.xmi").arg(n));
    return rr;
}

int main()
{
    int exe = 5;
    high_resolution_clock::time_point t1, t2;
    QString name = "/home/anderson/Documentos/tcc/tempo" + QString("%1").arg(exe)+ ".txt";
    ofstream out(name.toStdString());
    for(int k=1;k<=3000;k++){
        t1 = high_resolution_clock::now();
        //XMill::compress(getNome(k, true));
        XDemill::decompress(getNome(k, false));
        t2 = high_resolution_clock::now();
        auto t = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
        out << t << "\n";
    }
    out.close();
    return 0;
}

