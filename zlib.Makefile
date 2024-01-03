# CC=aarch64-linux-gnu-gcc
# AR=ar
ARFLAGS=rcs

# ======= ZLib Library ==============
LIBZ_SRC_DIR = external/zlib
LIBZ_OBJ_DIR = external/zlib
LIBZ_CFLAGS = -O3 -D_LARGEFILE64_SOURCE=1 -DHAVE_HIDDEN
LIBZ_OBJS = $(LIBZ_OBJ_DIR)/adler32.o $(LIBZ_OBJ_DIR)/crc32.o $(LIBZ_OBJ_DIR)/deflate.o $(LIBZ_OBJ_DIR)/infback.o $(LIBZ_OBJ_DIR)/inffast.o $(LIBZ_OBJ_DIR)/inflate.o $(LIBZ_OBJ_DIR)/inftrees.o $(LIBZ_OBJ_DIR)/trees.o $(LIBZ_OBJ_DIR)/zutil.o $(LIBZ_OBJ_DIR)/compress.o $(LIBZ_OBJ_DIR)/uncompr.o $(LIBZ_OBJ_DIR)/gzclose.o $(LIBZ_OBJ_DIR)/gzlib.o $(LIBZ_OBJ_DIR)/gzread.o $(LIBZ_OBJ_DIR)/gzwrite.o
LIBZ_TARGET = external/libz.a

$(LIBZ_TARGET): $(LIBZ_OBJS)
	$(AR) $(ARFLAGS) $@ $(LIBZ_OBJS)

$(LIBZ_OBJ_DIR)/%.o: $(LIBZ_SRC_DIR)/%.c
	$(CC) $(LIBZ_CFLAGS) -c $< -o $@

clean:
	rm $(LIBZ_OBJS) $(LIBZ_TARGET)