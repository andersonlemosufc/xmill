#ifndef XDEMILL_H
#define XDEMILL_H

#include <QDebug>
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
    char *name;
    int type;
    bool hasContent;

    static const int NORMAL           = 1;
    static const int START_DOCUMENT   = 2;
    static const int COMMENT          = 3;
    static const int DTD              = 4;
    static const int ATTRIBUTE        = 5;
} Tag;


class XDemill
{
public:
    XDemill();
    void decompress(QString filename);
    int uncompress(char **uncompressed, char *data, long len);

};

#endif // XDEMILL_H
