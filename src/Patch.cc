/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/


#include "Model.h"

extern Model *theModel;

Patch::Patch()
{
    nTrees = 0;
    maxSize = 20;
    xl = (int*)malloc(20 * sizeof(int));
    yl = (int*)malloc(20 * sizeof(int));
}

Patch::~Patch()
{
    free(xl);
    free(yl);
}

void Patch::optimizeMemory()
{
    xl = (int*)realloc(xl, (nTrees * sizeof(int)));
    yl = (int*)realloc(yl, (nTrees * sizeof(int)));
    int i;
    x = 0;
    y = 0;
    for(i = 0; i < nTrees; i++){
        x += xl[i];
        y += yl[i];
    }
    x = (int)(round((double)x / (double)nTrees));
    y = (int)(round((double)y / (double)nTrees));
}

void Patch::addX(int i, int j)
{
    if(nTrees == maxSize){
        maxSize *= 2;
        xl = (int*)realloc(xl, (maxSize * sizeof(int)));
        yl = (int*)realloc(yl, (maxSize * sizeof(int)));
    }
    xl[nTrees] = i;
    yl[nTrees] = j;
    nTrees++;
}

void Patch::setTreeSpecies(int s)
{
    trSpecies = s;
}

int Patch::getTreeSpecies()
{
    return trSpecies;
}

void Patch::receivePatch(Patch* p)
{
    int i, j, n2, *xl2, *yl2;
    n2 = p->getNTrees();
    xl2 = p->getXList();
    yl2 = p->getYList();
    xl = (int*)realloc(xl, (nTrees + n2) * sizeof(int));
    yl = (int*)realloc(yl, (nTrees + n2) * sizeof(int));
    j = 0;
    Cell *cell;
    for(i = nTrees; i < (nTrees + n2); i++){
        xl[i] = xl2[j];
        yl[i] = yl2[j];
        cell = cellGrid[xl2[j]][yl2[j]];
        cell->setPatch(this);
        j++;
    }
    nTrees += n2;
    maxSize = nTrees;
}

int* Patch::getXList()
{
    return xl;
}

int* Patch::getYList()
{
    return yl;
}

int Patch::getNTrees()
{
    return nTrees;
}

void Patch::getCenterX(int* i, int* j)
{
    *i = x;
    *j = y;
}

void Patch::printSelfOn(char myLetter)
{
    int i, mycolor;
    mycolor = 31 + trSpecies;
    for(i = 0; i < nTrees; i++){
        if(xl[i] == x && yl[i] == y) // highlight the center of the patch
            fprintf(stderr, "\033[%i;%iH\033[1;%dm%c\033[0m",
                    (yl[i]+2), (xl[i]+2), mycolor, myLetter);
        else
            fprintf(stderr, "\033[%i;%iH\033[%dm%c\033[0m",
                    (yl[i]+2), (xl[i]+2), mycolor, myLetter);
    }
}

