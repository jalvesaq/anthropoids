#ifndef CONTROL_WINDOW_HH
#define CONTROL_WINDOW_HH

#include <gtkmm/window.h>
#include <gtkmm/tooltip.h>
#include <gtkmm/toolbutton.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/box.h>
#include <gtkmm/main.h>

class Agent;
class ZoomRaster;
class Observer;

class Control : public Gtk::Window
{  
    public:
        Control(Observer *obs, int xsize, int ysize);
        ~Control();
        ZoomRaster *getZoomRaster();
        void setNewSize(int zoom);
        void update();

    private:
        Gtk::ToolButton *btRun;
        Gtk::ToolButton *btStep;
        Gtk::ToolButton *btView;
        Gtk::ToolButton *btGraph;
        Gtk::ToolButton *btPict;
        Gtk::ToolButton *btQuit;
        Gtk::Toolbar *toolbar1;
        Gtk::VBox *vbox1;
        ZoomRaster *drWorld;
        Observer *observer;
        bool running;
        bool updateWorld;

        void on_btRun_clicked();
        void on_btStep_clicked();
        void on_btView_clicked();
        void on_btGraph_clicked();
        void on_btPict_clicked();
        void on_btQuit_clicked();
        bool on_delete(GdkEventAny *ev);
};
#endif
