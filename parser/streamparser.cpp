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
    this->out = new ofstream(Util::getOutputFilename(filename), ofstream::binary);


}

StreamParser::~StreamParser()
{
    this->xmlFile->close();
    out->close();
    delete this->xmlFile;
    /*delete this->patricia;
    for(unordered_map<int,Container*>::iterator it=containers->begin();
        it!=containers->end();it++){
        Container *c = *(&it->second);
        delete c;
    }
    delete structureContainer;*/
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

int StreamParser::compress(char **compressed, char *data, long len)
{
    long nDestLen = EZ_COMPRESSMAXDESTLENGTH(len);
    *compressed = new char[nDestLen];
    int nErr = ezcompress( reinterpret_cast<unsigned char*>(*compressed), &nDestLen,
                           reinterpret_cast<unsigned char*>(data), len );
    if ( nErr == EZ_BUF_ERROR ) {
      delete[] *compressed;
      *compressed = new char[nDestLen]; // enough room now
      nErr = ezcompress( reinterpret_cast<unsigned char*>(*compressed), &nDestLen,
                           reinterpret_cast<unsigned char*>(data), len );
    }
    return (int)(nDestLen);
}

int StreamParser::uncompress(char **uncompressed, char *data, long len)
{
    long nDestLen = EZ_UNCOMPRESSMAXDESTLENGTH(len);
    *uncompressed = new char[nDestLen];
    int nErr = ezuncompress( reinterpret_cast<unsigned char*>(*uncompressed), &nDestLen,
                           reinterpret_cast<unsigned char*>(data), len );
    if ( nErr == EZ_BUF_ERROR ) {
      delete[] *uncompressed;
      *uncompressed = new char[nDestLen]; // enough room now
      nErr = ezuncompress( reinterpret_cast<unsigned char*>(*uncompressed), &nDestLen,
                           reinterpret_cast<unsigned char*>(data), len );
    }
    return (int)(nDestLen);
}

void StreamParser::startElement()
{


    int id = 0;
    if(xml.prefix().isEmpty()){
        id = insertPatricia(xml.name());
    } else {
        QString s = xml.prefix().toString()+":"+xml.name().toString();
        id = insertPatricia(s.toStdString());
    }
    tagIds.push(id);
    list<int> aux;
    aux.push_back(id);
    if(!xml.namespaceDeclarations().isEmpty()) {
        foreach(QXmlStreamNamespaceDeclaration nd, xml.namespaceDeclarations()){
            string attr = (nd.prefix().isEmpty()) ? "@xmlns" : "@xmlns:"+(nd.prefix().toString().toStdString());
            id = insertPatricia(attr);
            Container *c = getOrInsertContainer(id);
            int res  = c->addData(nd.namespaceUri());
            aux.push_back(id);
            if(res>0){
                this->contLengthBuffer += res;
                aux.push_back(-c->id);
            }
            aux.push_back(0);
        }
    }

    foreach(QXmlStreamAttribute a, xml.attributes()){
        string key = "@"+Util::trim(a.name());
        id = insertPatricia(key);
        Container* c = getOrInsertContainer(id);
        int res = c->addData(a.value());
        aux.push_back(id);
        if(res>0){
            this->contLengthBuffer += res;
            aux.push_back(-c->id);
        }
        aux.push_back(0);
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
    if(xml.isWhitespace()) return;
    int id = tagIds.top();
    Container* c = getOrInsertContainer(id);
    int res = c->addData(xml.text());
    if(res>0){
        this->contLengthBuffer += res;
        int aux[] = {-c->id};
        this->contLengthBuffer += structureContainer->addStructureData(aux, 1);
        if(this->contLengthBuffer>SIZE_TO_COMPRESS){
            compressContainers();
        }
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

    list<int> l;

    int xmlId = this->nextTagId++;
    patricia->insert("?xml", xmlId);
    this->structureContainer = new Container(ID_CONTAINER_STRUCTURE, TAG_MAP_CONTAINER_STRUCTURE);
    Container *xmlC = new Container(this->nextContainerID++, xmlId);
    (*containers)[TAG_MAP_CONTAINER_STRUCTURE] = this->structureContainer;
    (*containers)[xmlId] = xmlC;

    l.push_back(xmlId);

    QStringRef version = xml.documentVersion();
    QStringRef enconding = xml.documentEncoding();

    if(!version.isEmpty()){
         int verId = this->nextTagId++;
         patricia->insert("@version", verId);
         Container *verC = new Container(this->nextContainerID++, verId);
         verC->addData(this->xml.documentVersion());
         (*containers)[verId] = verC;
         this->contLengthBuffer += verC->size;
         l.push_back(verId);
         l.push_back(-verC->id);
         l.push_back(0);
    }

    if(!enconding.isEmpty()){
        int encId = this->nextTagId++;
        patricia->insert("@encoding", encId);
        Container *encC = new Container(this->nextContainerID++, encId);
        encC->addData(this->xml.documentEncoding());
        (*containers)[encId] = encC;
        this->contLengthBuffer += encC->size;
        l.push_back(encId);
        l.push_back(-encC->id);
        l.push_back(0);
    }

    if(xml.isStandaloneDocument()){
        int stanId = this->nextTagId++;
        patricia->insert("@standalone", stanId);
        Container *staC = new Container(this->nextContainerID++, stanId);
        staC->addData("yes");
        (*containers)[stanId] = staC;
        this->contLengthBuffer += staC->size;
        l.push_back(stanId);
        l.push_back(-staC->id);
        l.push_back(0);
    }
    l.push_back(0);
    structureContainer->addStructureData(l);
    this->contLengthBuffer += this->structureContainer->size + xmlC->size;

    if(this->contLengthBuffer>SIZE_TO_COMPRESS){
        compressContainers();
    }


}

void StreamParser::endDocument()
{
    if(this->contLengthBuffer>0){
        this->compressContainers();
    }
    out->seekp(0);
    this->end = true;
}

void StreamParser::invalid()
{
    qDebug() << xml.errorString();
    qDebug() << xml.lineNumber();
    qDebug() << xml.columnNumber();
    qDebug() << xml.characterOffset();
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
        aux = Util::convertIntToChar(*(+it), tAux);
        out->write(aux, tAux);
        *out << *it << "\n";
    }
    aux = Util::convertIntToChar(containers->size(), tAux);
    out->write(aux, tAux);

    for(unordered_map<int,Container*>::iterator it = containers->begin(); it != containers->end(); ++it){
        Container *c = *(&it->second);
        char *buffer = c->getData();
        char* compressed;
        int t = compress(&compressed, buffer, c->size);
        aux = Util::convertIntToChar(t, tAux);
        out->write(aux, tAux);
        aux = Util::convertIntToChar(c->id, tAux);
        out->write(aux, tAux);
        out->write(compressed, t);
    }

    out->flush();

    containers->clear();
    delete patricia;
    delete structureContainer;

    this->patricia = new Patricia<int>();
    this->nextContainerID = ID_CONTAINER_STRUCTURE+1;
    this->nextTagId = TAG_MAP_CONTAINER_STRUCTURE+1;
    this->contLengthBuffer = 0;
    this->structureContainer = new Container(ID_CONTAINER_STRUCTURE, TAG_MAP_CONTAINER_STRUCTURE);
    (*containers)[TAG_MAP_CONTAINER_STRUCTURE] = structureContainer;
}


