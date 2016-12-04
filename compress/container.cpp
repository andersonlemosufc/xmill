#include "container.h"

Container::Container()
{
    this->size = 0;
    this->id = this->pathId = 0;
    this->firstBlock = new Block();
    this->current = firstBlock;
}

Container::~Container()
{
    delete firstBlock;
}

Container::Container(int id, int pathId)
{
    this->size = 0;
    this->pathId = pathId;
    this->id = id;
    this->firstBlock = new Block();
    this->current = firstBlock;
}


int Container::addStructureData(int v[], int len)
{
    int l;
    int res = 0;
    for(int k=0;k<len;k++){
        //qDebug() << "vetor: " << v[k];
        char *aux = Util::convertIntToChar(v[k], l);
        for(int i=0;i<l;i++) {
            current = current->addData(aux[i]);
        }
        res += l;
    }
    this->size += res;
    return res;
}

int Container::addStructureData(std::list<int> list)
{
    int l;
    int res = 0;
    for(std::list<int>::iterator it=list.begin();it!=list.end();++it){
        //qDebug() << "lista: " << (*it);
        char *aux = Util::convertIntToChar((*it), l);
        for(int i=0;i<l;i++) {
            current = current->addData(aux[i]);
        }
        res += l;
    }
    this->size += res;
    return res;
}

int Container::addData(char *data, int len)
{
    int res = len;
    for(int k=0;k<len;k++){
        current = current->addData(data[k]);
        if(data[k]=='\\') {
            current = current->addData('\\');
            res++;
        }
    }
    current = current->addData('\\');
    this->size += res+1;
    return res;
}

int Container::addData(std::string data)
{
    return addData((char*)data.data(), data.length());
}


int Container::addData(QStringRef data)
{
    std::string s = Util::trim(data);
    return s.empty() ? 0 : addData(s);
}

char *Container::getData()
{
    char *res = new char[size];
    int k = 0;
    for(Block *b=firstBlock;b!=nullptr;b=b->next){
        for(int i=0;i<b->length;i++){
            res[k++] = b->data[i];
        }
    }
    return res;
}

