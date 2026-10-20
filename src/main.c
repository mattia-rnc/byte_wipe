/**
 * @file main.c
 * @brief Command-line interface entry point for ByteWipe.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bytewipe.h"

#define LEGACY_BUF_SIZE 512
#define DEFAULT_BUF_SIZE 4096
#define MAX_BUF_SIZE 10485760 // 10MB
#define MIN_PASSES 1
#define MAX_PASSES 35


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

    int passes = MIN_PASSES;
    WipeMode mode = WIPE_MODE_RANDOM;  // Random data wipe by default
    int force = 0;
    size_t buffer_size = DEFAULT_BUF_SIZE;
    int legacy = 0;

    // Command line arguments parsing
    for(int i=1; i < argc; i++) {

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

        // If argument is -f or --force
        } else if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--force") == 0){
            force = 1;

        // If argument in -b or --buffer
        if(strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--buffer") == 0){
            
            // Skip buffer size setting if --legacy flag was already parsed
            if(legacy){
                fprintf(stdout, "Warning: Skipping %s setting because --legacy flag was already specified. \n", argv[i]);

                if(i+1 < argc) i++; // Used to skip --buffer size number
            
            } else if(i+1 < argc){
                int parsed_size = atoi(argv[i + 1]);
                
                // Clamp the buffer size to minimum and maximum 
                if(parsed_size < LEGACY_BUF_SIZE) {
                    buffer_size = LEGACY_BUF_SIZE;
                } else if(parsed_size > MAX_BUF_SIZE) {
                    buffer_size = MAX_BUF_SIZE;
                } else {
                    buffer_size = parsed_size;
                }

                i++; // Skip the numeric value in the next iteration
            } else {
                fprintf(stderr, "Error: Missing value for buffer size argument.\n");
                return EXIT_FAILURE;
            }

        // If argument is --legacy set the proper buffer size
        } else if(strcmp(argv[i], "--legacy") == 0 ){
            buffer_size = LEGACY_BUF_SIZE;
        
        // If argument is -h or --help prints the command syntax
        } else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
            printf("\nByteWipe: a secure file wiping tool. \n");
            printf("Usage: bytewipe [options] <file_path> \n\n", argv[0]);
            printf("Options: \n");
            printf("  -p, --passes <num>     Number of overwrite passes (default: %d, max: %d) \n", MIN_PASSES, MAX_PASSES);
            printf("  -z, --zero             Overwrite with zeros instead of random data \n");
            printf("  -f, --force            Force operation by bypassing safety blocks \n");
            printf("  -b, --buffer <bytes>   Buffer size in bytes (default: %d) \n", DEFAULT_BUF_SIZE);
            printf("      --legacy           Use legacy %d-byte buffer size (shortcut for -b %d) \n", LEGACY_BUF_SIZE, LEGACY_BUF_SIZE);
            printf("  -h, --help             Display this help manual \n\n ");
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

    int res = wipeFile(target_file, buffer_size, passes, mode, force);
    return (res == BYTEWIPE_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}