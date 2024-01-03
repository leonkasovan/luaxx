CC = gcc
CFLAGS = -O2 -Wall -Isrc -D_FILE_OFFSET_BITS=64 -DCURL_STATICLIB -Iexternal/curl/include -Iexternal/zziplib/
SRC_DIR = src
OBJ_DIR = obj

# List all C source files
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Generate corresponding object file names
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Final executable name
TARGET = luaxx

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -s external/curl/lib/.libs/libcurl.a external/zlib/libzlibstatic.a external/zziplib/zzip/libzzip.a -lWS2_32 -lcrypt32 -lbcrypt

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)/* $(TARGET)

.PHONY: all clean
