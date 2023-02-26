
# -DGDK_VERSION_MIN_REQIRED=GDK_VERSION_3_4 -DGTK_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DG_DISABLE_DEPRECATED -DGDKMM_DISABLE_DEPRECATED -DGTKMM_DISABLE_DEPRECATED
GTKMMCFLAGS := $(shell pkg-config gtkmm-3.0 --cflags)

GTKMMLIBS := $(shell pkg-config gtkmm-3.0 --libs)

GSLLIBS := $(shell pkg-config gsl --libs)

CFLAGS=-g -O2 -Wall -Wextra -pedantic -c

OBJS=anthropoids.o Model.o Cell.o list.o Patch.o Prey.o THV.o Tree.o abmtools.o Agent.o Alliance.o Observer.o control.o listWindow.o zoomraster.o

HEADERS=abmtools.h Agent.h Alliance.h Cell.h config.h list.h Model.h Patch.h Prey.h THV.h Tree.h Observer.h control.h listWindow.h zoomraster.h

anthropoids: $(OBJS)
	g++ -O2 -o anthropoids $(OBJS) $(GSLLIBS) $(GTKMMLIBS)

list.o: list.cc list.h abmtools.h
	g++ $(CFLAGS) -o list.o list.cc

abmtools.o: abmtools.cc abmtools.h
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o abmtools.o abmtools.cc

Agent.o: Agent.cc $(HEADERS)
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o Agent.o Agent.cc

Alliance.o: Alliance.cc $(HEADERS)
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o Alliance.o Alliance.cc

anthropoids.o: anthropoids.cc $(HEADERS)
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o anthropoids.o anthropoids.cc

Cell.o: Cell.cc $(HEADERS)
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o Cell.o Cell.cc

Model.o: Model.cc $(HEADERS)
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o Model.o Model.cc

Patch.o: Patch.cc $(HEADERS)
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o Patch.o Patch.cc

Prey.o: Prey.cc $(HEADERS)
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o Prey.o Prey.cc

THV.o: THV.cc $(HEADERS)
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o THV.o THV.cc

Tree.o: Tree.cc $(HEADERS)
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o Tree.o Tree.cc

Observer.o: Observer.cc $(HEADERS)
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o Observer.o Observer.cc

control.o: control.cc control.h
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o control.o control.cc

zoomraster.o: zoomraster.cc zoomraster.h
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o zoomraster.o zoomraster.cc

listWindow.o: listWindow.cc listWindow.h Agent.h
	g++ $(CFLAGS) $(GTKMMCFLAGS) -o listWindow.o listWindow.cc

install: anthropoids
	install -s anthropoids /usr/local/bin/anthropoids
	mkdir -p /usr/local/share/doc/anthropoids
	cp analyse.R /usr/local/share/doc/anthropoids

data_clean:
	rm -f *.csv world_*.png parameters lastParameters followedAgents statistics knownPatches

clean:
	rm -f *.o anthropoids
