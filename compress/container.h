#ifndef CONTAINER_H
#define CONTAINER_H

#include <QDebug>
#include <string>
#include <list>
#include "./util/util.h"

#define DEFAULT_SIZE_CONTAINER_BLOCK int(65536)
#define SIZE_TO_COMPRESS int(8388608) //8 megas

typedef struct block{
    char *data;
    block *next;
    int length;
    block(){
        data = new char[DEFAULT_SIZE_CONTAINER_BLOCK];
        next = nullptr;
        this->length = 0;
    }
    block* addData(char ch){
        data[length++] = ch;
        if(length==DEFAULT_SIZE_CONTAINER_BLOCK){
            block *novo = new block();
            this->next = novo;
            return novo;
        }
        return this;
    }
} Block;

class Container
{
public:

    Container();
    Container(int id, int pathId);

    int id;
    int pathId;
    int size;
    Block *firstBlock;

    //return the number of bytes incremented in the container
    int addStructureData(int v[], int len);
    int addStructureData(std::list<int> list);
    int addData(char* data, int len);
    int addData(std::string data);
    int addData(QStringRef data);
    char *getData();


private:
    Block *current;
};

#endif // CONTAINER_H
