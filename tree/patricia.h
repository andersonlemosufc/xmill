#ifndef PATRICIA_H
#define PATRICIA_H

#include<string>
#include<stack>
#include<unordered_map>
#include<queue>
#include<vector>

template <class T>
struct Edge;


template <class T>
struct Nodo{

    bool isLeaf;
    T value;
    char *key;
    std::unordered_map<char, Edge<T>>* children;


    Nodo(){
        this->isLeaf = false;
        this->children = new std::unordered_map<char, Edge<T>>();
    }

    Nodo(const std::string &key, T value){
        this->isLeaf = true;
        this->value = value;
        int length = int(key.size());
        this->key = new char[length + 1];
        for (int i = 0;i < length; i++){
            this->key[i] = key[i];
        }
        this->key[length] = '\0';
    }

    ~Nodo (){
        if(this->isLeaf)
            delete[] this->key;
        else
            delete this->children;
    }
};

template <class T>
struct Edge{

    int ip;
    char *key;
    Nodo<T>* child;

    Edge(){}

    Edge(const int ip,Nodo<T>* child=nullptr){
        this->ip = ip;
        this->child = child;
    }
};

template <class T>
class Patricia
{

public:

    Patricia(){
        this->root = new Nodo<T>();
    }

    ~Patricia(){
        this->destroy(this->root);
    }

    bool insert(const std::string &key, T value){
        bool ok = this->insert(this->root, key, value, 0);
        if(ok) this->size++;
        return ok;
    }

    T get(const std::string &key){
        std::stack<Nodo<T>*> way;
        Nodo<T>* n = this->get(this->root,key,0,way);
        if(n==nullptr) return static_cast<T>(NULL);
        else return n->value;
    }

    bool remove(const std::string &key){
        std::stack<Nodo<T>*> way;
        Nodo<T>* nodo = this->get(this->root,key,0,way);

        if (nodo != nullptr) {
            Nodo<T> *father = way.top();
            way.pop();
            for (auto it = father->children->begin();
                 it != father->children->end(); it++) {
                if (it->second.child == nodo) {
                    delete nodo;
                    father->children->erase(it);
                    break;
                }
            }
            if (father != this->root) {
                Edge<T> edge = father->children->begin()->second;
                char * otherKey = edge.key;

                if (int(father->children->size()) == 1) {
                    nodo = father;
                    father = way.top();
                    way.pop();

                    for (auto it = father->children->begin();
                         it != father->children->end(); it++){
                        if (it->second.child == nodo) {
                            it->second = edge;
                            delete nodo;
                            break;
                        }
                    }
                }

                while (!way.empty()) {
                    nodo = father;
                    father = way.top();
                    way.pop();
                    for (auto it = father->children->begin();
                         it != father->children->end(); it++) {
                        if (it->second.child == nodo) {
                            it->second.key = otherKey;
                        }
                    }
                }
            }

            while (!way.empty()) {
                way.pop();
            }

            this->size--;
            return true;
        }
        return false;
    }

    std::vector<std::string> *listByPrefix(const std::string &key){
        if (key.empty()) return keys(this->root);

        std::queue<Nodo<T>*> q;
        q.push(this->root);
        int position = 0;
        while (!q.empty()) {
            Nodo<T> *nodo = q.front();
            q.pop();
            if (!nodo->isLeaf) {
                typename std::unordered_map<char, Edge<T>>::iterator it =
                        nodo->children->find(key[position]);

                if (it != nodo->children->end()) {
                    Edge<T>* edge = &it->second;

                    while (true){
                        if (position == int(key.size())+1)
                            return keys(edge->child);
                        if (position == edge->ip)
                            break;
                        if (key[position] != edge->key[position])
                            return nullptr;
                        position++;
                    }
                    q.push(edge->child);
                }
            } else {
                if (int(key.size()) == position)
                    return keys(nodo);
            }
        }
        return nullptr;
    }

    std::string largerPrefix(const std::string &key){
        std::queue<Nodo<T>*> q;
        q.push(this->root);
        int position = 0;
        while (!q.empty()) {
            Nodo<T> *nodo = q.front();
            q.pop();
            if (!nodo->isLeaf){
                typename std::unordered_map<char, Edge<T>>::iterator it =
                        nodo->children->find(key[position]);

                if (it != nodo->children->end()) {
                    Edge<T>* edge = &it->second;

                    while (true){
                        if (position == edge->ip)
                            break;
                        if (position == int(key.size())+1)
                            return "";
                        if (key[position] != edge->key[position])
                            return "";
                        position++;
                    }
                    q.push(edge->child);
                }
            } else {
                return nodo->key;
            }
        }
        return "";
    }

