#include <stdio.h>
#include "encode.h"
//#include "types.h"

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
unsigned int get_image_size_for_bmp(FILE *fptr_image)
{
    unsigned int width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    //to move offset back to index 0
    rewind(fptr_image);
    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: success or failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    //opening src_image (beautiful.bmp) in read mode
    encInfo->src_image_fptr = fopen(encInfo->src_image_fname, "r");

    //if failed to open
    if (encInfo->src_image_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return failure;
    }
    printf("Opened %s\n", encInfo->src_image_fname);

    //opening secret file in read mode
    encInfo->secret_fptr = fopen(encInfo->secret_fname, "r");

    if (encInfo->secret_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return failure;
    }
    printf("Opened %s\n", encInfo->secret_fname);


    //opening output image file in write mode 
    encInfo->output_image_fptr = fopen(encInfo->output_image_fname, "w");

    if (encInfo->output_image_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->output_image_fname);

    	return failure;
    }
    printf("Opened %s\n", encInfo->output_image_fname);

    return success;
}

//To check which operation(Encode/Decode) user wants to perform
Opr_type check_operation(char *option)
{
    if(!strcmp(option, "-e"))
        return encode;
    else if(!strcmp(option, "-d"))
        return decode;
    else
        return unsupported;
}

//encoding
Status do_encoding(char *argv[], EncodeInfo *encInfo)
{
    printf("## Encoding Procedure started ##\n");
    //validating the CLA
    if(validate_encode_args(argv, encInfo) == failure)
        return failure;

    //writing header info to output.bmp
    printf("Copying image header\n");
    if(copy_bmp_header(encInfo) == failure)
        return failure;

    //encoding magic string
    printf("Encoding Magic String Signature\n");
    if(encode_magic_string(MAGIC_STRING, encInfo) == failure)
        return failure;

    //encoding secret file extension
    printf("Encoding %s File Extension\n", encInfo->secret_fname);
    if(encode_secret_file_extn(encInfo) == failure)
        return failure;

    //encoding secret file data
    printf("Encoding %s file data\n", encInfo->secret_fname);
    if(encode_secret_file_data(encInfo) == failure)
        return failure;
    
    //copying remaining data from beautiful.bmp to output.bmp
    printf("Copying leftover Data\n");
    if(copy_remaining_img_data(encInfo) == failure)
        return failure;
    
    printf("## Encoding Done Successfully ##\n");
    return success;
    
}

//VALIDATING CLA
Status validate_encode_args(char * argv[], EncodeInfo * encInfo)
{
    
    //validating argv[2]
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
    printf("Src file is .bmp\n");
    encInfo->src_image_fname = argv[2];

    //validating argv[3]
    char * index = strstr(argv[3],".");
    if(index==NULL)
    {
        printf("Invalid source file\n");
        return failure;
    }
    else if(strlen(index)==1)
    {
        return failure;
    }
    else
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->secret_extn,index);
    }

    //validating argv[4]
    if(argv[4]==NULL)
    {
        encInfo->output_image_fname = "output.bmp";
        printf("Output file not mentioned. Creating output.bmp as default\n");
    }
    else
    {
        ptr = strstr(argv[4],".bmp");
        if(ptr == NULL)
            return failure;
        else
        {
            if(strcmp(".bmp",ptr)!=0)
            {
                return failure;
            }
        }
        encInfo->output_image_fname = argv[4];
    }
    
    //opening all files
    printf("Opening required files\n");
    if(open_files(encInfo) == failure)
    {
        return failure;
    }
    fseek(encInfo->src_image_fptr,0,SEEK_END);
    printf("%ld\n",ftell(encInfo->src_image_fptr));
    rewind(encInfo->src_image_fptr);
    printf("Done\n");
    
    //checking capacity
    if(check_capacity(encInfo) == failure)
        return failure;

    return success;
}

