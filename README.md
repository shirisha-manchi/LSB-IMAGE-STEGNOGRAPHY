# Image Steganography using C (LSB Technique)

## Project Overview
This project implements **Image Steganography** using the **Least Significant Bit (LSB)** technique in **C programming language**.  
It allows users to **hide a secret text file inside a BMP image** (Encoding) and later **extract the hidden message** from the image (Decoding).

The project works at a **low-level binary format**, modifying only the least significant bits of image data, ensuring that the **visual quality of the image remains unchanged**.

---

## Features
- Encode secret text into a **24-bit BMP image**
- Decode hidden text from the stego image
- Uses **LSB (Least Significant Bit)** technique
- No visible change in image quality
- Command-line based application
- Supports custom output file names
- Proper error handling and validation

---

## Technologies Used
- **Programming Language:** C  
- **Concepts Used:**
  - File handling
  - Bitwise operations
  - Structures and pointers
  - Modular programming

---

## Project Structure
├── main.c
├── encode.c
├── decode.c
├── encode.h
├── decode.h
├── types.h
├── common.h
├── test_encode.bmp
├── secret.txt
└── README.md


---

## Compilation
Compile the project using `gcc`:
gcc *.c -o stego


Encoding (Hide Secret Data)
./stego -e <source.bmp> <secret.txt> [output.bmp]

Decoding (Extract Secret Data)
./stego -d <stego.bmp> [output.txt]

Output
Encoded BMP image containing hidden data
Decoded text file with the original secret message

