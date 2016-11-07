#include "streamparser.h"

using namespace std;

const QString  StreamParser::types[] =  {
    "NoToken",
    "Invalid",
    "StartDocument",
    "EndDocument",
    "StartElement",
    "EndElement",
    "Characters",
    "Comment",
    "DTD",
    "EntityReference",
    "ProcessingInstruction"
};

StreamParser::StreamParser(QString filename, Patricia<int>* patricia, std::unordered_map<int,Container*>* containers)
{
    this->end = false;
    this->xmlFile = new QFile(filename);
    this->xmlFile->open(QIODevice::ReadOnly);
    this->xml.setDevice(xmlFile);
    this->patricia = patricia;
    this->containers = containers;
    this->nextContainerID = ID_CONTAINER_STRUCTURE+1;
    this->nextTagId = TAG_MAP_CONTAINER_STRUCTURE+1;
    this->contLengthBuffer = 0;
    this->out = new ofstream(Util::getOutputFilename(filename), ofstream::binary);
}

StreamParser::~StreamParser()
{
    this->xmlFile->close();
    delete out;
}

bool StreamParser::treatNextTag()
{
    if(!isEnd()){
        switch (xml.tokenType()) {
            case QXmlStreamReader::EndDocument:
                endDocument();
                break;
            case QXmlStreamReader::Invalid:
                invalid();
                return false;
            case QXmlStreamReader::StartElement:
                startElement();
                break;
            case QXmlStreamReader::EndElement:
                endElement();
                break;
            case QXmlStreamReader::StartDocument:
                startDocument();
                break;
            case QXmlStreamReader::Comment:
                comment();
                break;
            case QXmlStreamReader::DTD:
                dtd();
                break;
            case QXmlStreamReader::Characters:
                characters();
                break;
            default: break;
        }
        xml.readNext();
    }
    return true;
}

bool StreamParser::isEnd()
{
    return this->end;
}

int StreamParser::compress(char *compressed, char *data, long len)
{
    long nDestLen = EZ_COMPRESSMAXDESTLENGTH(len);
    compressed = new char[nDestLen];
    int nErr = ezcompress( reinterpret_cast<unsigned char*>(compressed), &nDestLen,
                           reinterpret_cast<unsigned char*>(data), len );
    if ( nErr == EZ_BUF_ERROR ) {
      delete[] compressed;
      compressed = new char[nDestLen]; // enough room now
      nErr = ezcompress( reinterpret_cast<unsigned char*>(compressed), &nDestLen,
                           reinterpret_cast<unsigned char*>(data), len );
    }
    return (int)(nDestLen);
}

int StreamParser::uncompress(char *uncompressed, char *data, long len)
{
    long nDestLen = EZ_UNCOMPRESSMAXDESTLENGTH(len);
    uncompressed = new char[nDestLen];
    int nErr = ezuncompress( reinterpret_cast<unsigned char*>(uncompressed), &nDestLen,
                           reinterpret_cast<unsigned char*>(data), len );
    if ( nErr == EZ_BUF_ERROR ) {
      delete[] uncompressed;
      uncompressed = new char[nDestLen]; // enough room now
      nErr = ezuncompress( reinterpret_cast<unsigned char*>(uncompressed), &nDestLen,
                           reinterpret_cast<unsigned char*>(data), len );
    }
    return (int)(nDestLen);
}

void StreamParser::startElement()
{
    int id = insertPatricia(xml.name());
    tagIds.push(id);
    list<int> aux;
    aux.push_back(id);
    foreach(QXmlStreamAttribute a, xml.attributes()){
        string key = "@"+Util::trim(a.name());
        id = insertPatricia(key);
        Container* c = getOrInsertContainer(id);
        this->contLengthBuffer += c->addData(a.value());
        aux.push_back(id);aux.push_back(-c->id);aux.push_back(0);
    }
    this->contLengthBuffer += structureContainer->addStructureData(aux);
    if(this->contLengthBuffer>SIZE_TO_COMPRESS){
        compressContainers();
    }
}

void StreamParser::endElement()
{
    tagIds.pop();
    int aux[] = {0};
    this->contLengthBuffer += structureContainer->addStructureData(aux,1);
    if(this->contLengthBuffer>SIZE_TO_COMPRESS){
        compressContainers();
    }
}

void StreamParser::characters()
{
    int id = tagIds.top();
    Container* c = getOrInsertContainer(id);
    this->contLengthBuffer += c->addData(xml.text());
    int aux[] = {-c->id};
    this->contLengthBuffer += structureContainer->addStructureData(aux, 1);
    if(this->contLengthBuffer>SIZE_TO_COMPRESS){
        compressContainers();
    }
}

