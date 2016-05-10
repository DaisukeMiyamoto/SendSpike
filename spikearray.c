#include <stdio.h>
#include <stdlib.h>

#include "spikearray.h"

int alloc_spikearray(SpikeArray *spikearray, int size)
{
  spikearray->spikes = (Spike *)malloc(size * sizeof(Spike));
  if (spikearray->spikes == NULL)
    {
      printf ("Memory Allocation Error.\n");
      exit(-1);
    }
  spikearray->max = size;

  return(0);
}


void free_spikearray(SpikeArray *spikearray)
{
  free(spikearray->spikes);
}


void unset_spikearray_state(SpikeArray *spikearray)
{
  int i;
  for(i=0; i<spikearray->n; i++)
    {
      spikearray->spikes[i].state = 0;
    }
}


void print_spikearray(SpikeArray *spikearray)
{
  int i;

  for(i=0; i<spikearray->n; i++)
    {
      printf("%8d, %8d, %8d\n", spikearray->spikes[i].step, spikearray->spikes[i].cellid, spikearray->spikes[i].state);
    }
}
