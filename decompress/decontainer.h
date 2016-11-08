#ifndef DECONTAINER_H
#define DECONTAINER_H


class Decontainer
{
public:
    Decontainer();
    Decontainer(int id, int size);
    Decontainer(int id, int size, char* data);

    int id;
    int size;
    char *data;

};

#endif // DECONTAINER_H
