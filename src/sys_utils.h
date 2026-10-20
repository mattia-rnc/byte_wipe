#include <stdio.h>

#ifndef SYS_UTILS_H
#define SYS_UTILS_H

/**
 * @brief Custom structure used to store file infos while being OS-independant
 */
typedef struct {
    long long size;         // Used to store the size of the file in bytes
    int is_regular;         // Flag set to 1 if the path points to a regular file (safe to wipe), 0 otherwise
    int is_readonly;        // Flag set to 1 if the user lacks write permissions, 0 otherwise
    unsigned int hardlinks; // Number of hard links pointing to the same data block
} FileInfo;

/**
 * @brief Obtains file infos using the OS-appropriate functions.
 * @param filepath File to be analyzed.
 * @param info Pointer to the FileInfo structure to be populated.
 * @return int 0 on success, -1 on failure.
 */
int getFileInfo(const char *filepath, FileInfo *info);

/**
 * @brief Forces the OS to physically write file buffers to the disk.
 * 
 * By default, standard I/O operations and the OS use RAM caching to improve
 * performance. This function ensures that the data currently in the buffer
 * is actually flushed to the physical storage hardware, bypassing the OS page cache.
 * This is critical to prevent the OS from discarding overwritten data if the
 * file is deleted immediately after writing.
 * 
 * @param fp Pointer to the FILE object stream to be synchronized.
 * @return int 0 on success, -1 on failure.
 */
int syncFileToDisk(FILE *fp);

/**
 * @brief Truncates the file size to 0 bytes to obfuscate the original size in filesystem metadata.
 * @param filepath Path to the target file.
 * @return int 0 on success, -1 on failure.
 */
int truncateFileToZero(const char *filepath);

/**
 * @brief Resets file access and modification timestamps.
 * @param filepath Path to the target file.
 * @return int 0 on success, -1 on failure.
 */
int obfuscateFileTimestamps(const char *filepath);

#endif