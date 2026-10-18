/**
 * @file bytewipe.c
 * @brief Implementation of the secure file wiping logic.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "bytewipe.h"
#include "random_gen.h"

#define BUFFER_SIZE 4096

/**
 * @brief Generates a random alphanumeric string for filename obfuscation.
 * 
 * @param str Buffer to store the random string.
 * @param size Size of the buffer.
 */
static void generateRandomFilename(char *str, size_t size) {
    const char CHARSET[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    unsigned char rand_byte;
    
    for(size_t i=0; i < size-1; i++){ // size-1 to terminate before string termination char
        
        generateRandomBytes(&rand_byte, 1); // Generate a random byte to pick a character from the charset
        str[i] = CHARSET[rand_byte % (sizeof(CHARSET) - 1)]; // -1 due to string termination char
    }
    str[size-1] = '\0';
}

int wipeFile(const char *filepath, int passes, WipeMode mode) {

    // REQUIRED CHECKS (done in main):
    // - passes: min 1, deafult 1
    // - mode: default random

    //PIPELINE:
    // 1. Get exact file size using stat()
    // 2. Open file in binary read/write mode 
    // 3. Overwrite loop (single or multiple)
    // 4. Metadata obfuscation
    // 5. File rename
    // 6. File removal

}