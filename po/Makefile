
POTFILES=../src/abmtools.cc \
	../src/Agent.cc \
	../src/Alliance.cc \
	../src/anthropoids.cc \
	../src/Cell.cc \
	../src/control.cc \
	../src/Model.cc \
	../src/Observer.cc \
	../src/Patch.cc \
	../src/Prey.cc \
	../src/THV.cc \
	../src/Tree.cc \
	../src/zoomraster.cc \
	../src/listWindow.cc

pt_BR.mo: pt_BR.po
	msgfmt pt_BR.po -o pt_BR.mo

pt_BR.po: anthropoids.pot
	msgmerge --update pt_BR.po anthropoids.pot
	
anthropoids.pot: $(POTFILES)
	xgettext --keyword=_ --language=C --add-comments -o anthropoids.pot ../src/*.cc

install: pt_BR.mo
	cp pt_BR.mo /usr/local/share/locale/pt_BR/LC_MESSAGES/anthropoids.mo

clean:
	rm -f *.po~ *.mo
