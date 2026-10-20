#include "sys_utils.h"
#include <sys/stat.h>

#ifdef _WIN32
    #include <io.h>
    #include <sys/utime.h> // Required for _utime and struct _utimbuf on Windows
#else
    #include <unistd.h>    // Required for UNIX functions  such as truncate and fsync
    #include <utime.h>     // Required for utime and struct utimbuf on UNIX
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


int syncFileToDisk(FILE *fp) {

    if(!fp) {
        return -1;
    }
    
    // Uses the standard library force the writing
    if(fflush(fp) != 0) {
        return -1;
    }

// OS-specific functions to commit to the disk
#ifdef _WIN32
    int fd = _fileno(fp);
    if(fd == -1) {
        return -1;
    }
    return (_commit(fd) == 0) ? 0 : -1;

#else
    int fd = fileno(fp);
    if(fd == -1) {
        return -1;
    }
    return (fsync(fd) == 0) ? 0 : -1;
#endif
}


int truncateFileToZero(const char *filepath) {
    
    if(!filepath) {
        return -1;
    }

#ifdef _WIN32
    // Opening in "wb" mode natively truncates an existing file to 0 bytes on Windows
    FILE *fp = fopen(filepath, "wb");
    if(!fp) return -1;
    fclose(fp);
    return 0;
#else
    // Native UNIX system call to shrink file size to 0
    return (truncate(filepath, 0) == 0) ? 0 : -1;
#endif
}


int obfuscateFileTimestamps(const char *filepath) {
    if(!filepath) {
        return -1;
    }

// Set last access and modification time to UNIX epoch (1970-01-01 00:00:00), using the OS-specific function
#ifdef _WIN32
    struct _utimbuf times;
    times.actime = 0;  
    times.modtime = 0;
    return (_utime(filepath, &times) == 0) ? 0 : -1;
#else
    struct utimbuf times;
    times.actime = 0;
    times.modtime = 0;
    return (utime(filepath, &times) == 0) ? 0 : -1;
#endif
}