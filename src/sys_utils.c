#include "sys_utils.h"
#include <sys/stat.h>

#ifndef _WIN32
#include <unistd.h> // Needed for UNIX constants
#endif

int getFileInfo(const char *filepath, FileInfo *info) {
// For Windows uses 64-bit versions
#ifdef _WIN32
    struct __stat64 st;
    if(_stat64(filepath, &st) == 0) {
        info->size = (long long)st.st_size;
        info->is_regular = (st.st_mode & _S_IFREG) ? 1 : 0;
        info->is_readonly = !(st.st_mode & _S_IWRITE) ? 1 : 0;
        info->hardlinks = (unsigned int)st.st_nlink;
        return 0;
    }    

// On UNIX uses the standard function and structure. On compilation the -D_FILE_OFFSET_BITS=64 flag is passed
#else
    struct stat st;

    // lstat to avoid recursin on symlinks
    if(lstat(filepath, &st) == 0) {
        info->size = (long long)st.st_size;
        info->is_regular = S_ISREG(st.st_mode) ? 1 : 0;
        info->is_readonly = !(st.st_mode & S_IWUSR) ? 1 : 0;
        info->hardlinks = (unsigned int)st.st_nlink;
        return 0;
    }

// Fallback for file not found/unreadable
#endif
    return -1;
}