#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "types.h" 
#include"decode.h"

/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
// Check if argv[2] is .bmp file
    char *src_ext = strrchr(argv[2], '.');
    if (src_ext == NULL || strcmp(src_ext, ".bmp") != 0) 
    {
        printf("Error: Source image must be .bmp file\n");
        return e_failure;
    }
    decInfo->stego_image_fname = argv[2];
    
    //check if the argv[3] is present for output file name
    if (argv[3] != NULL) 
    {
        char base_name[100];
        strcpy(base_name, argv[3]);

        char *dot = strrchr(base_name, '.');
        if(dot)
            *dot = '\0';

        decInfo->Output_fname = malloc(strlen(base_name) + 1);
        if(decInfo->Output_fname == NULL)
            return e_failure;

        strcpy(decInfo->Output_fname, base_name);
    }
    
    else {
        decInfo->Output_fname = malloc(strlen("output") + 1);
        if(decInfo->Output_fname == NULL)
            return e_failure;

        strcpy(decInfo->Output_fname, "output");
    }
    
    return e_success;
}

/* Perform the Decoding */
Status do_decoding(DecodeInfo *decInfo)
{
// step 1: open files
Status open_files = open_files_for_decode(decInfo);
if(open_files !=e_success)
{
    return e_failure;
}
// step 2: skip bmp header
Status skip_header = skip_bmp_header(decInfo->fptr_stego_image);
if(skip_header !=e_success)
{
    return e_failure;
}
// step 3: decode magic string
char magic_string[20];
Status magic_result = decode_magic_string(magic_string, decInfo);
if(magic_result != e_success)
{
    return e_failure;
}
// step 4: decode secret file extension size
Status extn_size_result = decode_secret_file_extn_size(decInfo);
if(extn_size_result != e_success)
{
    return e_failure;
}
// step 5: decode secret file extension
char file_extn[10];
Status extn_result = decode_secret_file_extn(file_extn, decInfo);
if(extn_result != e_success)
{
    return e_failure;
}

/* create output file after decoding extension */
char final_name[120];
sprintf(final_name, "%s%s", decInfo->Output_fname, decInfo->extn_output_file);

decInfo->fptr_output = fopen(final_name, "wb");
if(decInfo->fptr_output == NULL)
{
    perror("fopen");
    return e_failure;
}

// step 6: decode secret file size
Status size_result = decode_secret_file_size(decInfo);
if(size_result != e_success)
{
    return e_failure;
}
// step 7: decode secret file data
Status data_result = decode_secret_file_data(decInfo);
if(data_result != e_success)
{
    return e_failure;
}
// step 8: close files
fclose(decInfo->fptr_stego_image);
fclose(decInfo->fptr_output);

return e_success;
}

/*/* Get File pointers for i/p and o/p files */
Status open_files_for_decode(DecodeInfo *decInfo)
{
    printf("INFO: ##Decoding Procedure Started ##\n");
// open image file in read mode
        decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

        //Comparing stego_image to an null
        if(decInfo->fptr_stego_image == NULL) 
        {
            perror("fopen");
            return e_failure;
        }

        printf("INFO: Opened %s\n", decInfo->stego_image_fname);
        
        return e_success;
}

Status skip_bmp_header(FILE *fptr_stego_image)
{

    // skip 54 bytes from SEEK_SET
    fseek(fptr_stego_image,54,SEEK_SET);
return e_success;
}

/* Store Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    // Extract magic string from stego image
    printf("INFO: Decoding Magic String Signature\n");
    Status res = decode_image_to_data(magic_string, 2, decInfo->fptr_stego_image);
    
    if(res != e_success)
    {
        return e_failure;
    }
    
    // Null terminate the string
    magic_string[2] = '\0';
    
    // Verify magic string matches expected value
    if(strcmp(magic_string, "#*") != 0) 
    {
        printf("Error: Invalid magic string. This image may not contain hidden data.\n");
        return e_failure;
    }
    printf("INFO: Done\n");
    return e_success;
}
/* Decode secret file extenstion size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    //after extension 
    //write 32 bytes into stego file
    char buffer[32];
    // Read 32 bytes from stego file
    fread(buffer,1,32,decInfo->fptr_stego_image);
    // call the int to lsb function Status decode_int_to_lsb(file_extn_size, cimage_buffer
    decInfo->extn_size = decode_lsb_to_int(buffer);
    
    return e_success;
}

/* Decode secret file extenstion */
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo)
{
    //size*8 file size
    //create the output.txt and open file
   printf("INFO: Decoding Output File Extension\n");
    int res =  decode_image_to_data(file_extn,decInfo->extn_size,decInfo->fptr_stego_image);
   if(res!= e_success)
   {
    return e_failure;
   }
   // Null terminate extension string
    file_extn[decInfo->extn_size] = '\0';
    
    // Store extension in DecodeInfo structure
    strcpy(decInfo->extn_output_file, file_extn);
    printf("INFO: Done\n");
   return e_success;
 }

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    //file size find the size
    char buffer[32];
    printf("INFO: Decoding File Size\n");
    //Reading #2 bytes from fptr_stego_image
    fread(buffer,1,32,decInfo->fptr_stego_image);
    decInfo->size_output_file = decode_lsb_to_int(buffer);
    
    printf("INFO: Done\n");
    return e_success;
}

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    // Decode secret file data byte by byte
    char ch;
    char buffer[8];
    printf("INFO: Decoding File Data\n");
    for(int i = 0; i < decInfo->size_output_file; i++)
    {
        // Read 8 bytes from stego image
        fread(buffer, 1, 8, decInfo->fptr_stego_image);
        
        // Extract secret byte from LSB
        ch = decode_lsb_to_byte(buffer);
        
        // Write decoded byte to output file
        fwrite(&ch, 1, 1, decInfo->fptr_output);
    }
    printf("INFO: Done\n");
    return e_success;
}

/* Decode function, which extracts data from image */
Status decode_image_to_data(char *data, int size,FILE *fptr_stego_image)
{
//Read the data from file 
//Take one char buffer
   char buffer[8];
   //Using loop
   for(int i=0;i<size;i++)
   {
    //Storing it an buffer
    fread(buffer,1,8,fptr_stego_image);
    //Function call
    data[i] = decode_lsb_to_byte(buffer); 
   }
   return e_success;


}
/* Decode a byte into LSB of image data array */
char decode_lsb_to_byte(char *image_buffer)
{
    char data = 0;
    for(int i = 0; i < 8; i++) 
    {
        // Get the i-th bit from the data byte (0 to 7)
        char data_bit = image_buffer[i] & 1; 
        
        // Set the LSB to the data bit
        data = data|(data_bit<<(7-i));
    }
    return data;
}

//Decode the lsb bit to integer 
int decode_lsb_to_int(char *image_buffer)
{
    int data = 0;                                       
    for(int i = 0; i < 32; i++) 
    {
        // Extract LSB from image buffer
        char data_bit = image_buffer[i] & 1;            
        
        // Reconstruct integer
        data = data | (data_bit << (31-i));          
    }
    return data;
}