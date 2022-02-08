//mpicc mpi_gather-test.c -o mpi_gather-test -lm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#define MSG_LENGTH 10
#define MAXTASKS 32


int main (int argc, char *argv[])
{
   int i,sendarray[MSG_LENGTH] , tag=0, tasks, iam,gsize , root=0, buff2send, buff2recv[MAXTASKS], namelen;
    double x;
    int *rbuf;	
    char processor_name[MPI_MAX_PROCESSOR_NAME];
     
    
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    MPI_Comm_rank( MPI_COMM_WORLD, &iam); 
   
    if ( iam == root) { 
           rbuf = (int *)malloc(tasks*MSG_LENGTH*sizeof(int)); 
       } 
    for(int i=0; i<MSG_LENGTH;i++){
    	sendarray[i]=iam; 	
    }	
    MPI_Gather( sendarray, MSG_LENGTH, MPI_INT, rbuf, MSG_LENGTH, MPI_INT, root, MPI_COMM_WORLD); 
    if (iam == 0) {
        for(i = 0; i < tasks*MSG_LENGTH; i++)
            printf(" %i ",*(rbuf +i));
    }
    MPI_Finalize();
}
