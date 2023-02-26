/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#ifndef OBSERVER_H
#define OBSERVER_H

#include <gtkmm/main.h>
#include <gtkmm/window.h>

class Control;
class Model;
class ZoomRaster;

class Observer
{
    private:
        Control *control;
        Model *model;
        ZoomRaster *zr;
        Glib::RefPtr<Gtk::Application> m;
        bool showWorld;
        void regionRasterDeath();
        void toogleAgShow();
        void tooglePreyShow();
        void drawElements();

    public:
        Observer(Glib::RefPtr<Gtk::Application> mn);
        ~Observer();
        void go();
        void setShowWorld(bool s);

        void update();
        void cellClicked(int button, int x, int y);
        void showPopDensity();
        void drawWorld();
        void saveImg();
};

#endif
