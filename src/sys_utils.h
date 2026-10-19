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

#endif