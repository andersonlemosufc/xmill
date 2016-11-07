#include "xmill.h"

using namespace std;

XMill::XMill()
{

}

void XMill::compress(QString filename)
{
    this->patriciaTags = new Patricia<int>();
    StreamParser *parser = new StreamParser(filename, this->patriciaTags, &containers);
    while(!parser->isEnd()){
        if(!parser->treatNextTag()) return;

    }
    /*std::cout << "Patricia: --------------------->\n";

    std::vector<std::string>* keys = patriciaTags->keys();
    for(int k=0;k<keys->size();k++){
        std::string s = (*keys)[k];
        std::cout << "\tkey: " << s << " - valor: " << patriciaTags->get(s) << std::endl;
    }

    std::cout << "------------------------------\n";
    for(std::unordered_map<int,Container*>::iterator it=containers.begin();
        it!=containers.end();it++){
        int id = *(&it->first);
        Container *c = *(&it->second);
        std::cout << "ID: " << c->id << " tag: " << id << std::endl;
        Block* b = c->firstBlock;
        while(b!=nullptr){
            for(int i=0;i<b->length;i++){
                std::cout << b->data[i];
            }
            b = b->next;
        }
        std::cout << "\n===================\n";
    }*/
}

