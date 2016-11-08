#include "decontainer.h"

Decontainer::Decontainer()
{
    this->id = this->size = this->position = 0;
    this->data = nullptr;
}

Decontainer::Decontainer(int id, int size)
{
    this->position = 0;
    this->id = id;
    this->size = size;
    this->data = nullptr;
}

Decontainer::Decontainer(int id, int size, char* data)
{
    this->position = 0;
    this->id = id;
    this->size = size;
    this->data = data;
}

void Decontainer::getNextString(char **buffer, int *len)
{
    *buffer = new char[*len];
    int k = 0;
    while(position<size){
        char ch = data[position++];
        if(ch!='\\') (*buffer)[k++] = ch;
        else {
            if(position==size || data[position]!='\\'){
                break;
            } else {
                if(k==(*len)) duplicateArraySize(buffer, len);
                (*buffer)[k++] = data[position++];
            }
        }
        if(k==(*len)) duplicateArraySize(buffer, len);
    }
    (*len) = k;
}

void Decontainer::duplicateArraySize(char **ch, int *len)
{
    int tam = *len;
    char *res = new char[tam*2];
    for(int k=0;k<tam;k++){
        res[k] = (*ch)[k];
    }
    delete[] (*ch);
    *ch = res;
    (*len)*=2;
}
