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

    /* armazena a próxima string do container em buffer. se o tamanho do buffer
    não for suficiente, o buffer é realocado. de qualquer forma, o valor de len
    é alterado para o tamanho da string */
    void getNextString(char **buffer, int *len);

private:
    int position;
    void duplicateArraySize(char **ch, int *len);

};

#endif // DECONTAINER_H
