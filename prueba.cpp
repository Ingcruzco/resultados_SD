#include <iostream>
#include "mpi.h"
using namespace std;
extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb/stb_image.h"
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb/stb_image_write.h"
}
using namespace std;
int main(int argc, char *argv[]){
    int iam,tasks, root=0, width, height,size_array,acc=0,y=0,x=0;
    width=10;
    height=10;
    tasks=4;
    unsigned char* output_image, *rbuf;
    size_array=width*height/tasks;
    output_image=(unsigned char *)malloc(size_array*3);
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    MPI_Comm_rank( MPI_COMM_WORLD, &iam); 
    if(iam==root){
       rbuf=(unsigned char *)malloc(height*width*3);
    }
    for (int i=iam*size_array;i<size_array*(iam+1);i++){
        y=(int)floor(i/width);
        x=(int)(i-y*width);
        acc=y*width+x; 
        *(output_image+acc*3)=(uint8_t)rand()%(150-100 + 1) + 100;
        *(output_image+acc*3+1)=(uint8_t)rand()%(150-100 + 1) + 100;
        *(output_image+acc*3+2)=(uint8_t)rand()%(150-100 + 1) + 100;
        
    }
    
    MPI_Gather(output_image,size_array*3 , MPI_UNSIGNED_CHAR, rbuf, size_array*3, MPI_UNSIGNED_CHAR, root, MPI_COMM_WORLD);    
    std::cout<<"proceso terminado"<<endl; 
    if(iam==root){
       stbi_write_jpg("EDGE_IMAGE.jpg",width,height,3,rbuf,100);
       stbi_image_free(rbuf); 
    } 

    MPI_Finalize();
   // stbi_write_jpg("EDGE_IMAGE.jpg",width,height,3,rbuf,100);
    //stbi_image_free(output_image); 
    std::cout<<"Finalizado... "<<endl;
    return 0;
}