void StreamParser::comment()
{
    int id = insertPatricia("!--");
    Container* c = getOrInsertContainer(id);
    this->contLengthBuffer += c->addData(xml.text());
    int aux[] = {id, -c->id, 0};
    this->contLengthBuffer += this->structureContainer->addStructureData(aux, 3);
    if(this->contLengthBuffer>SIZE_TO_COMPRESS){
        compressContainers();
    }
}

void StreamParser::dtd()
{
    int id = insertPatricia("!DOCTYPE");
    Container* c = getOrInsertContainer(id);
    this->contLengthBuffer += c->addData(xml.text());
    int aux[] = {id, -c->id, 0};
    this->contLengthBuffer += this->structureContainer->addStructureData(aux, 3);
    if(this->contLengthBuffer>SIZE_TO_COMPRESS){
        compressContainers();
    }
}

void StreamParser::startDocument()
{

    int xmlId = this->nextTagId++;
    int encId = this->nextTagId++;
    int verId = this->nextTagId++;
    int stanId = this->nextTagId++;

    patricia->insert("?xml", xmlId);
    patricia->insert("@encoding", encId);
    patricia->insert("@version", verId);
    patricia->insert("@standalone", stanId);


    this->structureContainer = new Container(ID_CONTAINER_STRUCTURE, TAG_MAP_CONTAINER_STRUCTURE);
    Container *staC = new Container(this->nextContainerID++, stanId);
    Container *encC = new Container(this->nextContainerID++, encId);
    Container *verC = new Container(this->nextContainerID++, verId);
    Container *xmlC = new Container(this->nextContainerID++, xmlId);

     
    staC->addData((this->xml.isStandaloneDocument() ? "true" : "false"));
    encC->addData(this->xml.documentEncoding());
    verC->addData(this->xml.documentVersion());

    int aux[] = {xmlId, verId, -verC->id, 0, encId, -encC->id, 0, stanId, -staC->id, 0, 0};
    structureContainer->addStructureData(aux, 11);

    (*containers)[TAG_MAP_CONTAINER_STRUCTURE] = this->structureContainer;
    (*containers)[stanId] = staC;
    (*containers)[encId] = encC;
    (*containers)[verId] = verC;
    (*containers)[xmlId] = xmlC;

    this->contLengthBuffer += this->structureContainer->size + staC->size + verC->size + encC->size + xmlC->size;
    if(this->contLengthBuffer>SIZE_TO_COMPRESS){
        compressContainers();
    }

}

void StreamParser::endDocument()
{
    if(this->contLengthBuffer>0){
        this->compressContainers();
    }
    this->end = true;
}

void StreamParser::invalid()
{
    qDebug() << "Token inválido!";
}

int StreamParser::insertPatricia(const string &key)
{
    int id = this->patricia->get(key);
    if(id==NULL){
        this->patricia->insert(key, this->nextTagId);
        id = this->nextTagId++;
    }
    return id;
}

int StreamParser::insertPatricia(QStringRef key)
{
    return insertPatricia(Util::trim(key));
}

Container *StreamParser::getOrInsertContainer(int id)
{
    Container* c = nullptr;
    try {
        c = containers->at(id);
    } catch (const std::out_of_range& e) {
        c = new Container(this->nextContainerID++, id);
        (*containers)[id] = c;
    }
    return c;
}

void StreamParser::compressContainers()
{
    int tAux;
    char *aux = Util::convertIntToChar(patricia->getSize(), tAux);
    out->write(aux, tAux);
    for(Patricia<int>::iterator it = patricia->begin();it!=patricia->end();it++){
        *out << *it << " ";
        aux = Util::convertIntToChar(*(+it), tAux);
        out->write(aux, tAux);
    }
    aux = Util::convertIntToChar(containers->size(), tAux);
    out->write(aux, tAux);

    for(unordered_map<int,Container*>::iterator it = containers->begin(); it != containers->end(); ++it){
        Container *c = *(&it->second);
        char *buffer = c->getData();
        long t = EZ_COMPRESSMAXDESTLENGTH(c->size);
        char* compressed = new char[t];
        compress(compressed, buffer, t);
        aux = Util::convertIntToChar(int(t), tAux);
        out->write(aux, tAux);
        aux = Util::convertIntToChar(c->id, tAux);
        out->write(aux, tAux);
        out->write(compressed, t);
    }
    out->flush();

    /*delete containers;
    delete patricia;
    delete structureContainer;

    this->patricia = new Patricia<int>();
    this->containers = new unordered_map<int,Container*>;
    this->nextContainerID = ID_CONTAINER_STRUCTURE+1;
    this->nextTagId = TAG_MAP_CONTAINER_STRUCTURE+1;
    this->contLengthBuffer = 0;
    this->structureContainer = new Container(ID_CONTAINER_STRUCTURE, TAG_MAP_CONTAINER_STRUCTURE);
    (*containers)[TAG_MAP_CONTAINER_STRUCTURE] = structureContainer;*/

}

