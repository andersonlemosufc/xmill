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

    std::unordered_map<int,char*> map;
    stack<Tag*> tags;
    bool incompleteTag = false;
    bool incompleteAttributte = false;
    ifstream in(filename.toStdString(), ifstream::binary);
    ofstream out(Util::getDecompressedFilename(filename));
    char *buffer = new char[BUFFER_SIZE];


    //essa primeira leitura é usada para corrigir o problema do eof
    char first;
    in.read(&first, 1);
    while(!in.eof()){
        map.clear();
        int tam = Util::nextInt(first, &in);
        for(int k=0;k<tam;k++){
            int id = Util::nextInt(&in);
            in.getline(buffer, BUFFER_SIZE);
            int t = in.gcount()-1;
            char *valor = new char[t+1];
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

        int k=0;
        while(k<structure->size){
            int t = Util::nextInt(structure->data, &k);
            if(t>0){
                char *tagName = map.at(t);
                Tag *tag = new Tag();
                tag->name = tagName;
                tag->hasContent = false;
                if(incompleteTag && tagName[0]!='@'){
                    incompleteTag = false;
                    out << ">\n";
                    tags.top()->hasContent = true;
                }
                switch(tagName[0]){
                    case '!':
                        if(tagName[1]=='-') {
                            tag->type = Tag::COMMENT;
                            for(int k=0,tt=tags.size();k<tt;k++) out << "\t";
                            out << "<!-- ";
                        } else {
                            tag->type = Tag::DTD;
                        }
                        break;
                    case '?':
                        tag->type = Tag::START_DOCUMENT;
                        out << "<" << tagName;
                        break;
                    case '@':
                        tag->type = Tag::ATTRIBUTE;
                        out << " " << (tagName+1);
                        incompleteAttributte = true;
                        break;
                    default:
                        tag->type = Tag::NORMAL;
                        incompleteTag = true;
                        for(int k=0,tt=tags.size();k<tt;k++) out << "\t";
                        out << "<" << tagName;
                }
                tags.push(tag);
            } else if(t<0) {
                Tag *tag = tags.top();
                tag->hasContent = true;
                Decontainer *c = containers.at(-t);
                int len = BUFFER_SIZE;
                c->getNextString(&buffer, &len);
                if(incompleteAttributte){
                    incompleteAttributte = false;
                    out << "=\"";
                    out.write(buffer, len);
                    out << "\"";
                } else {
                    if(incompleteTag){
                        incompleteTag = false;
                        out << ">\n";
                    }
                    int tt=tags.size();

                    if(tag->type!=Tag::COMMENT){
                        if(tag->type==Tag::DTD) tt--;
                        for(int k=0;k<tt;k++) out << "\t";
                    }

                    out.write(buffer, len);
                    if(tag->type==Tag::DTD || tag->type==Tag::NORMAL) out << "\n";
                }
            } else {
                Tag *tag = tags.top();
                tags.pop();
                if(tag->hasContent){
                    if(incompleteTag && tag->type!=Tag::ATTRIBUTE){
                        incompleteTag = false;
                        out << ">\n";
                    }
                }
                switch(tag->type){
                    case Tag::ATTRIBUTE: break;
                    case Tag::DTD: break;
                    case Tag::START_DOCUMENT: out << "?>\n"; break;
                    case Tag::COMMENT: out << " -->\n"; break;
                    case Tag::NORMAL:
                        if(tag->hasContent){
                            for(int i=0, tt = tags.size();i<tt;i++) out << "\t";
                            out << "</" << (*tag).name << ">\n";
                        } else {
                            incompleteTag = false;
                            out << "/>\n";
                        }
                        break;
                }
            }
        }
        out.flush();
        in.read(&first, 1);
         //PATRICIA

       /*  for(unordered_map<int,char*>::iterator it = map.begin();
            it!=map.end();++it){

            int id = *(&it->first);
            char *c = *(&it->second);

            cout << id << " " << c << endl;

        }*/
/*
        for(unordered_map<int,Decontainer*>::iterator it=containers.begin();
            it!=containers.end();++it){

            int id = *(&it->first);
            Decontainer *c = it->second;

            cout << id << " " << c->size << " " << c->data << endl;
        }*/
        //break;
    }

    in.close();
    out.close();
}



