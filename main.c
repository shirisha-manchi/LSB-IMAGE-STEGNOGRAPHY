/*
NAME            :   Shirisha Manchi
DATE            :   23/01/2026
DESCRIPTION     :   The LSB Image Steganography project focuses on securely hiding confidential information inside digital images. 
                    Instead of sending secret data directly, which can be easily detected, this project embeds the data into a BMP image using the Least 
                    Significant Bit (LSB) technique. 
                    In this method, the least significant bits of image pixel bytes are altered to store secret file information without causing noticeable 
                    changes to the image’s appearance.
                    The project is implemented in C programming and supports two main operations: encoding and decoding. 
                    During encoding, the program first checks whether the selected image has enough capacity to hold the secret file. 
                    It then stores a magic string for identification, followed by the secret file’s extension, size, and actual data into the image’s pixel bytes. 
                    The image header is preserved to maintain file integrity.
                    During decoding, the program reads the encoded image, verifies the magic string to confirm the presence of hidden data, extracts the 
                    secret file details, and reconstructs the original secret file accurately. The project uses efficient bitwise operations and file handling 
                    techniques to manipulate binary data at a low level.
                    Overall, this project demonstrates how steganography can be used for secure data transmission, ensuring hidden communication while keeping 
                    the carrier image visually unchanged.
*/

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "common.h"

int main(int argc, char *argv[])
{
    //structure variables
    EncodeInfo encodeInfo;
    DecodeInfo decodeInfo;
    
    if( argc == 1 )
    {

        printf("Invalid no of commands\n");
        printf("Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
        printf("Decoding: ./a.out -d <.bmp file> [output file]\n");
	    return 0;
    }

    //to know which operation user wants do
    int opr = check_operation(argv[1]);
    //encoding
    if( opr == encode )
    {
        if(argc < 4 || argc > 5)
        {
            printf("ERROR: Invalid number of arguments\n");
            printf("Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
            printf("Decoding: ./a.out -d <.bmp file> [output file]\n");
            return 0;
        }
        //encoding fun call
        if(do_encoding(argv, &encodeInfo)==failure)
        {
            printf("read and validate is not successfully done.\n");
            return 0;
        }

    }
    //decoding
    else if( opr == decode )
    {
        if(argc < 3 || argc > 4)
        {
            printf("ERROR: Invalid number of arguments\n");
            printf("Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
            printf("Decoding: ./a.out -d <.bmp file> [output file]\n");
            return 0;
        }

        if(do_decoding(argv, &decodeInfo)==failure)
        {
            printf("ERROR: Invalid CLA\n");
            return 0;
        }

    }
    else
    {
        printf("Invalid Operation.\n");
        printf("Please pass either -e  for editing -d for decoding\n");
    }
    return 0;
}
