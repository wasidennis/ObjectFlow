INCDIR = -I.
DBG    = -g -pg
OPT    = -O3
CPP    = g++
CFLAGS = -Wall $(INCDIR)
LINK   = -lm 

HEADERS = edges.h \
		filter.h \
		histogram.h \
		segment-graph.h \
		image.h \
		misc.h \
		pnmfile.h \
		segment-image.h \
		disjoint-set.h



.cpp.o:
	$(CPP) $(CFLAGS) -c $< -o $@

all: program

program: gbh.cpp $(HEADERS)
	$(CPP) $(OPT) $(CFLAGS) -o gbh gbh.cpp $(LINK)

debug: gbh.cpp $(HEADERS)
	$(CPP) $(DBG) $(CFLAGS) -o gbh_debug gbh.cpp $(LINK)

clean:
	/bin/rm -f  gbh gbh_debug *.o

clean-all: clean
	/bin/rm -f *~ 



