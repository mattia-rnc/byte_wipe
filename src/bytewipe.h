#ifndef BYTEWIPE_H
#define BYTEWIPE_H

/**
 * @file bytewipe.h
 * @brief Header file for the bytewipe.
 */

 #include <stddef.h>

// Error codes returned by the wiping operations
#define BYTEWIPE_SUCCESS         0
#define ERR_INVALID_ARGS        -1
#define ERR_FILE_NOT_FOUND      -2
#define ERR_PERMISSION_DENIED   -3
#define ERR_OPEN_FAILED         -4
#define ERR_WRITE_FAILED        -5
#define ERR_RENAME_FAILED       -6
#define ERR_REMOVE_FAILED       -7

// Enum for supported wipe modes
typedef enum {
    WIPE_MODE_RANDOM,
    WIPE_MODE_ZERO
} WipeMode;

/**
 * @brief Securely overwrites, renames, and deletes a target file.
 * 
 * Performs one or multiple overwrite passes using either cryptographically secure 
 * random data or zeros, obfuscates the filename and metadata, and finally
 * unlinks the file.
 * 
 * @param filepath Path to the file to be securely wiped.
 * @param buffer_size Write buffer size, to optimize for the specific I/O device
 * @param passes Number of random data overwrite passes to execute.
 * @param mode Overwrite mode: WIPE_MODE_RANDOM (default) or WIPE_MODE_ZERO
 * @param force Used to bypass hardlink limits and attempt permission change
 * @return int BYTEWIPE_SUCCESS (0) on success, or a negative error code on failure.
 */
int wipeFile(const char *filepath, size_t buffer_size, int passes, WipeMode mode, int force);

#endif