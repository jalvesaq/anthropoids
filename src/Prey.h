/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#ifndef PREY_H
#define PREY_H

#include "Cell.h"

class Prey : public Eukaryote
{
    private:
        void reproduce();

    public:
        int x, y, age, maxAge;
        Cell *myCell;

        Prey();
        ~Prey();
        void step();
        double getHuntedBy(double n);
        void drawSelfOn();
};

#endif
