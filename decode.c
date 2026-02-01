#include <stdio.h>
#include "decode.h"

//Decoding
Status do_decoding(char *argv[], DecodeInfo *decInfo)
{
    printf("## Decoding Procedure Started ##\n");
    //vaidating decoding arguments
    if(validate_decode_args(argv, decInfo) == failure)
        return failure;
    
    //opening encoded image file
    printf("Opening required files\n");
    if(open_image_file(decInfo) == failure)
        return failure;
    
    //decoding magic string (length + data)
    printf("decoding Magic String\n");
    if(decode_magic_string(decInfo) == failure)
        return failure;
    
    //decoding secret file extension (length + extension)
    printf("Decoding secret file extension\n");
    if(decode_secret_file_extn(decInfo) == failure)
        return failure;
    
    //decoding secret file (length + data)
    printf("Decoding secret file data\n");
    if(decode_secret_file_data(decInfo) == failure)
        return failure;

    printf("## Decoding Procedure Completed ##\n");
    return success;
}

//VALIDATING DECODING ARGUMENTS
Status validate_decode_args(char * argv[], DecodeInfo * decInfo)
{
    //validating arg3
    char * ptr = strstr(argv[2],".bmp");
    if(ptr == NULL)
        return failure;
    else
    {
        if(strcmp(".bmp",ptr)!=0)
        {
            return failure;
        }
    }
    printf("It's a .bmp file\n");
    decInfo->encoded_image_fname = argv[2];

    //validating arg4
    if(argv[3] == NULL)
    {
        printf("Output file is not mentioned. Creating output.txt as secret file\n");
        argv[3] = strcpy(decInfo->output_fname,"output");
    }
    else
    {
        char output[20];
        for(int i=0;i<strlen(argv[3]);i++)
        {
            if(argv[3][i] != '.')
            {
                output[i] = argv[3][i];
            }
            else
                break;
        }
        strcpy(decInfo->output_fname,output);
    }
    
    return success;
}

//OPENING ENCODED IMAGE FILE
Status open_image_file(DecodeInfo *decInfo)
{
    decInfo->encoded_image_fptr = fopen(decInfo->encoded_image_fname, "r");
    if(decInfo->encoded_image_fptr == NULL)
    {
        printf("No encoded file\n");
        return failure;
    }
    printf("Opened %s\n",decInfo->encoded_image_fname);
    printf("Done\n");
    return success;
}

//DECODING MAGIC STRING
Status decode_magic_string(DecodeInfo *decInfo)
{
    fseek(decInfo->encoded_image_fptr, 54, SEEK_SET);
    char buffer_32[32];
    fread(buffer_32,32,1,decInfo->encoded_image_fptr);
    int magic_str_len = decode_4byte_to_lsb(buffer_32);

    char buffer_8[8];
    char magic_str[magic_str_len + 1];
    for(int i=0; i<magic_str_len; i++)
    {
        fread(buffer_8, 8, 1, decInfo->encoded_image_fptr);
        magic_str[i] = decode_1byte_to_lsb(buffer_8);
    }
    magic_str[magic_str_len] = '\0';

    if(strcmp(magic_str, MAGIC_STRING) != 0)
    {
        printf("Magic string is not decoding successfully\n");
        return failure;
    }

    printf("Done\n");
    return success;

}

//DECODING SECRET FILE EXTENSION
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer_32[32];
    fread(buffer_32,32,1,decInfo->encoded_image_fptr);
    int extn_len = decode_4byte_to_lsb(buffer_32);

    char file_extn[extn_len + 1];
    char buffer_8[8];
    for(int i=0; i<extn_len ; i++)
    {
        fread(buffer_8, 8, 1, decInfo->encoded_image_fptr);
        file_extn[i] = decode_1byte_to_lsb(buffer_8);
    }
    file_extn[extn_len] = '\0';

    strcpy(decInfo->output_fname ,strcat(decInfo->output_fname, file_extn));
    
    decInfo->output_fptr = fopen(decInfo->output_fname, "w");
    if(decInfo->output_fptr == NULL)
        return failure;
    printf("Done\n");
    return success;
}

//DECODING SECRET FILE DATA
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer_32[32];
    fread(buffer_32,32,1,decInfo->encoded_image_fptr);
    int sec_file_len = decode_4byte_to_lsb(buffer_32);

    char ch;
    for(int i=0;i<sec_file_len;i++)
    {
        char buffer_8[8];
        fread(buffer_8, 8, 1, decInfo->encoded_image_fptr);
        ch = decode_1byte_to_lsb(buffer_8);
        fwrite(&ch, 1,1,decInfo->output_fptr);
    }
    printf("Done\n");
    return success;
}

//DECODING 1 BYTE FROM LSB
char decode_1byte_to_lsb(char *buffer_8)
{
    //since 1 byte = 8 bits
    char ch =0;
    for(int i=7; i>=0; i--)
    {
        ch =((buffer_8[7-i] & 1) << i )| ch;
    }
    return ch;
}

//DECODING 4 BYTES FROM LSB
int decode_4byte_to_lsb(char *buffer_32)
{
    int ch =0;
    for(int i=31; i>=0; i--)
    {
        ch =((buffer_32[31-i] & 1) << i )| ch;
    }
    return ch;
}