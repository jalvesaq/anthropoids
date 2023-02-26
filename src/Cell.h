/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#ifndef CELL_H
#define CELL_H

#include "Tree.h"
#include "THV.h"
#include "Patch.h"
#include "list.h"

class Agent;

class Cell : public Thing
{
    private:
        List* getAgList();
        bool zig; // Shuffling in only in one direction doesn't assure enough randomization.

    public:
        int x, y, nAgents;
        Tree *tree;
        THV *thv;
        Cell ***perimeter;
        int *periLen;
        Patch *patch;
        List *aglist;
        List *preylist;

        Cell();
        ~Cell();
        void setX(int i, int j);
        void setTHV(THV *th);
        void setTree(Tree *tr);
        void setPatch(Patch *p);
        Patch* getPatch();
        void randomizePerimeter1();
        void randomizePerimeter2();

        void addAgent(Agent *ag);
        void removeAgent(Agent *ag);
        int getNAgents();
        double getEnergy();
        double getEatenBy(double e);

        void drawSelfOn();
        void drawPerimeter(int d);
        void fillPerimeters();

        void mkProbeToAgents();
};

#endif
