/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#include <gtkmmconfig.h>
#include "Observer.h"
#include "Model.h"
#include "zoomraster.h"


extern Model *theModel;
ZoomRaster *WorldRaster;


Observer::Observer(Glib::RefPtr<Gtk::Application> mn)
{
    showWorld = false;
    m = mn;
    model = new Model();
    theModel = model;
    model->setObserver(this);
    control = new Control(this, WorldXSize, WorldYSize);
    zr = control->getZoomRaster();
    WorldRaster = zr;
    showWorld = true;
}

Observer::~Observer()
{
    if(model)
        delete(model);
    model = NULL;
    delete control;

}

void Observer::go()
{
    m->run(*control);
}

void Observer::setShowWorld(bool s)
{
    showWorld = s;
}

void Observer::cellClicked(int button, int x, int y)
{
    Cell *c = cellGrid[x][y];
    if(button == 1){
        if(c->getNAgents() > 0){
            int oldStatus = model->getStatus();
            model->setStatus(1);
            c->mkProbeToAgents();
            if(oldStatus == 1){
                drawWorld();
            } else {
                model->setStatus(oldStatus);
                model->go();
            }
        }
    } else{
        if(button == 2){
            c->fillPerimeters();
        }
    }
}

void Observer::drawElements()
{
    if(!showWorld)
        return;

    Cell *c;
    for(int i = 0; i < WorldXSize; i++){
        for(int j = 0; j < WorldYSize; j++){
            c = cellGrid[i][j];
            c->drawSelfOn();
        }
    }
    if(WhiteAgent)
        WhiteAgent->drawSelfOn();
    if(preyList && ShowPreys){
        preyList->begin();
        Prey *p;
        while((p = (Prey*)preyList->next()))
            p->drawSelfOn();
        preyList->end();
    }
}

void Observer::drawWorld()
{
    drawElements();
}

void Observer::saveImg()
{
    drawElements();
    zr->savePNG();
}

void Observer::update()
{
    if(showWorld)
        drawWorld();
    control->update();
}

void Observer::tooglePreyShow()
{
    if(ShowPreys == true)
        ShowPreys = false;
    else
        ShowPreys = true;
    if(!showWorld)
        return;
    drawWorld();
}

void Observer::showPopDensity()
{
    ShowPopDensity = !ShowPopDensity;
    if(!showWorld)
        return;
    drawWorld();
}

void Observer::toogleAgShow()
{
    ShowAgents = !ShowAgents;
    if(!showWorld)
        return;
    drawWorld();
}

