#ifndef DECODE_H
#define DECODE_H
#include "common.h"


typedef struct DecodeInfo
{
    /* Encoded image info */
    char *encoded_image_fname;
    FILE *encoded_image_fptr;

    /* Output secret file info */
    char output_fname[30];
    FILE *output_fptr;

} DecodeInfo;


/* Decoding function prototypes */


/* Read and validate Decode args from argv */
Status validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(char *argv[], DecodeInfo *decInfo);

/* Get File pointers for decoded image file */
Status open_image_file(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode 1 byte from LSB of image data array */
char decode_1byte_to_lsb(char *buffer_8);

/* Decode 4byte(int) from LSB of image data array */
int decode_4byte_to_lsb(char *buffer_32);

#endif
