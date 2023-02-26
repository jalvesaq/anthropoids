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

Prey::Prey()
{
    age = 1;
    maxAge = getRandInt(50, 60);
}

Prey::~Prey()
{
    myCell->preylist->remove(this);
#ifdef DEBUG
    if(myCell->preylist->getFirst() == this)
        FATALERR("Impossible!");
#endif
    preyList->remove(this);
    NPreys--;
}

void Prey::step()
{
    age++;

    int i = getRandInt(0, (myCell->periLen[1] - 1));
    int x1 = myCell->perimeter[1][i]->x;
    int y1 = myCell->perimeter[1][i]->y;
#ifdef DEBUG
    if((x > x1 && (x - x1) != 1) || (x < x1 && (x1 - x) != 1))
        FATALERR("%d %d", x, x1);
    if((y > y1 && (y - y1) != 1) || (y < y1 && (y1 - y) != 1))
        FATALERR("%d %d", y, y1);
    if(x1 == x && y1 == y)
        FATALERR(_("IMPOSSIBLE"));
    if(x1 > WorldXSize)
        FATALERR("Impossible!");
    if(y1 > WorldYSize)
        FATALERR("Impossible!");
    if(x > WorldXSize)
        FATALERR("Impossible!");
    if(y > WorldYSize)
        FATALERR("Impossible!");
#endif
    bool found = myCell->preylist->remove(this);
    if(!found)
        fprintf(stderr, "NOT FOUND IN CELL %s %d\n", __FILE__, __LINE__);
    myCell = cellGrid[x1][y1];
    myCell->preylist->addLast(this);
    x = x1;
    y = y1;

    if(age >= maxAge)
        reproduce();
}

void Prey::reproduce()
{
    age = 1;
    if(NPreys < MaxNPreys){
        Prey *p = new(Prey);
        p->x = getRandInt(0, (WorldXSize - 1));
        p->y = getRandInt(0, (WorldYSize - 1));
        p->myCell = cellGrid[p->x][p->y];
        p->myCell->preylist->addFirst(p);
        preyList->addLast(p);
        NPreys++;
    }
}

double Prey::getHuntedBy(double n)
{ // Thanks to Francisco J. Alves de Aquino for finding this equation
    double p = 1.0 - pow(2.0, (((-1.0) * n * 8.0) / (double)age));
    p -= p * (1.0 / (3.0 * n));
    double r = getRandDouble(0.0, 1.0);
    if(r < p)
        return (double)age;
    else
        return 0.0;
}

void Prey::drawSelfOn()
{
#ifndef NOGUI
    WorldRaster->fillRectangle((x*5+2), (y*5+2), 1, 1, 0);
#endif
}


