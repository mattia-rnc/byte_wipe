/**
 * @file random_gen.c
 * @brief Utility to generate cryptographically secure random bytes on Windows and UNIX systems.
 */

#include "random_gen.h"
#include <stddef.h> // Includes size_t, needed for byte dimension type and to prevent overflows, and NULL

#ifdef _WIN32 
#include <windows.h> // Needed for basic Windows API and functionalities
#include <bcrypt.h> // Needed for safe random generation
#else
#include <stdio.h>
#endif

int generateRandomBytes(unsigned char *buffer, size_t size) {
#ifdef _WIN32
    // On Windows Cryptography Next Generation API is used
    if(BCryptGenRandom(NULL, buffer, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG) == 0) {
        return 0;
    }
    return -1;
#else
    // ON UNIX uses /dev/urandom
    FILE *f = fopen("/dev/urandom", "rb");
    if(!f) {
        return -1;
    }
    size_t read_bytes = fread(buffer, 1, size, f);
    fclose(f);
    
    return (read_bytes == size) ? 0 : -1;
#endif
}