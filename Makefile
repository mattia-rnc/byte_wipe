# Compiler to use
CC = gcc 

# Compiler flags:
# -Wall: enable standard warnings
# -Wextra: enable additional warnings
# -std=c99: use the C99 standard
# -O2: optimize for execution speed
CFLAGS = -Wall -Wextra -std=c99 -O2 

# Target executable name and source directory
TARGET = bytewipe
SRC_DIR = src

# Source and object files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/bytewipe.c $(SRC_DIR)/random_gen.c
OBJS = $(SRCS:.c=.o)

# OS-specific settings:
# LDFLAGS = -lbcrypt: needed for random generation
# CLEAN_CMD: custom command for file removal 
ifeq ($(OS),Windows_NT)
	LDFLAGS = -lbcrypt
	CLEAN_CMD = del /q /f src\*.o $(TARGET).exe
else
	LDFLAGS =
	CLEAN_CMD = rm -f $(OBJS) $(TARGET)
endif

# Default rule: compile the whole project
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile .c source files into .o object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean generated files (objects and executable)
clean:
	$(CLEAN_CMD)

# Prevent conflicts with files named "all" or "clean"
.PHONY: all clean