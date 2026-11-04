#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Check if argv[2] is .bmp file
    char *src_ext = strrchr(argv[2], '.');
    //Comparing the string with .bmp and comparing with null
    if (src_ext == NULL || strcmp(src_ext, ".bmp") != 0) {
        printf("Error: Source image must be .bmp file\n");
        return e_failure;
    }
    encInfo->src_image_fname = argv[2];
    
    // Check if argv[3] file exists using fopen
    FILE *test_file = fopen(argv[3], "r");
    //Checking for test file is null or not
    if (test_file == NULL) {
        printf("Error: Secret file not found\n");
        return e_failure;
    }
    //By not returning any failure we close the file
    fclose(test_file);
    encInfo->secret_fname = argv[3];
    
    // Check if argv[4] is present
    if (argv[4] != NULL) {
        // Check if argv[4] is .bmp file
        char *stego_ext = strrchr(argv[4], '.');
        //Comparing the stego.ext with NULL and .bmp
        if (stego_ext == NULL || strcmp(stego_ext, ".bmp") != 0) {
            printf("Error: Stego image must be a .bmp file\n");
            return e_failure;
        }
        encInfo->stego_image_fname = argv[4];
    }
    else {
        // Store default name
        encInfo->stego_image_fname = "stego.bmp";
    }
    
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo){
        //step->1 check open files(encInfo) return value is e_success or not
        //   e_failure ->print error message and return e_failure
        // for success we modify.
         int ret = open_files(encInfo);
         if(ret != e_success){
            printf("Error at opening the file.");
            return e_failure;
         }
         else{
            encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

            //check capacity
            Status capacity_result = check_capacity(encInfo);
            //Its not an success return failure
            if(capacity_result != e_success){
                printf("Error :Insufficient Capacity\n");
                return e_failure;
            }
            //copy_bmp_header
            Status header_result = copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
            //Its not an success return failure
            if(header_result != e_success){
                printf("Error : Bmp header copy failed\n");
                return e_failure;
            }
            //encode_magic_string
            Status magic_string_result = encode_magic_string("#*", encInfo);
            //Its not an success return failure
            if(magic_string_result != e_success){
                printf("Error: Failed to encode magic string\n");
                return e_failure;
            }

            // encode_secret_file_extension
            int file_extn_size = strlen(".txt");
            Status secret_file_result = encode_secret_file_extn_size(file_extn_size,encInfo);
            //Its not an success return failure
            if(secret_file_result != e_success){
                printf("Error: Failed to encode file extension size\n");
                return e_failure;
            }

            // encode_secret_file_extension
            const char *file_extn = ".txt";
            Status extn_result = encode_secret_file_extn(file_extn, encInfo);
            //Its not an success return failure
            if(extn_result != e_success){
                printf("Error: Failed to encode file extension\n");
                return e_failure;
            }

            // encode_secret_file_size
            uint secret_file_size = encInfo->size_secret_file;
            Status size_result = encode_secret_file_size(secret_file_size, encInfo);
            //Its not an success return failure
            if(size_result != e_success){
                printf("Error: Failed to encode file size\n");
                return e_failure;
            }

            
            // encode_secret_file_data
            Status data_result = encode_secret_file_data(encInfo);
             //Its not an success return failure
            if(data_result != e_success){
                printf("Error: Failed to encode secret file data\n");
                return e_failure;
            }
            
            // Copy remaining image data
            Status copy_result = copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
            //Its not an success return failure
            if(copy_result != e_success){
                printf("Error : Failed to copy remaining image data\n");
                return e_failure;
            }
            
            return e_success;
        }
}

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo){
    printf("## Encoding Procedure Started ##\n");
     // open secret file name in read mode
        encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
        printf("INFO: Checking for secret.txt size\n");
        //Secret pointer null or not
        if(encInfo->fptr_secret == NULL) {
            perror("fopen");
            //Null return failure
            return e_failure;
        }
        printf("INFO: Done. Not Empty\n");
        // open image file in read mode
        encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
        printf("INFO: Checking for SkeletonCode/beautiful.bmp capacity to handle secret.txt\n");
        //checking the src_image pointer is null or not
        if(encInfo->fptr_src_image == NULL) {
            perror("fopen");
            //Null return failure
            return e_failure;
        }
        printf("INFO: Done. Found Ok\n");
        //open stego file name in write mode 
        encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
        //Checking the stego_image is null or not
        if(encInfo->fptr_stego_image == NULL) {
            perror("fopen");
            return e_failure;
        }
        
        return e_success;
}

