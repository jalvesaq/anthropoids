/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#ifndef THV_H
#define THV_H

#include "abmtools.h"

class THV : public Eukaryote
{
    private:
        int x, y;
        double maxEnergy;
        double enoughEnergy;

    public:
        double energy;

        THV();

        void setX(int theX, int theY, double e);

        void step();
        double getEatenBy(double e);
        double getEnergy();

        void drawSelfOn();
};

#endif
