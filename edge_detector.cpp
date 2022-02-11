#include<iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <cmath>
#include <vector>
// #include "stb/stb_image.h"
#include <string>
#include "mpi.h"
using namespace std;
extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb/stb_image.h"
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb/stb_image_write.h"
}
#define LOW 20
#define HIGH 25
#define THREADS 16
#define CHANNEL 3

unsigned char *compute_grayscale(unsigned char *img,int width,int height,int channels);
unsigned char *compute_blur(unsigned char *img, int width,int height,int channels);
void compute_gradient(unsigned char* img,unsigned char* img_original, int width,int height, int argc,char* argv[]);
// unsigned char* convert_to_gray(unsigned char* img,int width,int height);


int main(int argc, char* argv[]){
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
    int counter;
    unsigned char *grayImage;
    unsigned char *blurImage;
     string inputPath="imagen.jpg";
    int width,height,channels;
    unsigned char *img = stbi_load(inputPath.c_str(), &width, &height, &channels, 0);
    if(img ==NULL) {
        printf("Error cargando la imagen\n");
        exit(1);
    }
    printf("Imagen cargada con un ancho de: %dpx, un alto de %dpx y %d canales\n",width,height,channels);
    grayImage=compute_grayscale(img,width,height,channels);
    stbi_write_jpg("gray_scale.jpg",width,height,1,grayImage,100);
    blurImage=compute_blur(grayImage,width,height,channels);
    stbi_write_jpg("blur_filter.jpg",width,height,1,blurImage,100);
    // gradientImage=compute_gradient(blurImage,width,height);
    
    free(blurImage);

    unsigned char* gradientImage = stbi_load("blur_filter.jpg",&width, &height, &channels, 0);

    compute_gradient(gradientImage,img,width,height,argc,argv);
    stbi_image_free(img);
    free(grayImage);
    free(gradientImage);
    gettimeofday(&tval_after, NULL);

    timersub(&tval_after, &tval_before, &tval_result);
    printf("Time elapsed: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
    return 0;
}

unsigned char *compute_grayscale(unsigned char *img,int width,int height,int channels){
    
    int gray_channels=channels==4?2:1;
    size_t img_size =width*height*channels;
    size_t gray_img_size=width*height*gray_channels;
    unsigned char* gray_img=(unsigned char*)malloc(gray_img_size);

    for(unsigned char *p=img,*pg=gray_img;p!=img + img_size;p+=channels,pg+=gray_channels){
        *pg=(uint8_t)((*p + *(p+1)+ *(p+2))/3.0);
        if(channels==4){
            *(pg+1)=*(p+3);
        }
    }


    return gray_img;
}

unsigned char *compute_blur(unsigned char *img, int width,int height,int channels){

    float kernel[5][5]={{0.00390625,0.015625,0.0234375,0.015625,0.00390625},{0.015625,0.0625,0.09375,0.0625,0.015625},{0.0234375,0.09375,0.140625,0.09375,0.0234375},{0.015625,0.0625,0.09375,0.0625,0.015625},{0.00390625,0.015625,0.0234375,0.015625,0.00390625}};
    int gray_channels=1;
    int offset=2;
    int iterator=0;
    size_t img_size =width*height;
    size_t gray_img_size=width*height*gray_channels;
    int acc;
    unsigned char* gray_img=(unsigned char*)malloc(gray_img_size);
    unsigned char* pg=gray_img;
    int xn,yn,y,x;
// #pragma omp parallel num_threads(THREADS)
// {
//     #pragma omp for
    for(int i=0;i<gray_img_size;i++){
    // for(int x=0;x<width;x++){
        y=(int)floor(i/width);
        x=(int)(i-y*width);
        acc=0;
        
        for(int a=0;a<5;a++){
            for(int b=0;b<5;b++){
                xn=x+a-offset;
                yn=y+b-offset;
                if(xn<0){
                    xn=0;
                }else if(xn>=width){
                    xn=width-1;
                }
                if(yn<0){
                    yn=0;
                }else if(yn>=height){
                    yn=height-1;
                }
                acc+=*(img+yn*width+xn)*kernel[a][b];
            }
        }
        // iterator=y*width+x;
        *(pg+i)=(uint8_t)(acc);
        //  iterator+=1;
    }
    //}

//}

    return gray_img;
}

void compute_gradient(unsigned char* img,unsigned char* img_original, int width,int height,int argc,char* argv[]){

    size_t img_size =width*height;
    size_t gray_img_size=width*height*CHANNEL;
    size_t imagePartialSize;
    unsigned char* gray_img;
    gray_img=compute_grayscale(img,width,height,CHANNEL);
    
    int channel=3, magx,magy,h,w,size,iam;
    int mag;
    int acc=0,root=0,counter;
    unsigned char* output_image,* rbuf,* partialBuffer;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &iam);
    if(iam==root){
      rbuf=(unsigned char *)malloc(width*height*channel);
      imagePartialSize=width*height*3/size;
      magx=0;
      magy=0;   
    }
    MPI_Bcast(&imagePartialSize, 1,MPI_UNSIGNED_LONG_LONG,0,MPI_COMM_WORLD);
    MPI_Bcast(&channel,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    partialBuffer=( unsigned char*)malloc(imagePartialSize);
    output_image=(unsigned char*)malloc(imagePartialSize);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(&img_original, imagePartialSize, MPI_UNSIGNED_CHAR,output_image, imagePartialSize, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD );
    MPI_Barrier(MPI_COMM_WORLD);
    /*for(int i=0;i<imagePartialSize;i++){
        *(partialBuffer+i)=(uint8_t)(*(output_image + i*3)+*(output_image+i*3+1)+*(output_image+i*3+2))/3;
    }*/
    MPI_Barrier( MPI_COMM_WORLD );
    MPI_Gather( &output_image, imagePartialSize, MPI_UNSIGNED_CHAR, rbuf, imagePartialSize, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD );
    /*for(int i=iam*imagePartialSize;i<imagePartialSize*(iam+1);i++){
        // for(int w=0;w<width;w++){
            h=(int)floor(i/width);
            w=(int)(i-h*width);
            acc=h*width+w;
            if((w>0)&&(w<width-1)&& (h>0)&&(h<height-1)){
                magx=*(gray_img+acc+1)-*(gray_img+acc-1);
                magy=*(gray_img+acc+width)-*(gray_img+acc-width);
                mag=sqrt(pow(magx,2)+pow(magy,2));
                
                if(mag>HIGH){
                    *(output_image+acc*3)=(uint8_t)255;
                    *(output_image+acc*3+1)=(uint8_t)0;
                    *(output_image+acc*3+2)=(uint8_t)0;
                }
                else{
                    *(output_image+acc*3)=*(img_original+acc*3);
                    *(output_image+acc*3+1)=*(img_original+acc*3+1);
                    *(output_image+acc*3+2)=*(img_original+acc*3+2);    
                }
            }else{
                *(output_image+acc*3)=(uint8_t)*(img_original+acc*3);
                *(output_image+acc*3+1)=(uint8_t)*(img_original+acc*3+1);
                *(output_image+acc*3+2)=(uint8_t)*(img_original+acc*3+2); 
            }
        }*/
    //MPI_Gather(output_image,total_size*3, MPI_UINT8_T,rbuf, total_size*3, MPI_UINT8_T, root, MPI_COMM_WORLD);     
    if(iam==root){
        std::cout<<"Creando Imagen ....."<<endl;
        stbi_write_jpg("EDGE_IMAGE.jpg",width,height,3,rbuf,100);
        stbi_image_free(rbuf);
        stbi_image_free(output_image);   
    }  
    MPI_Finalize();
   
}

// unsigned char* convert_to_gray(unsigned char* img,int width,int height){

//    int gray_channels=CHANNEL==4?2:1;
//     size_t img_size =width*height*CHANNEL;
//     size_t gray_img_size=width*height*gray_channels;
//     unsigned char* gray_img=(unsigned char*)malloc(gray_img_size);

//     for(unsigned char *p=img,*pg=gray_img;p!=img + img_size;p+=CHANNEL,pg+=gray_channels){
//         *pg=(uint8_t)((*p + *(p+1)+ *(p+2))/3.0);
//         if(CHANNEL==4){
//             *(pg+1)=*(p+3);
//         }
//     }

//     return gray_img; 

// }














