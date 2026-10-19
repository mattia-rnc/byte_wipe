/**
 * @file main.c
 * @brief Command-line interface entry point for ByteWipe.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bytewipe.h"

/**
 * @brief Main entry point of the application.
 * 
 * Parses command-line arguments and triggers the secure deletion procedure.
 * 
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int main(int argc, char *argv[]){

    // Validates command-line arguments
    if(argc < 2){
        fprintf(stderr, "Use '%s <file_path> [-p passes ] [-z]' instead! \n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *target_file = NULL;
    int passes = 1; // Minimum 1 pass
    WipeMode mode = WIPE_MODE_RANDOM;  // Random data wipe by default

    // Command line arguments paring
    for (int i=1; i < argc; i++) {

        // If argument in -p or --pass
        if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--passes") == 0){
            
            // Check if exists a next argument (the number of passes)
            if(i+1 < argc){
                passes = atoi(argv[i + 1]); // String to integer convertion of the number
                
                // Passes must be >= 1
                if(passes <= 0) {
                    fprintf(stderr, "Error: Number of passes must be greater than zero. \n");
                    return EXIT_FAILURE;
                }

                i++; // Skip the numeric value in the next iteration
            } else {
                fprintf(stderr, "Error: Missing value for passes argument.\n");
                return EXIT_FAILURE;
            }

        // If argument is -z or --zero
        } else if(strcmp(argv[i], "-z") == 0 || strcmp(argv[i], "--zero") == 0){
            mode = WIPE_MODE_ZERO; // Set wipe mode to zero
        
        // If argument is -h or --help prints the command syntax
        } else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
            printf("\nByteWipe a Secure File Deletion Tool \n");
            printf("Usage: %s <file_path> [options] \n\n", argv[0]);
            printf("Options: \n");
            printf("  -p, --passes <num>   Number of overwrite passes (default: 1) \n");
            printf("  -z, --zero           Overwrite with zeros instead of random data \n");
            printf("  -h, --help           Display this help message and exit \n\n ");
            return EXIT_SUCCESS;

        // If argument doesn't start with a flag
        } else {            
            // If no target file is set yet, treats it as the target file path
            if(target_file == NULL) {
                target_file = argv[i];

            // If a target file is already set, the argument is unexpected
            } else {
                fprintf(stderr, "Error: Unexpected argument '%s'. \n", argv[i]);
                return EXIT_FAILURE;
            }
        }
    }

    // File path must be provided
    if(target_file == NULL) {
        fprintf(stderr, "Error: No target file specified. \n");
        return EXIT_FAILURE;
    }


    wipeFile(target_file, passes, mode);
}