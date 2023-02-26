#ifndef LIST_H
#define LIST_H

#include "abmtools.h"

typedef struct ItemT
{
    Thing *obj;
    ItemT *next;
    ItemT *prev;
} Item;

class List
{
    int n;
    Item *first;
    Item *last;
    Item *current;

    public:
    List();
    ~List();
    bool contains(Thing *obj);
    bool remove(Thing *obj);
    bool addFirst(Thing *obj);
    bool addLast(Thing *obj);
    Thing *getFirst();
    void removeAll();
    void deleteAll();
    void stepAll();
    int getCount();
    void begin();
    void end();
    Thing *next();
    void shuffleWholeList();
    void Zero();
};

#endif
