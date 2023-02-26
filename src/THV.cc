/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/



#include "Model.h"


THV::THV()
{
    maxEnergy = getRandDouble((MaxPlantEnergy / 2), MaxPlantEnergy);
    energy = getRandDouble(PlantLogisticGrowth, maxEnergy);
    enoughEnergy = 0.98 * maxEnergy;
}

void THV::setX(int inX, int inY, double e)
{
    x = inX;
    y = inY;
    maxEnergy = e;
    energy = maxEnergy;
}

double THV::getEnergy()
{
    return energy - PlantLogisticGrowth;
}

// logistic growth rate borrowed from Luke Premo's model.
void THV::step()
{
    if(energy < enoughEnergy)
        energy += (PlantLogisticGrowth * energy * ((maxEnergy - energy) / maxEnergy));
}

double THV::getEatenBy(double e)
{
    if((energy - e) < (PlantLogisticGrowth ))
        e = energy - PlantLogisticGrowth - 0.0001; // 0.0001 to avoid rounding problems
    if(e < 0.0)
        return 0.0;
    energy -= e;
#ifdef DEBUG
    if(energy < PlantLogisticGrowth)
        FATALERR("%f %f %f", e, energy, PlantLogisticGrowth);
#endif
    return e;
}


void THV::drawSelfOn()
{
#ifndef NOGUI
    int color = (int)(round(9 * energy / MaxPlantEnergy) + 1);
    WorldRaster->fillRectangle((x*5), (y*5), 5, 5,  color);
#endif
}


