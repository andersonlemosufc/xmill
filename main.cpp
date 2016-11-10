#include "mainwindow.h"
#include <QApplication>
#include <iostream>

using namespace std;

int comp = 1;

void print(int a){
    cout << a << endl;
}

int main(int argc, char *argv[])
{

    int op = 3;
    int compare = 0;


    QString f1 = "Documentos/tmp/teste-xmill/a.xml";
    QString f2 = "Documentos/tmp/teste-xmill/books.xml";
    QString f3 = "Documentos/tmp/teste-xmill/brasil.svg";
    QString f4 = "Documentos/tmp/teste-xmill/big-books.xml";
    QString f5 = "Documentos/tmp/teste-xmill/sample.xml";
    QString f6 = "Documentos/tmp/teste-xmill/teste.xml";


    QString filename = (comp==1) ? "/home/anderson/" : (comp==2) ? "/home/anderson.silva/" : "";

    switch (op) {
        case 1: filename += f1; break;
        case 2: filename += f2; break;
        case 3: filename += f3; break;
        case 4: filename += f4; break;
        case 5: filename += f5; break;
        case 6: filename += f6; break;
    }
    if(compare) filename = Util::getWithoutExtension(filename)+".xdemi";
    qDebug() << filename;

    XMill *xmill = new XMill();
    XDemill *demill = new XDemill();
    xmill->compress(filename);
    if(!compare) demill->decompress(QString::fromStdString(Util::getOutputFilename(filename)));
    return 0;
    /*QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();*/
}

