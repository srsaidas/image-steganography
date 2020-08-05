#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"

/* Function Definitions */

/* 
 * Get operation type
 * Inputs: String pointer
 * Output: Status of operation
 * Return Value: e_enocde or e_decode or e_unsupported
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
		/*Checking source image file is there or in .bmp file*/
		if (argv[D_SOU] == NULL) // is source file is passed
		{
				fprintf(stderr,"ERROR Source image is not specified\n");
				return e_failure;
		}

		if ( (strrchr(argv[D_SOU],'.') == NULL)  || (strcmp(strrchr(argv[D_SOU],'.'),".bmp"))) // is File extension is .bmp
		{
				fprintf(stderr,"ERROR: Source image should be in .bmp format\n");
				return e_failure;
		}

		decInfo->src_image_fname = argv[D_SOU];

		/*Checking for  txt file*/
		if(argv[D_SEC] == NULL) // is secret file is passed
		{
				decInfo->secret_fname = "decoded.txt";
				return e_success;
		}

		if((strrchr(argv[D_SEC],'.') == NULL) ||  (strcmp(strrchr(argv[D_SEC],'.'),".txt")))// is extension is .txt
		{
				fprintf(stderr,"ERROR: Secret FIle should be in .txt format\n");
				return e_failure;
		}
		decInfo->secret_fname = argv[D_SEC];

		return e_success;
}


/* 
 * Open file decode
 * Inputs: decInfo
 * Output: Open files
 * Return Value: e_success or e_failure
 */
Status open_files_decode(DecodeInfo *decInfo)
{
		// Src Image file
		decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "rb");
		//Do Error handling
		if (decInfo->fptr_src_image == NULL)
		{
				perror("fopen");
				fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_image_fname);
				return e_failure;
		}

		printf("INFO: Opened %s\n", decInfo->src_image_fname);
		return e_success;

}

/* 
 * Decode Magic String
 * Inputs: Magic String, DecodeInfo
 * Output: Check whether magic string is  there or note
 * Return Value: e_success or e_failure
 */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
		// Move the file pointer to 54 byte from starting
		fseek(decInfo -> fptr_src_image, 54L, SEEK_SET);
		
		// Created an array to store magic string
		char data[MAGIC_SIZE];

		// Decoded 
		if (decode_data_from_image(data, MAGIC_SIZE, decInfo -> fptr_src_image) == e_failure)
		{
				fprintf(stderr,"ERROR: %s function failed\n", "decode_data_from_image" );
				return e_failure;
		}

		if (strcmp(data,magic_string))
				return e_failure;


		return e_success;
}

Status decode_file_ext_size(DecodeInfo *decInfo)
{
		if (decode_data_from_image((char *)&decInfo->size_secret_file_extn, 4,decInfo -> fptr_src_image) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "encode_data_from_image" );
				return e_failure;
		}
		return e_success;
}
/* 
 * Decode Magic String
 * Inputs: Magic String, DecodeInfo
 * Output: Geting decoded file extension
 * Return Value: e_success or e_failure
 */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
		if(decode_data_from_image(decInfo->extn_secret_file, decInfo->size_secret_file_extn, decInfo -> fptr_src_image) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "encode_data_from_image" );
				return e_failure;
		}

		return e_success;
}

/* 
 * Decode Magic String
 * Inputs: Magic String, DecodeInfo
 * Output: Secret file size
 * Return Value: e_success or e_failure
 */
Status decode_file_size(DecodeInfo *decInfo)
{
		if (decode_data_from_image((char *)&decInfo->size_secret_file, 4,decInfo -> fptr_src_image) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "encode_data_from_image" );
				return e_failure;
		}
		return e_success;
}

/* 
 * Decode Magic String
 * Inputs: Magic String, DecodeInfo
 * Output: Decode file data
 * Return Value: e_success or e_failure
 */
Status decode_file_data(DecodeInfo *decInfo)
{
		int i ;
		for (i = 0; i < decInfo->size_secret_file; i++)
		{
				if (decode_data_from_image(decInfo->secret_data,1,decInfo -> fptr_src_image) == e_failure)
				{
						fprintf(stderr, "ERROR: %s function failed\n", "encode_data_from_image" );
						return e_failure;
				}

				if(!fwrite(decInfo->secret_data,1,1,decInfo->fptr_secret))
						return e_failure;
		}
				return e_success;

}

/* 
 * Decode Magic String
 * Inputs: Magic String, DecodeInfo
 * Output: Decode file data
 * Return Value: e_success or e_failure
 */

Status decode_data_from_image(char *data, int size, FILE *fptr_src_image)
{
		int i ;
		char img_buffer[MAX_IMAGE_BUF_SIZE];

		for (i = 0; i < size; i++)
		{
				if (!fread(img_buffer, sizeof(img_buffer), 1, fptr_src_image))
						return e_failure;

				if (decode_byte_fromlsb(&data[i],img_buffer) == e_failure)
				{
						fprintf(stderr, "ERROR: %s function failed\n", "encode_byte_tolsb" );
						return e_failure;
				}


		}

		return e_success;
}


/* 
 * Decode Magic String
 * Inputs: Magic String, DecodeInfo
 * Output: Decode file data
 * Return Value: e_success or e_failure
 */

Status decode_byte_fromlsb(char * data, char *image_buffer)
{
		*data =0;
		int i;
		for (i = MAX_IMAGE_BUF_SIZE -1 ; i >= 0; i--)
		{
				*data |= (image_buffer[MAX_IMAGE_BUF_SIZE - 1 - i] & 1) << i;
		}

		return e_success;
}
