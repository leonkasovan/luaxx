MSYSTEM := $(shell uname)
ifeq ($(findstring MINGW,$(MSYSTEM)),MINGW)
	LDFLAGS = -lWS2_32 -lcrypt32 -lbcrypt -lcurl -lz -lzzip
	CFLAGS = -O2 -Wall -Isrc -D_FILE_OFFSET_BITS=64 -Iexternal/curl/include -Iexternal/zziplib/ -Iexternal/zlib
	TARGET = luaxx.exe
else
	LDFLAGS = -lssl -lcrypto -lpthread -ldl -lm -lcurl -lz -lzzip -lzstd
	CFLAGS = -O2 -Wall -Isrc -D_FILE_OFFSET_BITS=64 -DLUA_USE_LINUX -Iexternal/curl/include -Iexternal/zziplib/ -Iexternal/zlib
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

# ============== Main Target =============================
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -s $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm $(OBJS) $(TARGET)
	rm -r obj/

.PHONY: all clean
