#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{

		EncodeInfo encInfo;
		DecodeInfo decInfo;
		uint img_size;
		/*Checking for operation*/
		if(check_operation_type(argv) == e_encode)
		{
				/*Step 1 Validating command line arguments*/
				if (read_and_validate_encode_args(argv, &encInfo) == e_failure)
				{
						printf("ERROR: %s function failed \n", "read_and_validate_encode_args");
						return 1;
				}

				/*Step 2 Opening FIles*/
				printf("INFO: Opening required files\n");

				if (open_files(&encInfo) == e_failure)
				{
						printf("ERROR: %s function failed\n", "open_files" );
						return 1;
				}
				printf("INFO: Done\n");

				/*Step 3 Checking secret file is empty or not*/
				printf("INFO: Checking for %s size\n", encInfo.secret_fname);
				if (!(encInfo.size_secret_file = get_file_size(encInfo.fptr_secret)))
				{
						fprintf(stderr, "ERROR: %s is empty", encInfo.secret_fname);
						return e_failure;
				}
				printf("INFO: Done. Not empty\n");

				/*Step 4 Checking image will fit or not*/
				printf("INFO: Checking for %s has space to hold %s\n",encInfo.src_image_fname, encInfo.secret_fname);
				if(check_capacity(&encInfo) == e_failure)
				{
						fprintf(stderr, "ERROR: %s function failed\n", "check_capcaity" );
						return e_failure;
				}
				printf("INFO: Done. Found OK\n");

				/*Step 3 Checking for output filei*/
				if (argv[E_DES] == NULL)
				{
						printf("INFO: Output FIle is not mentioned, Creating %s as default\n", encInfo.stego_image_fname);
				}

				/*Step 5 Encoding */
				printf("INFO: ## Encoding Procedure Started ##\n");
				if (do_encoding(&encInfo) == e_failure)
				{
						printf("ERROR: %s function failed\n", "do_encoding" );
						return 1;

				}

				fclose(encInfo.fptr_src_image);
				fclose(encInfo.fptr_secret);
				fclose(encInfo.fptr_stego_image);
				printf("INFO: ## Encoding done sucessfully ##\n");

		}
		else if(check_operation_type(argv) == e_decode)
		{
				/*Step 1 Validating command line arguments*/
				if (read_and_validate_decode_args(argv, &decInfo) == e_failure)
				{
						printf("ERROR: %s function failed \n", "read_and_validate_encode_args");
						return 1;
				}

				printf("INFO: ## Decoding Procedure Started ##\n");

				/*Step 2 Open Files*/
				printf("INFO: Opening required files \n");
				if (open_files_decode(&decInfo) == e_failure)
				{
						printf("ERROR: %s function failed \n", "read_and_validate_encode_args");
						return 1;
				}
				printf("INFO: Done\n");

				/*Step 3 Decode Magic string*/

				printf("INFO: Decoding Magic String Signature\n");
				if (decode_magic_string(MAGIC_STR, &decInfo) == e_failure)
				{
						printf("ERROR: %s function failed \n", "decode_magic_string");
						return 1;
				}

				printf("INFO: Done\n");

				printf("INFO: Decoding File extension Size\n");

				if (decode_file_ext_size(&decInfo) == e_failure)
				{
						printf("ERROR: %s function failed \n", "decode_magic_string");
						return 1;
				}

				printf("INFO: Done\n");
				/*Step 4 Decode file extension*/

				printf("INFO: Decoding file extension \n");
				if (decode_secret_file_extn(&decInfo) == e_failure)
				{
						printf("ERROR: %s function failed \n", "decode_magic_string");
						return 1;
				}

				printf("INFO: Done\n");

				/*Step 5 Creating file */
				if (argv[D_SEC] == NULL)
				{
						printf("INFO: Output File not mentioned, Creating decoding.txt as default\n");
				}
				// Secret file
				decInfo.fptr_secret = fopen(decInfo.secret_fname, "w");
				//Do Error handling
				if (decInfo.fptr_secret == NULL)
				{
						perror("fopen");
						fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo.secret_fname);
						return 1;
				}

				printf("INFO: Opened %s\n", decInfo.secret_fname);

				printf("INFO: Done. Opened all required files\n");


				/*Step 5 Decode file size extension*/
				printf("INFO: Decoding File Size\n");

				if (decode_file_size(&decInfo) == e_failure)
				{
						printf("ERROR: %s function failed \n", "decode_magic_string");
						return 1;
				}

				printf("INFO: Done\n");


				printf("INFO: Decoding File Data\n");
				if (decode_file_data(&decInfo) == e_failure)
				{
						printf("ERROR: %s function failed \n", "decode_magic_string");
						return 1;
				}
				printf("INFO: Done\n");
				printf("INFO: ## Decoding Done Successfully ##\n");				

		}

		else 
		{
				printf("ERROR: unsupported operation\n");
				return 1;
		}
		return 0;
}