    std::vector<T>* elements(){
        std::vector<T> *res = new std::vector<T>();
        elements(this->root, res);
        return res;
    }

    std::vector<std::string>* keys(){
        return this->keys(this->root);
    }


    unsigned int getSize() const{
        return this->size;
    }

    struct iterator{

        std::stack<Nodo<T>*> way;
        std::stack<typename std::unordered_map<char,Edge<T>>::iterator> child;

        bool operator!=(iterator other){
            iterator me = *this;
            if(me.way.size() != other.way.size()) return true;
            while(!me.way.empty() && !other.way.empty()){
                if(me.way.top() != other.way.top()) return true;
                me.way.pop();other.way.pop();
            }
            return false;
        }

        char* operator*(){
            return this->way.top()->key;
        }

        T* operator+(){
            return &(this->way.top()->value);
        }


        iterator operator++(){
            if(this->way.top()->isLeaf) this->way.pop();
            while(!this->way.empty()){
                if(this->way.top()->isLeaf){
                    break;
                } else {
                    if(this->child.size()==this->way.size()){
                        this->child.top()++;
                        if(this->child.top() != this->way.top()->children->end()){
                            this->way.push(this->child.top()->second.child);
                        } else {
                            this->way.pop();
                            this->child.pop();
                        }
                    } else {
                        this->child.push(this->way.top()->children->begin());
                        this->way.push(this->child.top()->second.child);
                    }
                }
            }
            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;
                ++(*this);
                return temp;
        }
    };

    iterator begin(){
        iterator it;
        it.way.push(this->root);
        ++it;
        return it;
    }

    iterator end(){
        return iterator();
    }

private:

    Nodo<T>* root;
    unsigned int size{0};

    const char END = '$';

    bool insert(Nodo<T>*& nodo, const std::string &key, T value, int position){
        if (!nodo->isLeaf){
            auto it = nodo->children->find(key[position]);
            if (it != nodo->children->end()) {
                Edge<T>* edge = &it->second;

                while (true) {
                    if (position == int(key.size())+1) return false;
                    if (position == edge->ip)
                        return insert(edge->child, key, value, position);
                    if (key[position] != edge->key[position]) break;
                    position++;
                }

                Nodo<T> *newNodo = new Nodo<T>();
                Edge<T> newEdge(edge->ip, edge->child);
                newEdge.key = edge->key;

                (*newNodo->children)[edge->key[position]] = newEdge;
                edge->ip = position;
                edge->child = newNodo;

                return insert(newNodo, key, value, position);
            } else {
                Edge<T> edge(int(key.size())+1);
                edge.child = new Nodo<T>(key, value);
                edge.key = edge.child->key;
                (*nodo->children)[key[position]] = edge;
                return true;
            }
        }
        return false;
    }

    Nodo<T>* get(Nodo<T>*& nodo, const std::string &key, int position, std::stack<Nodo<T>*> &way){
        if (nodo->isLeaf){
            if (int(key.size())+1 == position){
                return nodo;
            }
        } else {
            way.push(nodo);
            typename std::unordered_map<char, Edge<T>>::iterator it =
                    nodo->children->find(key[position]);

            if (it != nodo->children->end()){
                Edge<T>* edge = &it->second;

                if (int(key.size())+1 < edge->ip) return nullptr;

                while (position < edge->ip){
                    if (key[position] != edge->key[position]) return nullptr;
                    position++;
                }
                return get(edge->child, key, position, way);
            }
        }
        return nullptr;
    }

    void destroy(Nodo<T> *nodo){
        if (!nodo->isLeaf) {
            for (typename std::unordered_map<char, Edge<T>>::iterator it =
                 nodo->children->begin();
                 it != nodo->children->end(); it++){
                destroy(it->second.child);
            }
        }
        delete nodo;
    }

    void elements(Nodo<T>* nodo, std::vector<T>* res){
        if(nodo->isLeaf){
            res->push_back(nodo->value);
        } else {
            for(typename std::unordered_map<char, Edge<T>>::iterator it =
                nodo->children->begin();
                it != nodo->children->end(); it++){
                elements(it->second.child,res);
            }
        }
    }

    std::vector<std::string> * keys(Nodo<T> *nodo){
        std::vector<std::string> *res = new std::vector<std::string>();
        std::queue<Nodo<T>*> q;
        q.push(nodo);
        while (!q.empty()){

            Nodo<T> *n = q.front();
            q.pop();
            if (n->isLeaf){
                res->push_back(n->key);
            } else {
                for (typename std::unordered_map<char, Edge<T>>::iterator it =
                    n->children->begin();
                    it != nodo->children->end(); it++) {
                    q.push(it->second.child);
                }
            }
        }
        return res;
    }
};

#endif // PATRICIA_H
