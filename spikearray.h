#ifndef __SPIKEARRAY_H__
#define __SPIKEARRAY_H__

typedef struct _spike{
  int step;
  int cellid;
} Spike;

typedef struct _spikearray{
  int max;
  int n;
  Spike *spikes;
} SpikeArray;


int alloc_spikearray(SpikeArray *spikearray, int size);
void free_spikearray(SpikeArray *spikearray);
void print_spikearray(SpikeArray *spikearray);


#endif
