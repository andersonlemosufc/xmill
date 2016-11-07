#include "mainwindow.h"
#include <QApplication>
#include <iostream>

using namespace std;

void print(int a){
    cout << a << endl;
}

int main(int argc, char *argv[])
{

    int op = 1;

    QString f1 = "/home/anderson/Área de Trabalho/books.xml";
    QString f2 = "/home/anderson/Área de Trabalho/brasil.svg";
    QString f3 = "/home/anderson/Área de Trabalho/a.xml";

    QString filename = "";

    switch (op) {
        case 1: filename = f1; break;
        case 2: filename = f2; break;
        case 3: filename = f3; break;
    }

    XMill *xmill = new XMill();
    xmill->compress(filename);
    return 0;
    /*QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();*/
}