/* check capacity */
Status check_capacity(EncodeInfo *encInfo){
    uint image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    uint secret_file_size = encInfo->size_secret_file;
    uint src_file_size = 16+32+32+32+(secret_file_size*8);
    if(image_capacity >= src_file_size){
        return e_success;
    }
    else{
        printf("Error: Insufficient capacity in source image\n");
        return e_failure;
    }
}

/* Get file size */
uint get_file_size(FILE *fptr){
    fseek(fptr, 0, SEEK_END);
    uint offset = ftell(fptr);
    rewind(fptr);
    return offset;
}

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){
rewind (fptr_src_image);

char buffer[54];
printf("INFO: Copying Image Header\n");
size_t read = fread(buffer,1,54,fptr_src_image);
size_t write= fwrite(buffer,1,54,fptr_dest_image);
if(read!=54 || write!=54){
    printf("Error :Failed to copy bites from source to destination\n");
    return e_failure;

}
printf("INFO: Done\n");
return e_success;
}

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo){
  printf("INFO: Encoding Magic String Signature\n");
  int res = encode_data_to_image((char *)magic_string,2,encInfo->fptr_src_image,encInfo->fptr_stego_image);

  if(res!=e_success){
    return e_failure;
  }
  printf("INFO: Done\n");
  return e_success;
}

/* Encode secret file extenstion size*/
Status encode_secret_file_extn_size(int file_extn_size, EncodeInfo *encInfo){

    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_int_to_lsb(file_extn_size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    
    return e_success;
}

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo){
    int file_extn_size = strlen(file_extn);
    printf("INFO: Encoding Secret.txt File Extension\n");
    int res =  encode_data_to_image((char *)file_extn,file_extn_size,encInfo->fptr_src_image,encInfo->fptr_stego_image);
   if(res!= e_success){
    return e_failure;
   }
   printf("INFO: Done\n");
   return e_success;
}

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo){
    char buffer[32];
    printf("INFO: Encoding Secret.txt File Size\n");
    fread(buffer,1,32,encInfo->fptr_src_image);
    encode_int_to_lsb(file_size,buffer);
    fwrite(buffer,1,32,encInfo->fptr_stego_image);
    printf("INFO: Done\n");
    return e_success;
}

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo){
    char ch;
    char buffer[8];
    printf("INFO: Encoding Secret.txt File Data\n");

    rewind(encInfo->fptr_secret);

    for(int i=0;i<encInfo->size_secret_file;i++){
        fread(&ch,1,1,encInfo->fptr_secret);
        fread(buffer,1,8,encInfo->fptr_src_image);
        encode_byte_to_lsb(ch,buffer);
        fwrite(buffer,1,8,encInfo->fptr_stego_image);
    }
    printf("INFO: Done\n");
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image){
   char buffer[8];
   for(int i=0;i<size;i++){
    fread(buffer,1,8,fptr_src_image);
    encode_byte_to_lsb(data[i],buffer);
    fwrite(buffer,1,8,fptr_stego_image);
   }
   return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer){
    for(int i = 0; i < 8; i++) {
        char data_bit = (data >>(7-i)) & 1;
        image_buffer[i] = (image_buffer[i] & (~1));
        image_buffer[i] = image_buffer[i] | data_bit;
    }
    
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){
    
    char buffer[1024];
    size_t read_bytes;
    printf("INFO: Copying Left Over Data\n");
    while((read_bytes = fread(buffer,1,sizeof(buffer),fptr_src))>0){
       
       if(fwrite(buffer,1,read_bytes,fptr_dest)!=read_bytes){
            return e_failure;
        }
    }
    printf("INFO: Done\n");
    return e_success;
}

Status encode_int_to_lsb(int data, char *image_buffer){
    for(int i = 0; i < 32; i++) {
        char data_bit = (data >> (31-i)) & 1;
        image_buffer[i] = (image_buffer[i] & (~1));
        image_buffer[i] = image_buffer[i] | data_bit;
    }
    
    return e_success;
}