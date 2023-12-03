#ifndef ABMTOOLS_H
#define ABMTOOLS_H

#ifndef MINGW32
#include <libintl.h>
#include <locale.h>
#define _(String) gettext (String)
#else
#define _(String) (String)
#endif

/*
Class hierarchy:

    Thing
      Cell
      Patch
      Eukaryote
        Agent
        Prey
        THV
        Tree

 */

class Thing
{
    public:
        virtual ~Thing() { }
};

class Eukaryote : public Thing
{
    public:
        virtual void step() { }
};


double getRandDouble(double min, double max);
int getRandInt(int min, int max);
__attribute__((format(printf,3,4)))
void fatalErr(const char *modulname, int lno, const char *fmt, ...);
__attribute__((format(printf,1,2)))
void warnMsg(const char *fmt, ...);

// based on swarm code
#define FATALERR( fmt, ...) fatalErr(__FILE__, __LINE__, fmt, ## __VA_ARGS__)

#endif
