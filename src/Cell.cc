/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#include "Model.h"
#ifndef NOGUI
#include "listWindow.h"
extern Glib::RefPtr<Gtk::Application> m;
#endif


Cell::Cell()
{
#ifdef DEBUG
    x = 99999;
    y = 99999;
#endif
    tree = NULL;
    thv = NULL;
    periLen = NULL;
    aglist = new List();
    preylist = new List();
    nAgents = 0;
    patch = NULL;
    int r = getRandInt(0, 1000);
    if(r > 500)
        zig = true;
    else
        zig = false;
}

void Cell::setX(int i, int j)
{
    x = i;
    y = j;
}

void Cell::setTHV(THV* th)
{
    thv = th;
}

void Cell::setTree(Tree* tr)
{
#ifdef DEBUG
    if(tree)
        FATALERR("Tree should be NULL!\n");
#endif
    tree = tr;
}

double Cell::getEatenBy(double e)
{
    if(tree)
        e = tree->getEatenBy(e);
    else
        if(thv)
            e = thv->getEatenBy(e);
        else
            return 0.0;
    return e;
}

void Cell::randomizePerimeter1()
{
    int i, r;
    Cell **cc = perimeter[1];
    Cell *c;
    if(zig){
        zig = false;
        for(i = 0; i < periLen[1]; i++){
            r = getRandInt(0, (periLen[1] - 1));
            c = cc[i];
            cc[i] = cc[r];
            cc[r] = c;
        }
    } else{
        zig = true;
        for(i = (periLen[1] - 1); i >= 0; i--){
            r = getRandInt(0, (periLen[1] - 1));
            c = cc[i];
            cc[i] = cc[r];
            cc[r] = c;
        }
    }
}

void Cell::randomizePerimeter2()
{
    int i, r, d;
    Cell **cc;
    Cell *c;
    for(d = 2; d <= MaxDistance; d++){
        cc = perimeter[d];
        if(zig){
            zig = false;
            for(i = 0; i < periLen[d]; i++){
                r = getRandInt(0, (periLen[d] - 1));
                c = cc[i];
                cc[i] = cc[r];
                cc[r] = c;
            }
        } else{
            zig = true;
            for(i = (periLen[d] - 1); i >= 0; i--){
                r = getRandInt(0, (periLen[d] - 1));
                c = cc[i];
                cc[i] = cc[r];
                cc[r] = c;
            }
        }
    }
}

void Cell::addAgent(Agent *ag)
{
    aglist->addLast(ag);
    nAgents++;
}

double Cell::getEnergy()
{
    if(tree)
        return tree->getEnergy();
    else
        if(thv)
            return thv->getEnergy();
        else
            return 0.0;
}


void Cell::removeAgent(Agent *ag)
{
#ifdef DEBUG
    if(aglist->remove(ag) == false)
        FATALERR(_("IMPOSSIBLE"));
#else
    aglist->remove(ag);
#endif
    nAgents--;
}

int Cell::getNAgents()
{
    return aglist->getCount();
}

void Cell::setPatch(Patch* p)
{
    patch = p;
}

Patch* Cell::getPatch()
{
    return patch;
}

List* Cell::getAgList()
{
    return aglist;
}

void Cell::drawSelfOn()
{
#ifndef NOGUI
    if(tree)
        tree->drawSelfOn();
    else
        if(thv)
            thv->drawSelfOn();
        else{
            WorldRaster->fillRectangle((x*5), (y*5), 5, 5, 0);
        }
    if(ShowAgents && aglist->getCount()){
        int c;
        if(ShowPopDensity){
            c = 29 + nAgents;
            if(c > 39)
                c = 39;
        } else{
            int nMales = 0;
            int nFemales = 0;
            aglist->begin();
            Agent *ag;
            while((ag = (Agent*)aglist->next()))
                if(ag->sex == 'f')
                    nFemales++;
                else
                    nMales++;
            aglist->end();
            c = (((9 * nMales) / (nFemales + nMales)) + 20);
        }
        WorldRaster->fillRectangle((x*5+1), (y*5+1), 3, 3, c);
    }
#endif
}

void Cell::mkProbeToAgents()
{
#ifndef NOGUI
    if(nAgents == 0)
        return;

    listWindow *listW = new class listWindow();

    aglist->begin();
    Agent *ag;
    while((ag = (Agent*)aglist->next()))
        listW->addAgent(ag);
    aglist->end();

    listW->show();
#endif
}

Cell::~Cell()
{
    int d;
    free(periLen);
    for(d = 1; d <= MaxDistance; d++)
        free(perimeter[d]);
    free(perimeter);
    delete(aglist);
    delete(preylist);
}

void Cell::drawPerimeter(int d)
{
#ifndef NOGUI
    int xa, xb, ya, yb;
    xa = x * 5 - ((d - 1) * 5);
    xb = x * 5 + (d * 5);
    ya = y * 5 - ((d - 1) * 5);
    yb = y * 5 + (d * 5);
    WorldRaster->drawLine(xa, ya, xa, yb, 250);
    WorldRaster->drawLine(xa, yb, xb, yb, 250);
    WorldRaster->drawLine(xb, yb, xb, ya, 250);
    WorldRaster->drawLine(xb, ya, xa, ya, 250);
#endif
}

void Cell::fillPerimeters()
{
#ifndef NOGUI
    Cell *c;
    for(int d = 1; d <= MaxDistance; d++){
        for(int i = 0; i < periLen[d]; i++){
            c = perimeter[d][i];
            WorldRaster->fillRectangle(5 * c->x, 5 * c->y, 5, 5, d);
            if(c->x < 0 || c->x >= WorldXSize)
                FATALERR("%d\n", c->x);
            if(c->y < 0 || c->y >= WorldYSize)
                FATALERR("%d\n", c->y);
        }
    }
#endif
}

