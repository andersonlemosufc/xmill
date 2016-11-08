#include "decontainer.h"

Decontainer::Decontainer()
{
    this->id = this->size = 0;
    this->data = nullptr;
}

Decontainer::Decontainer(int id, int size)
{
    this->id = id;
    this->size = size;
    this->data = nullptr;
}

Decontainer::Decontainer(int id, int size, char* data)
{
    this->id = id;
    this->size = size;
    this->data = data;
}
