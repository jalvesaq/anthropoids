/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#ifndef PATCH_H
#define PATCH_H
#include "abmtools.h"

class Patch : public Thing
{
    private:
        int* getXList();
        int* getYList();
        int maxSize;	// number of plants in this patch

    public:
        int *xl, *yl; 	// list of coordinates of the plants
        int x, y;		// the patch center
        int trSpecies;	// species of tree in this patch
        int nTrees;

        Patch();
        ~Patch();
        void addX(int x, int y);
        void setTreeSpecies(int s);
        void receivePatch(Patch *p);
        void optimizeMemory();

        int getNTrees();
        int getTreeSpecies();
        void getCenterX(int *i, int *j);

        void printSelfOn(char myLetter);
};

#endif
