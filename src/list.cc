
#include <stdio.h>
#include <stdlib.h>
#include "list.h"


List::List()
{
    n = 0;
    first = NULL;
    last = NULL;
    current = NULL;
}

List::~List()
{
    removeAll();
}

bool List::contains(Thing *obj)
{
    Item *tmp = first;
    while(tmp != NULL){
        if(tmp->obj == obj)
            return(true);
        tmp = tmp->next;
    }
    return false;
}

bool List::remove(Thing *obj)
{
    Item *tmp = first;
    while(tmp != NULL){
        if(tmp->obj == obj){
            if(tmp->prev == NULL)
                first = tmp->next;
            else
                tmp->prev->next = tmp->next;
            if(tmp->next == NULL)
                last = tmp->prev;
            else
                tmp->next->prev = tmp->prev;
            if(tmp == current)
                current = current->next;
            free(tmp);
            n--;
#ifdef DEBUG
            if(n == 0 && (first || last || current))
                FATALERR("Impossible!");
            if(n == 1 && (first != last))
                FATALERR("Impossible!");
            if(n > 0 && (first == NULL || last == NULL))
                FATALERR("Impossible!");
#endif
            return true;
        }
        tmp = tmp->next;
    }
    return false;
}

bool List::addFirst(Thing *obj)
{
#ifdef DEBUG
    if(contains(obj)){
        fprintf(stderr, "ERROR: list alread has object. (%s, %d)\n", __FILE__, __LINE__);
        exit(1);
    }
#else
    if(contains(obj))
        return false;
#endif
    Item *tmp = (Item*)calloc(1, sizeof(Item));
    tmp->obj = obj;
    if(first){
        tmp->next = first;
        first->prev = tmp;
    }
    first = tmp;
    if(n == 0)
        last = first;
    n++;
    return true;
}

bool List::addLast(Thing *obj)
{
#ifdef DEBUG
    if(contains(obj)){
        fprintf(stderr, "ERROR: list alread has object. (%s, %d)\n", __FILE__, __LINE__);
        exit(1);
    }
#else
    if(contains(obj))
        return false;
#endif
    Item *tmp = (Item*)calloc(1, sizeof(Item));
    if(last){
        last->next = tmp;
        tmp->prev = last;
    }
    last = tmp;
    last->obj = obj;
    if(n == 0)
        first = last;
    n++;
    return true;
}

Thing* List::getFirst()
{
    if(first)
        return first->obj;
    else
        return NULL;
}

void List::removeAll()
{
#ifdef DEBUG
    if(current)
        FATALERR("Impossible!");
    int contagem = n;
#endif
    Item *tmp = first;
    while(tmp != NULL){
#ifdef DEBUG
        contagem--;
#endif
        tmp = tmp->next;
        free(first);
        first = tmp;
    }
    last = NULL;
    n = 0;
#ifdef DEBUG
    if(contagem != 0){
        fprintf(stderr, "contagem = %d (%s : %d)\n", contagem, __FILE__, __LINE__);
        exit(1);
    }
#endif
}

void List::deleteAll()
{
    Item *tmp = first;
    Thing *th;
    while(tmp != NULL){
        th = (Thing*)(tmp->obj);
        tmp = tmp->next;
        delete(th);
    }
    removeAll();
}

void List::stepAll()
{
    Eukaryote *ek;
    begin();
    while((ek = (Eukaryote*)next()))
        ek->step();
    end();
}

int List::getCount()
{
    return n;
}

void List::begin()
{
#ifdef DEBUG
    if(current){
        fprintf(stderr, "ERROR! List::begin() called twice: current != NULL\n");
        exit(1);
    }
#endif
    current = first;
}

Thing* List::next()
{
    if(current == NULL)
        return NULL;
    Item *tmp = current;
    current = current->next;
    return tmp->obj;
}

void List::end()
{
    current = NULL;
}

void List::shuffleWholeList()
{
    if(n < 3)
        return;
    Item *tmp = first;
    Item **array = (Item**)malloc(n * sizeof(Item*));
    int i = 0;
    while(tmp != NULL){
        array[i] = tmp;
        tmp = tmp->next;
        i++;
    }
#ifdef DEBUG
    if(i != n)
        FATALERR("Impossible!");
#endif
    int r, max = n - 1;
    while(i > 0){
        i--;
        r = getRandInt(0, max);
        if(i != r){
            tmp = array[i];
            array[i] = array[r];
            array[r] = tmp;
        }
    }
    first = array[0];
    array[0]->prev = NULL;
    array[0]->next = array[1];
    i = 1;
    for(i = 1; i < max; i++){
        array[i]->prev = array[i-1];
        array[i]->next = array[i+1];
    }
    array[max]->next = NULL;
    array[max]->prev = array[max-1];
    last = array[max];
    free(array);
#ifdef DEBUG
    tmp = first;
    r = 0;
    while(tmp != NULL){
        if(tmp->next && tmp->next->prev != tmp){
            printf("erro next\n");
            exit(1);
        }
        if(tmp->prev && tmp->prev->next != tmp){
            printf("erro prev\n");
            exit(1);
        }
        r++;
        tmp = tmp->next;
    }
#endif
}

