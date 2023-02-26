
#ifndef ZOOMRASTER_HH
#define ZOOMRASTER_HH

#include <gtkmm/window.h>
#include <gtkmm/drawingarea.h>
#include <gdkmm/rectangle.h>

class Observer;
class Control;

class ZoomRaster : public Gtk::DrawingArea
{
    public:
        ZoomRaster(Control *cntrl, Observer *obs, int xsize, int ysize);
        ~ZoomRaster();
        int get_zoom();
        void setColors();
        void fillRectangle(int x0, int y0, int x1, int y1, int color);
        void drawLine(int x0, int y0, int x1, int y1, int color);
        void clearCanvas();
        void savePNG();
        Cairo::RefPtr<Cairo::Surface> m_surface;

    protected:
        bool on_draw_need(const Cairo::RefPtr<Cairo::Context>& cr);
        virtual bool on_button_press_event(GdkEventButton* event);
        virtual bool on_scroll_event(GdkEventScroll* event);
        bool on_configure_event(GdkEventConfigure* event);

    private:
        int width;
        int height;
        int zoom, minzoom, maxzoom;
        Control *control;
        Observer *observer;
        void setNewSize();
        double ctab[256][3];
};

#endif
