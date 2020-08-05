#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"


/* Function Definitions */

/* 
 * Get operation type
 * Inputs: String pointer
 * Output: Status of operation
 * Return Value: e_enocde or e_decode or e_unsupported
 */
OperationType check_operation_type(char * argv[])
{
		if ((argv[E_OPE]) != NULL)
		{
				if (!strcmp(argv[E_OPE],"-e"))
						return e_encode;

				if (!strcmp(argv[E_OPE],"-d"))
						return e_decode;
		}

		return e_unsupported;
}



/* 
 * Read an validate arguments for encode
 * Inputs: String pointer, 
 * Output: Status of operation 
 * Return Value: e_success or e_failure, on file errors
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
		/*Checking source image file is there or in .bmp file*/
		if (argv[E_SOU] == NULL) // is source file is passed
		{
				fprintf(stderr,"ERROR Source image is not specified\n");
				return e_failure;
		}

		if ( (strrchr(argv[E_SOU],'.') == NULL)  || (strcmp(strrchr(argv[E_SOU],'.'),".bmp"))) // is File extension is .bmp
		{
				fprintf(stderr,"ERROR: Source image should be in .bmp format\n");
				return e_failure;
		}

		encInfo->src_image_fname = argv[E_SOU];

		/*Checking for secret txt file*/
		if(argv[E_SEC] == NULL) // is secret file is passed
		{
				fprintf(stderr,"ERROR: Secret txt file is not specified\n");
				return e_failure;
		}

		if((strrchr(argv[E_SEC],'.') == NULL) ||  (strcmp(strrchr(argv[E_SEC],'.'),".txt")))// is extension is .txt
		{
				fprintf(stderr,"ERROR: Secret FIle should be in .txt format\n");
				return e_failure;
		}
		strcpy(encInfo -> extn_secret_file, ".txt");
		encInfo-> size_secret_file_extn = strlen(encInfo -> extn_secret_file);
		encInfo->secret_fname = argv[E_SEC];

		/*Checking for destination file*/
		if (argv[E_DES] == NULL) // is destination file is passed
		{
				encInfo->stego_image_fname = "stego_image.bmp";
				return e_success;
		}

		if ((strrchr(argv[E_DES],'.') == NULL) || (strcmp(strrchr(argv[E_DES],'.'),".bmp"))) // is extension is .bmp
		{
				fprintf(stderr,"ERROR: Image should be in .bmp format\n");
				return e_failure;
		}

		encInfo->stego_image_fname = argv[E_DES];

		return e_success;
}


/* Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
		// Src Image file
		encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
		//Do Error handling
		if (encInfo->fptr_src_image == NULL)
		{
				perror("fopen");
				fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
				return e_failure;
		}

		printf("INFO: Opened %s\n", encInfo->src_image_fname);

		// Secret file
		encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
		//Do Error handling
		if (encInfo->fptr_secret == NULL)
		{
				perror("fopen");
				fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
				return e_failure;
		}

		printf("INFO: Opened %s\n", encInfo->secret_fname);

		// Stego Image file
		encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
		//Do Error handling
		if (encInfo->fptr_stego_image == NULL)
		{
				perror("fopen");
				fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
				return e_failure;
		}

		printf("INFO: Opened %s\n", encInfo->stego_image_fname);

		// No failure return e_success
		return e_success;
}



/* Get file size
 * Input: src_image file  pointer
 * Output: size of file
 * Description: seek pointer to the end of file and check pointer position 
 */
uint get_file_size(FILE *fptr)
{
		fseek(fptr,0L,SEEK_END);
		return ftell(fptr);
}


/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
		uint width, height;
		// Seek to 18th byte
		fseek(fptr_image, 18, SEEK_SET);

		// Read the width (an int)
		fread(&width, sizeof(int), 1, fptr_image);
		//printf("width = %u\n", width);

		// Read the height (an int)
		fread(&height, sizeof(int), 1, fptr_image);
		//printf("height = %u\n", height);

		// Return image capacity
		return width * height * 3;
}

/* Checking image has sufficient size to hold secret data
 * Input: src_image_file,  secret_file
 * Output:  Return value based on the sapce 
 * Return Value: e_success or e_failure, on file errors
 */

Status check_capacity(EncodeInfo *encInfo)
{
		uint text_size; 
		encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
		text_size = MAGIC_SIZE + FILE_EXT_SIZE +encInfo -> size_secret_file_extn + FILE_SIZE +  encInfo->size_secret_file;

		if (encInfo -> image_capacity >= text_size * 8)
				return e_success;

		fprintf(stderr, "ERROR: Image %s has not sufficient size to hold secret data in %s", encInfo -> src_image_fname, encInfo -> secret_fname );
		return e_failure;

}

/* Copy 54 byte header information to destination image 
 * Input:  src_image_file
 * Output:  stego_image_file
 * Description: 54 byte include BMP file information and is copy as such in destination file 
 * Return Value: e_success or e_failure, on file errors
 */
Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_dest_image)
{
		char temp;
		int i = 0;

		// to bring pointer back to starting 
		fseek(fptr_src_image,0L,SEEK_SET);
		for (i = 0; i < 54; i++)
		{
				if(!fread(&temp, sizeof(temp), 1, fptr_src_image))
						return e_failure;

				if(!fwrite(&temp, sizeof(temp), 1, fptr_dest_image))
						return e_failure;

		}
		return e_success;
}

/* Do encoding   
 * Input:  source image, secret text, stego image
 * Output:  stego_image
 * Description: encode secret data inside source image and new image is formed
 * Return Value: e_success or e_failure, on file errors
 */
