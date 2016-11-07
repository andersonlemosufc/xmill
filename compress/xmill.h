#ifndef XMILL_H
#define XMILL_H

#include <QString>
#include <iostream>
#include <unordered_map>
#include "parser/streamparser.h"

class XMill
{
public:
    XMill();
    void compress(QString filename);

private:
    Patricia<int>* patriciaTags;
    std::unordered_map<int, Container*> containers;

};

#endif // XMILL_H
