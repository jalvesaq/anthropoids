/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#ifndef TREE_H
#define TREE_H

#include "abmtools.h"

class Tree : public Eukaryote
{
    private:
        double fruitEnergy;	// Amount of energy in each fruit
        int fruitMaxAge,	// Duration of fruits (days)
            myspecies,	// The species of this tree
            nFruitsDay,	// Number produced each day during the fruit season
            today,		// The current day in the circular list of fruits
            seasonBegin,	// Day of year where the fruit season begins
            seasonEnd;	// Day of year where the fruit season ends


    public:
        int x, y;
        int nf; 		// Number of fruits
        int *fruits;	// Circular list of fruits at each fruit age
        double drawFactor;	// Number used to calculate the color
        int myColor;	// A color indicating the tree species

        ~Tree();
        void set(int newX, int newY, int s, int b, int e, int a, int n, double f);

        void step();
        double getEatenBy(double e);
        double getEnergy();
        void drawSelfOn();
};

#endif
