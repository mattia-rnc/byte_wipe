# ByteWipe
 
ByteWipe is a cross-platform command-line utility written in **C** designed to securely delete files by overwriting their contents, thus preventing data recovery through forensic analysis.


## Features

- **Cryptographically Secure Random Generation**: utilizes native system entropy sources (`BCryptGenRandom` on Windows and `/dev/urandom` on UNIX systems) to generate unpredictable overwrite patterns
- **Cross-Platform Metadata & Safety Checks**: safely inspects target files using abstraction wrappers (`_stat64` on Windows and `lstat` on UNIX systems) to validate regular files, check read-only status, and monitor hardlink counts
- **Flexible Overwriting Modes**: supports multi-pass pseudo-random wiping (1 to 35 rounds) as well as an optional zero-fill mode (`-z`).
- **Configurable Buffer Management**: allows custom buffer sizes for I/O operation optimization (ranging from 512 bytes to 10MB) with a dedicated `--legacy` flag shortcut for 512 bytes
- **Safety Overrides**: includes a force flag (`-f`) to handle multiple hardlinks or attempt permission change when explicitly requested 

- **Metadata Obfuscation**: destroys the file's temporal and structural footprint prior to deletion. Specifically, the tool truncates the file to 0 bytes, resets MAC (Modified, Accessed, Created) timestamps and renames the file

### Feature Focus: Cryptographically Secure Random Generation

