#CC = aarch64-linux-gnu-gcc

ifeq ($(OS),Windows_NT)
	LDFLAGS = -lWS2_32 -lcrypt32 -lbcrypt
	CFLAGS = -O2 -Wall -Isrc -D_FILE_OFFSET_BITS=64 -DCURL_STATICLIB -Iexternal/curl/include -Iexternal/zziplib/ -Iexternal/zlib
	TARGET = luaxx.exe
else
	LDFLAGS = -lssl -lcrypto -lpthread -ldl -lm
	CFLAGS = -O2 -Wall -Isrc -D_FILE_OFFSET_BITS=64 -DLUA_USE_LINUX -DCURL_STATICLIB -Iexternal/curl/include -Iexternal/zziplib/ -Iexternal/zlib
	TARGET = luaxx
endif

SRC_DIR = src
OBJ_DIR = obj

# List all C source files
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Generate corresponding object file names
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

all: $(TARGET)
	@echo "OS: $(OS)"
	@echo "MSYSTEM: $(SYSTEM)"

# ============== Build external libraries ================
external/libz.a:
	$(MAKE) -f zlib.Makefile

external/libzzip.a:
	$(MAKE) -f zzip.Makefile

external/libcurl.a:
	$(MAKE) -f curl.Makefile
	
# ============== Main Target =============================
$(TARGET): $(OBJS) external/libz.a external/libzzip.a external/libcurl.a
	$(CC) $(CFLAGS) $^ -o $@ -s $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm $(OBJS) $(TARGET)
	$(MAKE) -f zlib.Makefile clean
	$(MAKE) -f zzip.Makefile clean
	$(MAKE) -f curl.Makefile clean

.PHONY: all clean
