# this make files just go to all subdirs

SUBDIRS=Hash TaskIdList cser DataSpace prod_cons Cache Task stack dbLkList

.PHONY: all clean ${SUBDIRS}
all: ${SUBDIRS}

${SUBDIRS}:
	make -C $@
	make -C $@ install

clean:
	make -C Hash clean
	make -C TaskIdList clean
	make -C cser clean
	make -C DataSpace clean
	make -C prod_cons clean
	make -C Cache clean
	make -C Task clean
	make -C stack clean
	make -C dbLkList clean
