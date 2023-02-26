
all:
	(cd src ; make)
	(cd po ; make)

install:
	(cd src ; make install)
	(cd po ; make install)

clean:
	(cd src ; make clean)
	(cd po ; make clean)
