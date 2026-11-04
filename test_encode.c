#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;//structure declaration
    DecodeInfo decInfo;
    if(argc < 2){
    printf("Enter valid arguments\n");
    return e_failure;
    }
    
    //validate for command line arguments
    OperationType ret = check_operation_type(argv);

    if (ret == e_encode) {
      //encoding
      if (argc < 4){
        printf("Error: Insufficient Arguments for encoding\n");
        return e_failure;
      }
      
      if (read_and_validate_encode_args(argv, &encInfo) == e_success)
      {  
         if (do_encoding(&encInfo) == e_success){
           printf("## Encoding Done Successfully ##\n");
           printf("Source Image: %s\n", encInfo.src_image_fname);
           printf("Secret File: %s\n", encInfo.secret_fname);
           printf("Output Image: %s\n", encInfo.stego_image_fname);
           return e_success;
         }
         else{
            printf("Encoding failed\n");
            return e_failure;
         }
      }
      else{
        printf("Error: Invalid Arguments\n");
        return e_failure;
      }
    }
    else if (ret == e_decode) {
      //decoding
      if (argc < 3){
        printf("Error: Insufficient Arguments for decoding\n");
        return e_failure;
      }
      
      if (read_and_validate_decode_args(argv, &decInfo) == e_success)
      {
         if (do_decoding(&decInfo) == e_success){
           printf("## Decoding Done Successfully ##\n");
           printf("Stego Image: %s\n", decInfo.stego_image_fname);
           printf("Output File: %s\n", decInfo.Output_fname);
           return e_success;
         }
         else{
            printf("Decoding failed\n");
            return e_failure;
         }
      }
      else{
        printf("Error: Invalid Arguments\n");
        return e_failure;
      }
    }
    else{
        printf("Invalid Input\nUnsupported operation: %s\n", argv[1]);
        printf("Supported operations: -e (encode), -d (decode)\n");
        return e_failure;
    }
}
    
OperationType check_operation_type(char *argv[]){
        //check the argv[1] is -e or not
        if(strcmp(argv[1], "-e") == 0) {
        return e_encode;
        }
        //check the argv[1] is -d or not
        else if(strcmp(argv[1], "-d") == 0) {
        return e_decode;
        }
        //Both the conditons are false return e_unsupported
        else {
        return e_unsupported;
        }
}