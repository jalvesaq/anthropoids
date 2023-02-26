/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/


#ifndef LISTWINDOW_H
#define LISTWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/listviewtext.h>
#include <gtkmm/scrolledwindow.h>

class Agent;

class listWindow: public Gtk::Window
{  

    public:
        listWindow();
        ~listWindow();
        void addAgent(Agent *a);

    private:
        class Gtk::ListViewText *agListView;
        class Gtk::ScrolledWindow *scrolledwindow1;
        void on_agListView_row_activated(const Gtk::TreeModel::Path& path,
                Gtk::TreeViewColumn* column);
        bool on_listWindow_delete_event(GdkEventAny *ev);
        Agent **ag;
        unsigned int max;
};
#endif
