ARFLAGS=rcs
ZZIP_SRC_DIR = external/zziplib/zzip
ZZIP_OBJ_DIR = external/zziplib/zzip
ZZIP_CFLAGS = -O3 -Iexternal/zziplib -Iexternal/zlib -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES
ZZIP_OBJS = $(ZZIP_OBJ_DIR)/err.o $(ZZIP_OBJ_DIR)/dir.o $(ZZIP_OBJ_DIR)/fetch.o $(ZZIP_OBJ_DIR)/file.o $(ZZIP_OBJ_DIR)/fseeko.o $(ZZIP_OBJ_DIR)/info.o $(ZZIP_OBJ_DIR)/plugin.o $(ZZIP_OBJ_DIR)/stat.o $(ZZIP_OBJ_DIR)/write.o $(ZZIP_OBJ_DIR)/zip.o 
ZZIP_TARGET = external/libzzip.a

$(ZZIP_TARGET): $(ZZIP_OBJS)
	$(AR) $(ARFLAGS) $@ $(ZZIP_OBJS)

$(ZZIP_OBJ_DIR)/%.o: $(ZZIP_SRC_DIR)/%.c
	$(CC) $(ZZIP_CFLAGS) -c $< -o $@

clean:
	rm $(ZZIP_OBJS) $(ZZIP_TARGET)

.PHONY: all clean