Status do_encoding(EncodeInfo *encInfo)
{
		
		/* Step 1 Copy header*/
		printf("INFO: Copying image header\n");
		if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "copy_bmp_header" );
				return e_failure;
		}
		printf("INFO: Done\n");


		/*Step 2 Encoding magic string*/
		printf("INFO: Encoding Magic string signature\n");
		if (encode_magic_string(MAGIC_STR, encInfo) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "encode_magic_string");
				return e_failure;
		}
		printf("INFO: Done\n");

		/*step 3  Encoding file extension size*/
		printf("INFO: Encoding %s file extension size\n", encInfo -> secret_fname);
		if(encode_secret_file_ext_size(encInfo -> size_secret_file_extn, encInfo) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "encode_secret_file_ext_size");
				return e_failure;
		}
		printf("INFO: Done\n");

		/*Step 4 Encoding file ext string*/
		printf("INFO: Encoding %s file extension\n", encInfo -> secret_fname);
		if (encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "encode_secret_file_extn");
				return e_failure;
		}
		printf("INFO: Done\n");

		/*step 5  Encoding file size*/
		printf("INFO: Encoding %s file size\n", encInfo -> secret_fname);
		if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "encode_secret_file_size");
				return e_failure;
		}
		printf("INFO: Done\n");



		/*Step 6  Encoding data in file*/
		printf("INFO: Encoding %s file data\n", encInfo -> secret_fname);
		if(encode_secret_file_data(encInfo) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "encode_secret_file_data");
				return e_failure;
		}
		printf("INFO: Done\n");

		/*Step 7 Copy remaining image bytes from src to stego image after encoding */
		if (copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "copy_remaining_img_data");
				return e_failure;
		}

		return e_success;

}
/* Encode Magic String
 * Input:  magic string
 * Output: Encoded image 
 * Description: Will encode magic string to stego_image
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
		if (encode_data_to_image((char *)magic_string, MAGIC_SIZE, encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
		{
				fprintf(stderr,"ERROR: %s function failed\n", "encode_data_to_image" );
				return e_failure;
		}
		return e_success;
}


Status encode_secret_file_ext_size(long file_ext_size, EncodeInfo *encInfo)
{
		if (encode_data_to_image((char *)&file_ext_size, FILE_EXT_SIZE, encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "encode_data_to_image" );
				return e_failure;
		}
		return e_success;
}

/* Encode Secret File Extn
 * Input: secret file extention .txt 
 * Output: Encode image
 * Description: will encode .txt to 4* 8 byte to stego_image
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
		if(encode_data_to_image((char *)file_extn, encInfo-> size_secret_file_extn, encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "encode_data_to_image" );
				return e_failure;
		}

		return e_success;
}

/* Encode Secret File size
 * Input: Secret file size 
 * Output: Encoded image
 * Description: will encode file size in 4 * 8 bytes to stego_image
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
		if (encode_data_to_image((char *)&file_size, FILE_EXT_SIZE, encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "encode_data_to_image" );
				return e_failure;
		}
		return e_success;
}

/* Encode Secret File Data
 * Input:  Data inside secret file 
 * Output: Encoded image
 * Description: encode data inside the secret file to image
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
		char temp [encInfo -> size_secret_file];
		fseek(encInfo->fptr_secret, 0L, SEEK_SET);

		if(!fread(temp, sizeof(temp), 1, encInfo -> fptr_secret))
				return e_failure;

		if(encode_data_to_image(temp, encInfo -> size_secret_file, encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
		{
				fprintf(stderr, "ERROR: %s function failed\n", "encode_data_to_image" );
				return e_failure;
		}


		return e_success;
}
/* Encode function, which does the real encoding 
 * Input: Secret file size 
 * Output: Encoded image
 * Description: will encode file size in 4 * 8 bytes to stego_image
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
		int i ;
		char img_buffer[MAX_IMAGE_BUF_SIZE];

		for (i = 0; i < size; i++)
		{
				if (!fread(img_buffer, sizeof(img_buffer), 1, fptr_src_image))
						return e_failure;

				if (encode_byte_tolsb(data[i],img_buffer) == e_failure)
				{
						fprintf(stderr, "ERROR: %s function failed\n", "encode_byte_tolsb" );
						return e_failure;
				}

				if(!fwrite(img_buffer, sizeof(img_buffer), 1, fptr_stego_image))
						return e_failure;
		}

		return e_success;
}

/* Encode a byte into LSB of image data array 
 * Input: image 
 * Output: Encoded image
 * Description: will encode file size in 4 * 8 bytes to stego_image
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_byte_tolsb(char data, char *image_buffer)
{
		int i;
		for (i = MAX_IMAGE_BUF_SIZE -1 ; i >= 0; i--)
		{
				if (data & 1 << i)
				{
						if (!(image_buffer[ MAX_IMAGE_BUF_SIZE - 1 - i] & 1))
								image_buffer[ MAX_IMAGE_BUF_SIZE - 1 - i] |= 1;
				}
				else
				{
						if(image_buffer[ MAX_IMAGE_BUF_SIZE - 1 - i])
								image_buffer[ MAX_IMAGE_BUF_SIZE - 1 - i] &= ~1;
				}
		}
		return e_success;
}

/* Copy Remaining img date 
 * Input: fptr_src 
 * Output: fptr_desti
 * Description: copying remaing data in source image
 * Return Value: e_success or e_failure, on file errors
 */

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
		char ch;

		while (fread(&ch, 1, 1, fptr_src) > 0)
		{
				if(!fwrite(&ch, 1, 1, fptr_dest))
						return e_failure;
		}
		return e_success;

}