//CHECKING CAPACITY TO MAKE SURE SECRET MSG IS ENCODED WITHOUT ANY LOSS
Status check_capacity(EncodeInfo *encInfo)
{
    printf("Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->output_image_fname);
    encInfo->secret_extn_size = strlen(encInfo->secret_extn);
    encInfo->secret_file_size = get_file_size(encInfo->secret_fptr);

    unsigned long int image_capacity = get_image_size_for_bmp(encInfo->src_image_fptr);
    int encoding_things = (4 + strlen(MAGIC_STRING) + 4 + (encInfo->secret_extn_size) + 4 + (encInfo->secret_file_size) ) * 8;
    
    if(image_capacity < encoding_things)
        return failure;

    
    printf("Done, Found OK\n");
    return success;
}

//TO FIND FILE SIZE
unsigned int get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    int size = ftell(fptr);
    
    rewind(fptr);
    return size;
}

//TO COPY HEADER DATA
Status copy_bmp_header(EncodeInfo *encInfo)
{
    //header info is present in first 54 bytes
    char header[54];
    fread(header,54,1,encInfo->src_image_fptr);
    fwrite(header,54,1,encInfo->output_image_fptr);

    printf("Done\n");
}

//ENCODING 1 BYTE TO LSB
Status encode_1byte_to_lsb(char data, char *buffer_8)
{
    //since 1 byte = 8 bits
    for(int i=7; i>=0; i--)
    {
        buffer_8[7-i] = (buffer_8[7-i] & (~1)) | ((data >> i) & 1);
    }
}

//ENCODING 4 BYTES TO LSB
Status encode_4byte_to_lsb(int data, char *buffer_32)
{
    //since 4 bytes = 32bits
    for(int i=31; i>=0; i--)
    {
        buffer_32[31-i] = (buffer_32[31-i] & (~1)) | ((data >> i) & 1);
    }
}

//ENCODING MAGIC STRING
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    //encoding magic_string length
    char len_buffer[32];
    fread(len_buffer, 32, 1,encInfo->src_image_fptr);
    encode_4byte_to_lsb(strlen(MAGIC_STRING),len_buffer);
    fwrite(len_buffer,32,1,encInfo->output_image_fptr);

    //encoding magic_string data
    char data_buffer[8];
    for(int i=0; i< strlen(MAGIC_STRING); i++)
    {
        fread(data_buffer, 8, 1,encInfo->src_image_fptr );
        encode_1byte_to_lsb(MAGIC_STRING[i],data_buffer);
        fwrite(data_buffer,8,1,encInfo->output_image_fptr);
    }

    return success;

}

//ENCODING SECRET FILE EXTENSION
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    //encoding length of extn
    char len_buffer[32];
    fread(len_buffer, 32, 1,encInfo->src_image_fptr );
    encode_4byte_to_lsb(encInfo->secret_extn_size,len_buffer);
    fwrite(len_buffer,32,1,encInfo->output_image_fptr);

    //ecoding extn
    char data_buffer[8];
    for(int i=0; i< encInfo->secret_extn_size ; i++)
    {
        fread(data_buffer, 8, 1,encInfo->src_image_fptr );
        encode_1byte_to_lsb(encInfo->secret_extn[i],data_buffer);
        fwrite(data_buffer,8,1,encInfo->output_image_fptr);
    }

    return success;

}

//ENCODING SECRET FILE DATA
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //encoding length of secret file
    char len_buffer[32];
    fread(len_buffer, 32, 1,encInfo->src_image_fptr );
    encode_4byte_to_lsb(encInfo->secret_file_size,len_buffer);
    fwrite(len_buffer,32,1,encInfo->output_image_fptr);

    //encoding secret file data
    char data_buffer[8];
    char ch;
    for(int i=0;i<encInfo->secret_file_size;i++)
    {
        fread(data_buffer, 8, 1,encInfo->src_image_fptr );
        //reading character by character from secret file
        fread(&ch, 1, 1, encInfo->secret_fptr);
        encode_1byte_to_lsb(ch,data_buffer);
        fwrite(data_buffer,8,1,encInfo->output_image_fptr);
    }

    return success;

}

//COPYING REMAINING DATA
Status copy_remaining_img_data(EncodeInfo * encInfo)
{
    char ch;
    while(1)
    {
        if(fread(&ch, 1, 1, encInfo->src_image_fptr) == 1)
        {
            fwrite(&ch, 1, 1, encInfo->output_image_fptr);
        }
        else
            break;
    }
    return success;
}

