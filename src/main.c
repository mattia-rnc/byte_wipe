/**
 * @file main.c
 * @brief Command-line interface entry point for ByteWipe.
 */

#include <stdio.h>
#include <stdlib.h>

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

    // validate command-line arguments
    if(argc < 2){
        fprintf(stderr, "Use '%s <file_path>' instead! \n", argv[0]);
        return EXIT_FAILURE;
    }
}