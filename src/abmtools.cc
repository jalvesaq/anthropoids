
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <gsl/gsl_rng.h>
#ifndef NOGUI
#include <gtkmm.h>
#include <gtkmm/dialog.h>
#endif
#include <math.h>
#include <unistd.h>
#include "abmtools.h"


extern bool GUIMode;

gsl_rng * rng;
double MaxRN;

double getRandDouble(double min, double max)
{
#ifdef DEBUG
    if(min >= max)
        fprintf(stderr, "Warning: min >= max (%s : %d)\n", __FILE__, __LINE__);
#endif
    double r = gsl_rng_uniform(rng);
    r = r * (max - min) + min;
#ifdef DEBUG
    if(r < min || r > max){
        fprintf(stderr, "ERROR: r = %f, min = %f, max = %f (%s : %d)\n", r, min, max, __FILE__, __LINE__);
        exit(1);
    }
#endif
    return r;
}

int getRandInt(int min, int max)
{
#ifdef DEBUG
    if(min >= max)
        fprintf(stderr, "Warning: min >= max (%d %d) (%s : %d)\n", min, max, __FILE__, __LINE__);
#endif
    double r = gsl_rng_uniform(rng);
    int ir = (int)(round(r * (double)(max - min) + (double)min));
#ifdef DEBUG
    if(ir < min || ir > max){
        fprintf(stderr, "ERROR: r = %d, min = %d, max = %d (%s : %d)\n", ir, min, max, __FILE__, __LINE__);
        exit(1);
    }
#endif
    return ir;
}

void fatalErr(const char *modulname, int lno, const char *fmt, ...)
{
    va_list argptr;
    char msg[255];
    char msg2[255];
    va_start(argptr, fmt);
    strcpy(msg, fmt);
    vsprintf(msg2, msg, argptr);
    sprintf(msg, _("FATAL ERROR: \"%s\", line %d\n    "), modulname, lno);
    strcat(msg, msg2);
    if(GUIMode){
#ifndef NOGUI
        Gtk::MessageDialog dialog(_("FATAL ERROR!"), false,
                Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, false);
        dialog.set_secondary_text(msg);
        dialog.run();
#endif
    } else{
        int isTTY = isatty(2);
        if(isTTY)
            fprintf(stderr, "\033[1;31m");
        fprintf(stderr, "\n%s\n", msg);
        if(isTTY)
            fprintf(stderr, "\033[0m ");
    }
    exit(1);
}

void warnMsg(const char *fmt, ...)
{
    va_list argptr;
    char msg[255];
    char msg2[255];
    va_start(argptr, fmt);
    strcpy(msg, fmt);
    vsprintf(msg2, msg, argptr);
    if(GUIMode){
#ifndef NOGUI
        Gtk::MessageDialog dialog(_("Warning:"), false,
                Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, false);
        dialog.set_secondary_text(msg2);
        dialog.run();
#endif
    } else{
        int isTTY = isatty(2);
        if(isTTY)
            fprintf(stderr, "\n\033[1;31m%s\033[0m", _("Warning:"));
        else
            fprintf(stderr, _("Warning:"));
        fprintf(stderr, "  %s\n", msg2);
    }
}

