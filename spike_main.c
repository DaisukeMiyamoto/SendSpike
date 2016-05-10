#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mpi.h"

#include "spikearray.h"

const int MPI_ROOT_ID = 0;

const int SPIKEGENERATOR_MAX = 100000;
const int SPIKERECORDER_MAX  = 100000;


typedef struct _connection{
  int precellid;
  int postcellid;
} Connection;

typedef struct _connectionarray{
  int n;
  Connection *conn;
} ConnectionArray;


void init_spikegenerator_example(SpikeArray *spikegenerator, const int stop_step, const int n_cell_local, const int cellid_offset)
{
  int STOP_STEP = 1000;
  int i, j;

  alloc_spikearray(spikegenerator, SPIKEGENERATOR_MAX);
  spikegenerator->n = 0;
  for (i=0; i<STOP_STEP; i++)
    {
      for (j=0; j<n_cell_local; j++){
	spikegenerator->spikes[spikegenerator->n].step = i;
	spikegenerator->spikes[spikegenerator->n].cellid = j+cellid_offset;
	spikegenerator->spikes[spikegenerator->n].state = 0;
	spikegenerator->n++;
      }
    }

  return;
}


void init_spikerecorder(SpikeArray *spikerecorder)
{
  alloc_spikearray(spikerecorder, SPIKERECORDER_MAX);
  spikerecorder->n = 0;
}


void init_spikebuffer(SpikeArray *spikebuffer, int n_connection)
{
  int i;
  alloc_spikearray(spikebuffer, n_connection);
  spikebuffer->n = n_connection;

  for (i=0; i<spikebuffer->n; i++)
    {
      spikebuffer->spikes[i].step = 0;
      spikebuffer->spikes[i].cellid = i;
      spikebuffer->spikes[i].state = 0;
    }
}

void init_connection(ConnectionArray *connarray, int n_cell)
{
  int i;

  connarray->conn = (Connection *)malloc(n_cell * sizeof(Connection));
  if(connarray->conn == NULL)
    {
      printf ("Memory Allocation Error.\n");
      exit(-1);
    }
  connarray->n = n_cell;

  for (i=0; i<n_cell; i++)
    {
      connarray->conn[i].precellid = i;
      connarray->conn[i].postcellid = (i+1)%n_cell;
    }
}

void print_connection(ConnectionArray *connarray)
{
  int i;
  for (i=0; i<connarray->n; i++)
    {
      printf ("%d -> %d\n", connarray->conn[i].precellid, connarray->conn[i].postcellid);
    }
}

void check_result(SpikeArray *spikegenerator, SpikeArray *spikerecorder)
{
  int sum_send_spikes;
  int sum_recv_spikes;
  int mpi_id;

  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
  MPI_Reduce(&(spikegenerator->n), &(sum_send_spikes), 1, MPI_INT, MPI_SUM, MPI_ROOT_ID, MPI_COMM_WORLD);
  MPI_Reduce(&(spikerecorder->n),  &(sum_recv_spikes), 1, MPI_INT, MPI_SUM, MPI_ROOT_ID, MPI_COMM_WORLD);

  if (mpi_id == MPI_ROOT_ID)
    {
      printf (" [Check]\n");
      printf ("  * Send Spikes: %d\n", sum_send_spikes);
      printf ("  * Recv Spikes: %d\n", sum_recv_spikes);
    }
}


void send_spike_main()
{
  //char in_filename_template[] = "%s/";
  //char out_filename_template[] = "%s/";
  int mpi_id, mpi_size;
  int n_cell_local = 4;
  int stop_step = 5;

  SpikeArray _spikegenerator, _spikerecorder, _spikebuffer;
  SpikeArray *spikegenerator = &_spikegenerator;
  SpikeArray *spikerecorder = &_spikerecorder;
  SpikeArray *spikebuffer = &_spikebuffer;

  ConnectionArray _connarray;
  ConnectionArray *connarray = &_connarray;


  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  if(mpi_id == MPI_ROOT_ID){
    printf(" [SendSpike Conditions]\n");
    printf("  * MPI Size:      %d\n", mpi_size);
    printf("  * N_CELL:        %d\n", n_cell_local * mpi_size);
    printf("  * Spike Pattern: %s\n", "simple");
  }

  //data = read_datafile(in_filename, cellid);
  init_spikegenerator_example(spikegenerator, stop_step, n_cell_local, n_cell_local*mpi_id);
  init_spikerecorder(spikerecorder);

  init_connection(connarray, n_cell_local * mpi_size);
  print_connection(connarray);
  init_spikebuffer(spikebuffer, connarray->n);

  // main loop
  printf (" [Start]\n");
  int step;
  int i_spikegenerator=0;
  for(step=0; step < stop_step; step++)
    {
      printf("step=%d\n", step);
      unset_spikearray_state(spikebuffer);
      while(spikegenerator->spikes[i_spikegenerator].step == step)
	{
	  spikebuffer->spikes[ (spikegenerator->spikes[i_spikegenerator].cellid) ].state = 1;
	  i_spikegenerator++;
	}
      print_spikearray(spikebuffer);

    }
  printf (" [Finish]\n");

  //print_spikearray(spikegenerator);
  //print_spikearray(spikerecorder);
  //write_datafile(out_filename);
  check_result(spikegenerator, spikerecorder);
  
  free_spikearray(spikegenerator);
  free_spikearray(spikerecorder);
  free_spikearray(spikebuffer);

  return;
}

int main(int argc, char **argv)
{
  // setup MPI
  MPI_Init(&argc, &argv);  

  send_spike_main();

  MPI_Finalize();
  return(0);
}
