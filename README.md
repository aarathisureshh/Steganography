# Steganography System

A C program that hides a secret file inside a BMP image using the least significant bit (LSB) technique. It stores the file’s extension, size, and contents in the image data, then extracts them during decoding.

## Requirements

- GCC or another C compiler
- A 24-bit BMP image

## Build

```bash
gcc -o steganography encode.c decode.c test_encode.c
```

## Usage

Encode a file into an image:

```bash
./steganography -e beautiful.bmp secret.txt stego.bmp
```

Decode the hidden file:

```bash
./steganography -d stego.bmp output.txt
```
