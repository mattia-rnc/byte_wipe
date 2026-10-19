#include "sys_utils.h"
#include <sys/stat.h>

int getFileInfo(const char *filepath, FileInfo *info) {
// For Windows uses 64-bit versions
#ifdef _WIN32
    struct __stat64 st;
    if(_stat64(filepath, &st) == 0) {
        info->size = (long long)st.st_size;
        info->is_regular = (st.st_mode & _S_IFREG) != 0;
        info->is_directory = (st.st_mode & _S_IFDIR) != 0;
        return 0;
    }

// On UNIX uses the standard function and structure. On compilation the -D_FILE_OFFSET_BITS=64 flag is passed
#else
    struct stat st;
    if(stat(filepath, &st) == 0){
        info->size = (long long)st.st_size;
        info->is_regular = S_ISREG(st.st_mode) ? 1 : 0;
        info->is_directory = S_ISDIR(st.st_mode) ? 1 : 0;
        return 0;
    }

// Fallback for file not found/unreadable
#endif
    return -1;
}