#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include "common.h"



#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char magic_string[3];

    /* Output File Info */
    char *Output_fname;
    FILE *fptr_output;
    long  extn_size;
    char extn_output_file[MAX_FILE_SUFFIX];
    long size_output_file;

} DecodeInfo;


/* Decoding function prototype */


/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the Decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_for_decode(DecodeInfo *decInfo);



Status skip_bmp_header(FILE *fptr_src_image);

/* Store Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);
    

/* Decode secret file extenstion size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function, which does the real encoding */
Status decode_image_to_data(char *data, int size,FILE *fptr_stego_image);

/* Decode a byte into LSB of image data array */
char decode_lsb_to_byte(char *image_buffer);

int decode_lsb_to_int(char *image_buffer);

#endif
