CC = gcc
IDIR = ../include
CFLAGS = -I$(IDIR)

ODIR = ../obj
LDIR = ../lib

LIBS = -lm -lpthread

_DEPS = cache.h filtragem.h inspecao.h proxyServerToin.h estruturas.h
DEPS = $(patsubst%,$(IDIR)/%,$(_DEPS))

_OBJ = cache.o filtragem.o proxyEstruturas.o proxy.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
proxy: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
	
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
