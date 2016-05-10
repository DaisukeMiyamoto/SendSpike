# Makefile

#CC       = mpifccpx
CC	= mpicc
CFLAGS	= -Wall -O2
#CFLAGS   = -Kfast,openmp,optmsg=2 -DKCOMPUTER -Nrt_tune,src,sta
#CFLAGS   = -Kfast,openmp,ocl,optmsg=2 -DKCOMPUTER -Nrt_tune,src,sta -DTABLE_TYPE
LDFLAGS  = $(CFLAGS)
LDLIBS   = -lm
INCLUDES = spikearray.h
SRC      = spike_main.c spikearray.c
OBJS     = spike_main.o spikearray.o
PROG     = spike.out


all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDLIBS) $(LDFLAGS)

asm: $(INCLUDES) $(SRC)
	$(CC) -S $(SRC) $(CFLAGS)

*.o : $(INCLUDES) $(SRC)

.c.o:

clean:
	rm -fv $(OBJ) *~ *.o

