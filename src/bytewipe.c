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
#include "sys_utils.h"

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


int wipeFile(const char *filepath, size_t buffer_size, int passes, WipeMode mode, int force){

    //PIPELINE:
    // 1. Get exact file size using stat() ok
    // 2. Open file in binary read/write mode ok 
    // 3. Overwrite loop (single or multiple) ok
    // 4. Metadata obfuscation 
    // 5. File rename
    // 6. File removal


    FileInfo info; // Custom structure for file infos

    if(getFileInfo(filepath, &info) != 0){
        fprintf(stderr, "Error: File not found at path: %s. \n", filepath);
        return ERR_FILE_NOT_FOUND;
    }

    if(!info.is_regular){
        fprintf(stderr, "Error: Target is not a regular file. \n");
        return ERR_INVALID_ARGS;
    }

    if(info.is_readonly){
        // If --force isn't passed throws an error
        if(!force){
            fprintf(stderr, "Error: File is read-only. Use --force to attempt a permission change. \n");
            return ERR_PERMISSION_DENIED;
        }
        
        // Attempts a permission change
        if(makeFileWritable(filepath) != 0){
            fprintf(stderr, "Error: Failed to change file permissions. \n");
            return ERR_PERMISSION_DENIED;
        }
    }

    // If file has hardlink, by default the shredding is blocked. If --force is passed this block is bypassed
    if(info.hardlinks > 1 && !force){
        fprintf(stderr, "Error: File has %u hardlinks. Use --force to bypass this security block. \n", info.hardlinks);
        return ERR_INVALID_ARGS;
    }

    long long total_bytes = info.size; // Used to store file size in bytes

    FILE *fp = fopen(filepath, "rb+"); // File is opened in read and write binary mode
    if(!fp){
        return ERR_OPEN_FAILED;
    }

    // Memory allocation for the buffer
    unsigned char *buffer = (unsigned char *)malloc(buffer_size);
    if(!buffer){
        fclose(fp);
        fprintf(stderr, "Error: Impossible to allocate %zu bytes for the buffer. \n", buffer_size);
        return ERR_WRITE_FAILED;
    }

    // If file is empty no overwrite needed but obfuscation is still needed
    if(total_bytes > 0){

        // Passes cycle
        for(int p=0; p < passes; p++){

            // Repositions the cursor to the beginning of the file for the new pass
            if(fseek(fp, 0, SEEK_SET) != 0){
                free(buffer);
                fclose(fp);
                fprintf(stderr, "Error: Impossible to reset cursor position for next pass. \n");
                return ERR_WRITE_FAILED;
            }

            long long remaining_bytes = total_bytes; // Used to track remaining bytes to write

            // File overwrite cycle (i used do...while since at least one write has to be performed)
            do{
                // Calculate the correct amount of bytes to write, to avoid writing outside the file
                size_t actual_bytes_to_write = (remaining_bytes < buffer_size) ? (size_t)remaining_bytes : buffer_size;

                // For zero-mode wipe, memset() is used
                if(mode == WIPE_MODE_ZERO){
                    memset(buffer, 0, actual_bytes_to_write);

                // For random-mode wipe, custom random generation function is used
                } else if (mode == WIPE_MODE_RANDOM){
                    if(generateRandomBytes(buffer, actual_bytes_to_write) != 0){
                        free(buffer);
                        fclose(fp);
                        fprintf(stderr, "Error: An error has occurred while generating random bytes. \n");
                        return ERR_WRITE_FAILED;
                    }
                } else {
                    fprintf(stderr, "Error: Passed wipe mode isn't defined. \n");
                    return ERR_INVALID_ARGS;
                }

                // Actual disk write
                if(fwrite(buffer, 1, actual_bytes_to_write, fp) != actual_bytes_to_write){
                    free(buffer);
                    fclose(fp);
                    fprintf(stderr, "Error: An error has occurred while writing the file. \n");
                    return ERR_WRITE_FAILED;
                }            

                remaining_bytes -= actual_bytes_to_write; // Update remaining bytes to write    

            }while(remaining_bytes > 0);

            // Forces the OS to write onto the disk
            if(syncFileToDisk(fp) != 0) {            
                free(buffer);
                fclose(fp);
                fprintf(stderr, "Error: Failed to synchronize data to physical disk. \n");
                return ERR_WRITE_FAILED; 
            }
        }
    }
    
    // Write operations completed, buffer must be freed and file closed
    free(buffer); 
    fclose(fp);

    // File size obfuscation
    if(truncateFileToZero(filepath) != 0) {
        fprintf(stderr, "Warning: Failed to truncate file size to zero. \n");
    }

    // Timestamp obfuscation
    if(obfuscateFileTimestamps(filepath) != 0) {
        fprintf(stderr, "Warning: Failed to obfuscate file timestamps. \n");
    }

    // Allocation for a copy of the original filepath
    size_t path_len = strlen(filepath);
    char *new_filepath = (char *)malloc(path_len + 1);
    
    // Fallback if memory allocation fails: attempt basic deletion (not very safe)
    if(!new_filepath){        
        remove(filepath);
        fprintf(stderr, "Warning: File rename failed. The file has been deleted without renaming. \n");
        return ERR_RENAME_FAILED;
    }

    strcpy(new_filepath, filepath); // Copy of the original filepath

    // Locate the start of the filename within the full path
    char *last_slash = strrchr(new_filepath, '/');
    char *last_backslash = strrchr(new_filepath, '\\');
    char *filename_start = new_filepath; // Default value if only the filename is passed
    
    // If both '/' and '\' are present, we select the greater address (the last slash)
    if(last_slash && last_backslash) {
        filename_start = (last_slash > last_backslash) ? last_slash + 1 : last_backslash + 1;
    } else if(last_slash) {
        filename_start = last_slash + 1;
    } else if(last_backslash) {
        filename_start = last_backslash + 1;
    }

    size_t name_len = strlen(filename_start);

    // Generate a random name of the exact same length to effectively overwrite directory entries
    generateRandomFilename(filename_start, name_len + 1); // +1 is used because of the terminator char

    const char *path_to_remove = filepath;

    // Renaming of the file
    if(rename(filepath, new_filepath) == 0) {
        path_to_remove = new_filepath; // Update target path for final removal
    } else {
        fprintf(stderr, "Warning: Rename failed. The file will be deleted without renaming. \n");
    }

    // File deletion
    if(remove(path_to_remove) != 0) {
        free(new_filepath);
        fprintf(stderr, "Error: Failed to delete the file from the filesystem. \n");
        return ERR_REMOVE_FAILED;
    }

    free(new_filepath);

    return BYTEWIPE_SUCCESS;
}