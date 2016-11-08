#ifndef XDEMILL_H
#define XDEMILL_H

#include <QString>
#include <fstream>
#include <unordered_map>
#include <stack>
#include "./compress/zlib/easyzlib.h"
#include "./util/util.h"
#include "./decompress/decontainer.h"

#define BUFFER_SIZE int(1048576) //1 mega

#define ID_CONTAINER_STRUCTURE int(1)


typedef struct {
    int id;
    int type;

    static int NORMAL           = 1;
    static int START_DOCUMENT   = 2;
    static int COMMENT          = 3;
    static int DTD              = 4;
} Tag;


class XDemill
{
public:
    XDemill();
    void decompress(QString filename);
    int uncompress(char **uncompressed, char *data, long len);

private:
    std::unordered_map<int,char*> map;

};

#endif // XDEMILL_H
