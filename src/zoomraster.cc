
#include <gtkmmconfig.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/widget.h>
#include <cairomm/context.h>
#include <gdkmm/general.h>

#include "zoomraster.h"
#include "control.h"
#include "abmtools.h"
#include "Observer.h"

extern bool GUIMode;

ZoomRaster::ZoomRaster(Control *cntrl, Observer *obs, int xsize, int ysize)
{
    control = cntrl;
    observer = obs;
    width = xsize;
    height = ysize;
    zoom = 1;
    minzoom = 1;
    maxzoom = 5;

    // avoid black region if the world is too small
    int w = zoom * width * 5;
    while(w < 220 && zoom < maxzoom){
        zoom++;
        minzoom++;
        w = zoom * width * 5;
    }

    setColors();

    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK );

    signal_draw().connect(sigc::mem_fun(*this, &ZoomRaster::on_draw_need));

    // signal emitted when the size, position or stacking of the Widget's window has changed.
    signal_configure_event().connect(sigc::mem_fun(*this, &ZoomRaster::on_configure_event));
}

ZoomRaster::~ZoomRaster()
{
}

int ZoomRaster::get_zoom()
{
    return zoom;
}

void ZoomRaster::setColors()
{
    ctab[0][0] = 0.05; // Background color
    ctab[0][1] = 0.05;
    ctab[0][2] = 0.05;

    double r, g, b;
    r = 0.787692;
    g = 0.669231;
    b = 0.315385;
    for(int i = 1; i < 11; i++){ // thv: between green and yellow
        ctab[i][0] = r;
        ctab[i][1] = g;
        ctab[i][2] = b;
        r -= 0.0393846;
        g -= 0.0109231;
        b -= 0.0118462;
    }
    r = 1.0;
    g = 0.0;
    b = 0.0;
    for(int i = 20; i < 30; i++){ // Agents according to sex ratio
        ctab[i][0] = r;
        ctab[i][1] = g;
        ctab[i][2] = b;
        r -= 0.1;
        b += 0.1;
    }
    r = 1.0;
    g = 1.0;
    b = 1.0;
    for(int i = 30; i < 40; i++){ // Agents according to population density
        ctab[i][0] = r;
        ctab[i][1] = g;
        ctab[i][2] = b;
        r -= 0.1;
        g -= 0.1;
        b -= 0.05;
    }
    for(int i = 100; i < 120; i++){ // Fruits
        ctab[i][0] = 20.0/(double)(140 - i);
        ctab[i][1] = 0.6;
        ctab[i][2] = 0;
    }

    ctab[200][0] = 0.0; // Tree - species 0
    ctab[200][1] = 0.5;
    ctab[200][2] = 0.0;
    ctab[201][0] = 0.2; // Tree - species 1
    ctab[201][1] = 0.4;
    ctab[201][2] = 0.0;
    ctab[202][0] = 0.3; // Tree - species 2
    ctab[202][1] = 0.3;
    ctab[202][2] = 0.0;
    ctab[230][0] = 1.0; // Prey
    ctab[230][1] = 1.0;
    ctab[230][2] = 0.0;
    ctab[250][0] = 1.0; // Perimeter
    ctab[250][1] = 1.0;
    ctab[250][2] = 0.0;
    ctab[255][0] = 1.0; // Follow me
    ctab[255][1] = 1.0;
    ctab[255][2] = 1.0;
}


bool ZoomRaster::on_draw_need(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (!m_surface)
        m_surface = get_window()->create_similar_surface(Cairo::CONTENT_COLOR,
                width * zoom * 5, height * zoom * 5);
    cr->set_source(m_surface, 0, 0);
    cr->paint();
    return false;
}

void ZoomRaster::setNewSize()
{
    m_surface = get_window()->create_similar_surface(Cairo::CONTENT_COLOR,
            width * zoom * 5, height * zoom * 5);
    control->setNewSize(zoom);
    observer->drawWorld();
}

bool ZoomRaster::on_configure_event(GdkEventConfigure *e)
{
    if (zoom > minzoom && (e->width < zoom * width * 5 || e->height < zoom * height * 5)) {
        zoom--;
        setNewSize();
    } else if (zoom < maxzoom && (e->width >= (zoom + 1) * width * 5 || e->height >= (zoom + 1) * height * 5)) {
        zoom++;
        setNewSize();
    }
    return true;
}

bool ZoomRaster::on_scroll_event(GdkEventScroll* e)
{
    if(e->direction){
        if(zoom < maxzoom) {
            zoom++;
            setNewSize();
        }
    } else {
        if(zoom > minzoom){
            zoom--;
            setNewSize();
        }
    }

    return true;
}

bool ZoomRaster::on_button_press_event(GdkEventButton* e)
{
    int bx = (int)e->x;
    int by = (int)e->y;
    int button = (int)e->button;
    switch(button){
        case 1:
        case 2:
            bx = bx / (5 * zoom);
            by = by / (5 * zoom);
            observer->cellClicked(button, bx, by);
            break;
        case 3:
            observer->showPopDensity();
            observer->drawWorld();
            break;
        default:
            fprintf(stderr, "Unknow mouse button: %d\n", button);
    }
    return true;
}

void ZoomRaster::drawLine(int x0, int y0, int x1, int y1, int color)
{
    int X0, Y0, X1, Y1;
    X0 = x0 * zoom;
    Y0 = y0 * zoom;
    X1 = x1 * zoom;
    Y1 = y1 * zoom;

    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(m_surface);
    cr->set_source_rgb(ctab[color][0], ctab[color][1], ctab[color][2]);
    cr->set_line_width(2.0);
    cr->move_to(X0, Y0);
    cr->line_to(X1, Y1);
    cr->stroke();
    get_window()->invalidate(true);
}

void ZoomRaster::fillRectangle(int x, int y, int w, int h, int color)
{
    int X, Y, W, H;
    X = x * zoom;
    Y = y * zoom;
    W = w * zoom;
    H = h * zoom;

    const Gdk::Rectangle update_rect(X, Y, W, H);

    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(m_surface);
    Gdk::Cairo::add_rectangle_to_path(cr, update_rect);
    cr->set_source_rgb(ctab[color][0], ctab[color][1], ctab[color][2]);
    cr->fill();
    get_window()->invalidate_rect(update_rect, true);
}

void ZoomRaster::clearCanvas()
{
    int W, H;
    W = width * zoom * 5;
    H = height * zoom * 5;
    fillRectangle(0, 0, W, H, 0);
}

void ZoomRaster::savePNG()
{
    static int i = 1;
    char str[64];
    sprintf(str, "world_%d.png", i);
    m_surface->write_to_png(str);
    i++;
}

