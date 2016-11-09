#ifndef STREAMPARSER_H
#define STREAMPARSER_H

#include <QString>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>

#include <iostream>
#include <unordered_map>
#include <stack>
#include <fstream>

#include "./tree/patricia.h"
#include "./compress/container.h"
#include "./util/util.h"
#include "./compress/zlib/easyzlib.h"

#define ID_CONTAINER_STRUCTURE int(1)
#define TAG_MAP_CONTAINER_STRUCTURE int(1)

class StreamParser
{

public:
    const static QString types[];
    StreamParser(QString filename, Patricia<int>* patricia, std::unordered_map<int,Container*>* containers);
    ~StreamParser();
    bool treatNextTag();
    bool isEnd();
    int compress(char **compressed, char *data, long len);
    int uncompress(char **uncompressed, char *data, long len);

private:
    bool end;
    QXmlStreamReader xml;
    QFile* xmlFile;
    Patricia<int>* patricia;
    std::unordered_map<int,Container*>* containers;
    int nextContainerID;
    int nextTagId;
    int contLengthBuffer;
    std::stack<int> tagIds;
    Container *structureContainer;
    std::ofstream* out;

    void startElement();
    void endElement();
    void characters();
    void comment();
    void dtd();
    void startDocument();
    void endDocument();
    void invalid();

    int insertPatricia(const std::string &key);
    int insertPatricia(QStringRef key);
    Container *getOrInsertContainer(int id);
    void compressContainers();
};

#endif // STREAMPARSER_H
