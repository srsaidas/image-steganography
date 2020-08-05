#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

#define MAGIC_STR "#*"

#define D_OPE 1
#define D_SOU 2
#define D_SEC 3

#define MAGIC_SIZE 2 //2 character
#define FILE_EXT_SIZE 4 // 4 charcter
#define FILE_SIZE 4 // uint

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file_extn;
    long size_secret_file;


} DecodeInfo;

/*Read and validate*/ 
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);


/* Open files for decode*/
Status open_files_decode(DecodeInfo *decInfo);

/* Decode Magic string*/
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/*Decode file extension size*/
Status decode_file_ext_size(DecodeInfo *decInfo);

/*Decode serect file extension*/
Status decode_secret_file_extn(DecodeInfo *decInfo);

/*Decode file size*/
Status decode_file_size(DecodeInfo *decInfo);

/*Decode file data*/
Status decode_file_data(DecodeInfo *decInfo);

/*Decode data from image*/
Status decode_data_from_image(char *data, int size, FILE *fptr_src_image);

/*Decode byte from lsb*/
Status decode_byte_fromlsb(char *data, char *image_buffer);

#endif
