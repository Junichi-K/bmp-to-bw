#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

//we're interested in the name and image_offset parameters only. The file type should be "BM" and image_offset specifies the start of the image data
struct bmp_header{
    char name[3];
    int garbage[2];
    unsigned int image_offset;
};

//helps us retrieve image height and width and we wanna make sure that bits/pixel is 24(3 color values) and that there is no compression method
struct dib_header{
    unsigned int size;
    int image_width;
    int image_height;
    unsigned short int color_planes;
    unsigned short int bits_per_pixel;
    unsigned short int compression_method;
    unsigned int image_size;
    int garbage[24];
    unsigned short int temp;
};

bool valid_file(struct bmp_header first_header, struct dib_header second_header){//returns true if file can be transormed
    
    if(first_header.name[0] != 'B' || first_header.name[1] != 'M'){
        return false;
    }

    if(second_header.bits_per_pixel != 24 || second_header.compression_method != 0){
        return false;
    }
    
    return true;
}


struct pixel{//used to retrieve individual pixel data
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

unsigned char pixel_to_grayscale(struct pixel rgb){//convert the pixel to a grayscale value
    return 0.21 * rgb.red + 0.72 * rgb.green + 0.07 * rgb.blue;
}
    


struct image{//contains all the relevant info of the image we require: its height, width and a pointer to the beginning of the image
    int height; 
    int width;
    struct pixel **rgb;
};

int main(){
    
    FILE* rimac_img = fopen("rimac.bmp", "rb");//opening image of the fastest ev in the world (yeah I'm mentally 12)
    if(rimac_img == NULL){
        printf("Couldn't open file!1");
        return 1;
    }
    
    struct bmp_header header1;
    struct dib_header header2;

    fread(header1.name, 2, 1, rimac_img);//reading bmp_header and dib_headers of image
    fread(&header1.garbage, 3 * sizeof(int), 1, rimac_img);
    fread(&header2.size, sizeof(struct dib_header), 1, rimac_img);

    if(valid_file(header1, header2)){//block that gets executed if file is tranformable
        fseek(rimac_img, header1.image_offset, SEEK_SET);
        
        FILE* bw_rimac_img = fopen("bw_rimac.bmp", "wb");//creating black and white image file
        if(bw_rimac_img == NULL){
            printf("Couldn't open file!!");
            return 1;
        }

        fwrite(header1.name, 2, 1, bw_rimac_img);//writing all the header info of the image into this file cuz we are essentially just changing values
        fwrite(&header1.garbage, 12, 1, bw_rimac_img);
        fwrite(&header2, sizeof(struct dib_header), 1, bw_rimac_img);
        
        int rimac_width = header2.image_width;//setting height and width
        int rimac_height = header2.image_height; 
        
        struct pixel current_pixel;
    
        for (int i = 0; i < rimac_height; i++){//writing black and white pixel data into image (height * width)
            for (int j = 0; j < rimac_width; j++){
                fread(&current_pixel, sizeof(struct pixel), 1, rimac_img);
                current_pixel.red = current_pixel.blue = current_pixel.green = pixel_to_grayscale(current_pixel);
                fwrite(&current_pixel, sizeof(struct pixel), 1, bw_rimac_img);
            }
        }
        
        fclose(bw_rimac_img);
        printf("Success");
    }

    else{
        printf("The image you provided can't be transformed :(");//this is just sad news
    }

    fclose(rimac_img);

    return 0;
}