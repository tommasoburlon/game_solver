target := sudoku wordle

all: $(target)

shared:
	cd shared && $(MAKE) all LIBPATH=${CURDIR}/bin/lib.o

$(target): shared
	cd $@ && $(MAKE) all OUTDIR=${CURDIR}/bin LIBPATH=${CURDIR}/bin/lib.o HDRPATH=${CURDIR}/shared/include

run:
	./bin/$(GAME)

.PHONY: shared
