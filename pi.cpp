#include<iostream>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"
#define ITERATIONS 10E8
using namespace std;

int main (int argc, char *argv[])
{

   long accumulator=0,acc=0,iterations=0,salida;
   int root=0,tasks,iam;
   struct timeval tval_before, tval_after, tval_result;
   gettimeofday(&tval_before, NULL);
   
   MPI_Status status;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &tasks);
   MPI_Comm_rank( MPI_COMM_WORLD, &iam);
   iterations=ITERATIONS/tasks;
   if(iam==root){
      salida=0;
   }
   for(int i=iam*iterations;i<iterations*(iam+1);i++){
      accumulator+=1;
   }
  
   gettimeofday(&tval_after, NULL);

   timersub(&tval_after, &tval_before, &tval_result);
   printf("Time elapsed: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
   return 0;

}
