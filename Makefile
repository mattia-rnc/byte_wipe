# compiler to use
CC = gcc 

# compiler flags:
# -Wall: enable standard warnings
# -Wextra: enable additional warnings
# -std=c99: use the C99 standard
# -O2: optimize for execution speed
CFLAGS = -Wall -Wextra -std=c99 -O2 

# target executable name and source directory
TARGET = bytewipe
SRC_DIR = src

# source and object files
SRCS = $(SRC_DIR)/main.c
OBJS = $(SRCS:.c=.o)

# default rule: compile the whole project
all: $(TARGET)

# link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# compile .c source files into .o object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean generated files (objects and executable)
clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)

# prevent conflicts with files named "all" or "clean"
.PHONY: all clean