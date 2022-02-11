#include<iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <cmath>
#include <string>
#include "mpi.h"
using namespace std;
extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb/stb_image.h"
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb/stb_image_write.h"
}

int main(int argc, char *argv[]){

    int width,height,channels, root=0, iam, tasks,counter=0;
    string inputPath="imagen.jpg";
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
    //Image load
    unsigned char *rbuf, *img = stbi_load(inputPath.c_str(), &width, &height, &channels, 0);
    if(img ==NULL) {
        printf("Error cargando la imagen\n");
        exit(1);
    }
    printf("Ancho: %i, Alto: %i, canal: %i",width,height,channels);

    size_t imageFullSize=width*height;
       
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    MPI_Comm_rank( MPI_COMM_WORLD, &iam);
    int  partialImageSize=width*height/tasks;
    unsigned char outputImage[partialImageSize];
    
    if(iam==root){
	size_t  outputSizeImage=width*height;
	rbuf=(unsigned char*)malloc(outputSizeImage);	 
    }
	
    for(int i=iam*partialImageSize;i<partialImageSize*(iam+1);i++){
      outputImage[counter]= (*(img+i*3)+*(img+i*3+1)+*(img+i*3))/3;
      counter+=1;
     }      
    

    MPI_Gather(outputImage, partialImageSize, MPI_UNSIGNED_CHAR, rbuf, partialImageSize, MPI_UNSIGNED_CHAR, root, MPI_COMM_WORLD); 
    
    if(iam==root){		
    	stbi_write_jpg("Imagen_de_Salida.jpg",width,height,1,rbuf,100);    
    	stbi_image_free(img);
    	gettimeofday(&tval_after, NULL);
    	timersub(&tval_after, &tval_before, &tval_result);
   	 printf("Time elapsed: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
     }
     MPI_Finalize();

     return 0;
}
