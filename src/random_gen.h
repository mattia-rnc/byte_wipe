#ifndef RANDOM_GEN_H
#define RANDOM_GEN_H

/**
 * @file random_gen.h
 * @brief Header file for the random byte generation utility.
 */

#include <stddef.h> // Includes size_t, needed for byte dimension type and to prevent overflows

/**
 * @brief Fills a buffer with cryptographically secure random bytes.
 * 
 * Uses BCrypt on Windows and /dev/urandom on UNIX systems.
 * 
 * @param buffer Pointer to the memory array to be filled.
 * @param size Number of bytes to generate.
 * @return int 0 on success, -1 on failure.
 */
int generateRandomBytes(unsigned char *buffer, size_t size);

#endif