
##### You need to see these with the location of your PNG library 
#####  if it is not by default in your build path
PNGINC = /sw64/include
PNGLIB = /sw64/lib

CPPFLAGS = -I${PNGINC} -O3 -Wall

LIBS = -L${PNGLIB} -lpng -lz -lm

PUBHDRS = images.h \
			matrix.h \
			graphs.h \
			config.h \
			settings.h \
			imagegraphs.h

OBJS = images.o \
		config.o \
		graphs.o \
		imagegraphs.o \
		vizhierarchy.o \
		swa.o

COBJS = imagefuncs.o

all:	.depend $(OBJS) $(COBJS) programs

programs:
	g++ -o swa $(OBJS) $(COBJS) $(LIBS);

clean:
	rm -f *.o swa

.depend:	$(OBJS:.o=.cpp) $(COBJS:.o=.c)
	$(CC) -M $(CPPFLAGS) $(OBJS:.o=.cpp) $(COBJS:.o=.c) > .depend
