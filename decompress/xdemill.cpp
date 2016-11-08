#include "xdemill.h"

using namespace std;

XDemill::XDemill()
{

}

int XDemill::uncompress(char **uncompressed, char *data, long len)
{
    long nDestLen = EZ_UNCOMPRESSMAXDESTLENGTH(len);
    *uncompressed = new char[nDestLen];
    int nErr = ezuncompress( reinterpret_cast<unsigned char*>(*uncompressed), &nDestLen,
                           reinterpret_cast<unsigned char*>(data), len );
    if ( nErr == EZ_BUF_ERROR ) {
        cout << "erro" << endl;
      delete[] *uncompressed;
      *uncompressed = new char[nDestLen]; // enough room now
      nErr = ezuncompress( reinterpret_cast<unsigned char*>(*uncompressed), &nDestLen,
                           reinterpret_cast<unsigned char*>(data), len );
    }
    return (int)(nDestLen);
}


void XDemill::decompress(QString filename)
{

    map.clear();
    ifstream in(filename.toStdString(), ifstream::binary);
    ofstream out(Util::getDecompressedFilename(filename));
    char *buffer = new char[BUFFER_SIZE];

    while(!in.eof()){
        int tam = Util::nextInt(&in);
        for(int k=0;k<tam;k++){
            int id = Util::nextInt(&in);
            in.getline(buffer, BUFFER_SIZE);
            int t = in.gcount()-1;
            char *valor = new char[t];
            for(int i=0;i<t;i++) valor[i] = buffer[i];
            valor[t] = '\0';
            map[id] = valor;
        }
        int numberOfContainers = Util::nextInt(&in);
        unordered_map<int,Decontainer*> containers;

        for(int k=0;k<numberOfContainers;k++){
            int t = Util::nextInt(&in);
            int id = Util::nextInt(&in);
            char *compressed = new char[t];
            in.read(compressed, t);
            char *uncompressed;
            int size = uncompress(&uncompressed, compressed, t);
            Decontainer *c = new Decontainer(id, size, uncompressed);
            containers[id] = c;
        }

        Decontainer *structure = containers.at(ID_CONTAINER_STRUCTURE);
        containers.erase(ID_CONTAINER_STRUCTURE);

        stack<Tag> tags;

        for(int k=0;k<structure->size;k++){
            int t = Util::nextInt(structure->data, &k);
            if(t==0){
                Tag tag = tags.top();
                tags.pop();
                switch(tag.type){
                    case Tag::DTD: break;
                    case Tag::START_DOCUMENT: out << "?>\n"; break;
                    case Tag::COMMENT: out << " -->"; break;
                    case Tag::NORMAL:
                        for(int i=0;i<tags.size();i++) out << "\t";
                        out << "</" << map.at(tag.id) << ">\n";
                        break;
                }
            } else if(t<0) {

            }
        }

         /*//PATRICIA

         for(unordered_map<int,char*>::iterator it = map.begin();
            it!=map.end();++it){

            int id = *(&it->first);
            char *c = *(&it->second);

            cout << id << " " << c << endl;

        }

        for(unordered_map<int,Decontainer*>::iterator it=containers.begin();
            it!=containers.end();++it){

            int id = *(&it->first);
            Decontainer *c = it->second;

            cout << id << " " << c->size << " " << c->data << endl;
        }
        break;*/
    }
}



