#include <iostream>
using namespace std;
extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb/stb_image.h"
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb/stb_image_write.h"
}
using namespace std;
int main(){
    int width, height;
    width=1200;
    height=1200;
    unsigned char* output_image=(unsigned char *)malloc(width*height*3);
    for (int i=0;i<width*height;i+=3){
        *(output_image+i)=(uint8_t)rand()%(150-100 + 1) + 100;
        *(output_image+i+1)=(uint8_t)rand()%(150-100 + 1) + 100;
        *(output_image+i+2)=(uint8_t)rand()%(150-100 + 1) + 100;
    }
    stbi_write_jpg("EDGE_IMAGE.jpg",width,height,3,output_image,100);
    stbi_image_free(output_image); 
    // std::cout<<sizeof(__uint8_t)<<endl;
    return 0;
}
