/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/


#include <gtkmmconfig.h>
#include "listWindow.h"
#include "Agent.h"


listWindow::listWindow()// : Gtk::Window(Gtk::WINDOW_TOPLEVEL)
{  
    agListView = Gtk::manage(new class Gtk::ListViewText(3, false, Gtk::SELECTION_SINGLE));
    scrolledwindow1 = Gtk::manage(new class Gtk::ScrolledWindow());
    agListView->set_can_focus(true);
    agListView->set_headers_visible(true);
    agListView->set_reorderable(false);
    agListView->set_enable_search(true);
    scrolledwindow1->set_can_focus(true);
    scrolledwindow1->set_shadow_type(Gtk::SHADOW_IN);
    scrolledwindow1->set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);
    scrolledwindow1->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
    scrolledwindow1->add(*agListView);

    set_default_size(400, 200);
    set_title(_("Agent List"));
    set_modal(true);
    property_window_position().set_value(Gtk::WIN_POS_NONE);
    set_resizable(true);
    property_destroy_with_parent().set_value(false);
    add(*scrolledwindow1);
    agListView->show();
    scrolledwindow1->show();
    show();

    agListView->set_column_title(0, _("Age"));
    agListView->set_column_title(1, _("Sex"));
    agListView->set_column_title(2, _("Energy"));

    agListView->signal_row_activated().connect(sigc::mem_fun(this, &listWindow::on_agListView_row_activated), false);
    signal_delete_event().connect(sigc::mem_fun(*this, &listWindow::on_listWindow_delete_event), false);
    max = 3;
    ag = (Agent**)malloc(3 * sizeof(void*));
}

listWindow::~listWindow()
{
    free(ag);
}

void listWindow::on_agListView_row_activated(
        __attribute__((unused)) const Gtk::TreeModel::Path& pth,
        __attribute__((unused)) Gtk::TreeViewColumn* column)
{
    int i = agListView->get_selected()[0];
    ag[i]->followMe();
    printf("%d\n", i);
    hide();
    delete(this);
}

bool listWindow::on_listWindow_delete_event(__attribute__((unused)) GdkEventAny *ev)
{
    hide();
    return true;
}

void listWindow::addAgent(Agent *a)
{
    guint row_number = agListView->append();

    if(row_number >= max){
        max += 3;
        ag = (Agent**)realloc(ag, max * sizeof(void*));
    }
    ag[row_number] = a;

    char s[64];
    sprintf(s, "%d", a->age);
    agListView->set_text(row_number, 0, s);
    sprintf(s, "%c", a->sex);
    agListView->set_text(row_number, 1, s);
    sprintf(s, "%f", a->energy);
    agListView->set_text(row_number, 2, s);
}


