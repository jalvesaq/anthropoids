/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/


#include "Model.h"


Tree::~Tree()
{
    free(fruits);
}

void Tree::set(int newX, int newY, int s, int b, int e, int a, int n, double f)
{
    x = newX;
    y = newY;
    myspecies = s;
    if(myspecies == 0)
        myColor = 200;
    else
        if(myspecies == 1)
            myColor = 201;
        else
            myColor = 202;
    today = 0;
    nf = 0;
    seasonBegin = b;
    seasonEnd = e;
    fruitEnergy = f;
    fruitMaxAge = a;
    nFruitsDay = n;
    fruits = (int*)calloc(fruitMaxAge, sizeof(int));
    if(a == 0 || n == 0)
        drawFactor = 0.0;
    else
        drawFactor = 19.0 / ((double)(a * n));
}


void Tree::step()
{
#ifdef DEBUG
    int i, l, n;
    l = fruitMaxAge;
    n = 0;
    for(i = 0; i < l; i++){
        n += fruits[i];
    }
    if(n != nf)
        FATALERR("(%d != %d)\n", n, nf);
#endif

    // Drop rotten fruits
    today++;
    if(today == fruitMaxAge)
        today = 0;
    nf -= fruits[today];
    fruits[today] = 0;

    // Produce new fruits
    if(Day >= seasonBegin && Day <= seasonEnd){
        fruits[today] = nFruitsDay;
        nf += nFruitsDay;
    }
}

// Assume that the more a fruit is ripe the more it's preferred: the oldest
// fruits are eaten first.
double Tree::getEatenBy(double  e)
{
    int n = (int)(round(0.5 + (e / fruitEnergy)));
    e = n * fruitEnergy;
    int oldest = today + 1;
    if(oldest == fruitMaxAge)
        oldest = 0;
    int eaten = 0, noteaten = n;
    while(eaten < n && nf > 0){
        if(fruits[oldest] >= noteaten){
            eaten += noteaten;
            n -= noteaten;
            fruits[oldest] -= noteaten;
            nf -= noteaten;
        } else{
            eaten += fruits[oldest];
            noteaten -= fruits[oldest];
            nf -= fruits[oldest];
            fruits[oldest] = 0;
            oldest++;
            if(oldest == fruitMaxAge)
                oldest = 0;
        }
    }
    return (eaten * fruitEnergy);
}

double Tree::getEnergy()
{
    return nf * fruitEnergy;
}

void Tree::drawSelfOn()
{
#ifndef NOGUI
    WorldRaster->fillRectangle((x*5), (y*5), 5, 5, myColor);
    int color = (int)(drawFactor * nf + 100);
    WorldRaster->fillRectangle((x*5+1), (y*5+1), 3, 3, color);
#endif
}