Standard random number generators (like the C standard library's `rand()`) are highly predictable and insufficient for secure data sanitization. ByteWipe utilizes cryptographically secure pseudo-random number generators natively provided by the operating system to ensure maximum entropy during the overwriting process.
- **On Windows**: the tool uses the Cryptography Next Generation (CNG) API, specifically `BCryptGenRandom`, to generate secure byte sequences without relying on deprecated libraries
- **On UNIX systems**: the tool reads directly from `/dev/urandom`, which serves as a high-quality entropy pool relying on environmental noise gathered by the kernel


### Feature Focus: Handling Read-Only Files

By default, ByteWipe respects system-level file protections. So, if a target file is marked as read-only, the tool will immediately abort the operation to prevent the accidental destruction of protected data.

However, when the `--force` flag is provided, ByteWipe invokes a cross-platform abstraction to automatically grant the necessary write permissions before attempting the wipe:
- **On Windows**: the tool utilizes the universal C runtime (`_chmod`) to alter the file attributes, actively removing the read-only flag while keeping other file properties intact.
- **On UNIX systems**: the tool queries the current file permission mask using `lstat`. It then applies a bitwise OR operation with the `S_IWUSR` constant and uses `chmod`. This safely grants write access specifically to the file owner, without altering or breaking existing group or external user permissions.

### Feature Focus: Metadata Obfuscation

Simply overwriting a file's content leaves behind structural and temporal footprints. Forensic analysts can use file sizes, original names, and MAC timestamps to reconstruct activity timelines. ByteWipe mitigates this through a three-step obfuscation pipeline executed right before the final unlinking:
1. **Size Obfuscation (Truncation)**: the file is shrunk to 0 bytes, clearing the original size from filesystem records (like the NTFS Master File Table or ext4 inodes)
2. **Timestomping**: the file's access and modification timestamps are forcefully reset to the UNIX Epoch (January 1, 1970), destroying the temporal context of the file's existence and destruction.
3. **Filename Obfuscation**: the file is renamed using a cryptographically random string of the exact same length as the original name. This ensures the original filename is physically overwritten in the directory entry without leaving residual characters

## Project Structure

- `src/main.c`: command-line interface entry point and argument parser
- `src/bytewipe.c`: core wiping logic and orchestration pipeline
- `src/bytewipe.h`: public interface definitions, error codes, and configuration types
- `src/random_gen.c` / `src/random_gen.h`: OS-specific secure random byte generation module
- `src/sys_utils.c` / `src/sys_utils.h`: cross-platform filesystem metadata utilities and wrappers


## Compilation

The project uses a standard GNU Makefile supporting automated cross-platform compilation. To build the project, run:

```bash
make
```

To clean up compiled object files and binaries:

```bash
make clean
```

### Development Environment Notes (Windows):

This project was developed and tested using [MSYS2](https://www.msys2.org/) with the UCRT64 environment. To replicate this setup and compile the project on Windows:

1. Install MSYS2 by following the official guide on the website

2. Install the required GCC compiler and Make toolchain by running the following command in the UCRT64 terminal:
    ```bash
    pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
    ```

3. Navigate to your MSYS2 installation folder (typically `C:\msys64\ucrt64\bin`), locate the make executable (named `mingw32-make.exe`), and rename it to `make.exe`

4. Add that `bin` folder to your Windows system `PATH` environment variable

5. You can now use the make command directly in your terminal to compile the project

On **UNIX** systems, the toolchain can be installed easily via your standard package manager.

## Usage
```bash
bytewipe [options] <file_path>
```
### Options:
`-p, --passes <num>`: number of random overwrite passes to execute (default: 1, max: 35)

`-z, --zero`: overwrite with zeros instead of random data

`-f, --force`: force operation by bypassing safety blocks (such as read-only status or multiple hardlinks)

`-b, --buffer <bytes>`: specify custom buffer size in bytes (clamped between 512 bytes and 10 MB)

`--legacy`: use legacy 512-byte buffer size (shortcut for -b 512)

`-h, --help`: display the help manual and usage instructions

### Examples:
Perform a standard secure wipe with default settings:
```bash
bytewipe top_secret.txt
```

Perform a 3-pass random wipe with forcing execution:
```bash
bytewipe -p 3 -f secret_data.bin
```

Use a legacy 512-byte buffer size with a 2-pass zero-fill:
```bash
bytewipe -z -p 2 --legacy old_file.dat
```

## Important Note SSDs 

Due to the inherent hardware architecture of modern Solid State Drives (SSDs), specifically features like **Wear Leveling**, **TRIM**, and **Over-provisioning**, software-based secure deletion is inherently less predictable than on traditional mechanical hard drives. 

When a program attempts to overwrite a file, the SSD controller may redirect the new data to physically different memory cells to evenly distribute wear. This can leave the original data orphaned in unmapped blocks, which are inaccessible via the file system but, theoretically, recoverable at the hardware level. 

Furthermore, performing multiple overwrite passes on an SSD provides **no** additional security and only **causes unnecessary wear** and degradation to the flash memory cells. 

Therefore, when targeting files on an SSD, it is strongly advised to stick to the default **1 pass** to prevent premature drive wear.

## Roadmap & Planned Features

- **Graphical User Interface (GUI)**: develop a standalone application with a user-friendly graphical interface
- **Directory Wiping**: implement secure deletion for entire folders
- **Multiple Files Support**: allow batch processing to wipe multiple target files in a single command
- **Enhanced CLI**: improve the command-line interface with better formatting, colors, and progress indicators
- **Quiet Mode**: introduce a `--quiet` or `--silent` flag to suppress non-essential output, making the tool easier to integrate into automated scripts
- **Advanced Security Hardening**: implement advanced TOCTOU (Time-of-Check to Time-of-Use) mitigations and further defensive programming strategies

## Possible Future Development

- **Graphical User Interface (GUI)**: develop a standalone application with a user-friendly graphical interface
- **Directory Wiping**: implement secure deletion for entire folders
- **Multiple Files Support**: allow batch processing to wipe multiple target files in a single command
- **Enhanced CLI**: improve the command-line interface with better formatting, colors, and progress indicators
- **Quiet Mode**: introduce a `--quiet` or `--silent` flag to suppress non-essential output, making the tool easier to integrate into automated scripts
- **Advanced Security Hardening**: implement advanced TOCTOU (Time-of-Check to Time-of-Use) mitigations and further defensive programming strategies