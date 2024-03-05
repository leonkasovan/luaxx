#!/bin/bash

# Build luaxx for arm/arm64 arch with static library: zlib, zzip and curl
# pre: sudo apt install gcc-aarch64-linux-gnu automake autoconf cmake make libtool
# pre: sudo apt install libssl-dev libssl-dev:arm64

# pre: (select one)
# source ~/start_recalbox_toolchain
# source ~/start_koriki_toolchain
# export CC=aarch64-linux-gnu-gcc
# export CC=/home/ark/recalbox-rg353x/output/host/bin/aarch64-buildroot-linux-gnu-gcc

cd external

cd zlib
cmake .
cd ..

cd curl
os_name=$(uname -s)
if [[ "$os_name" == *"CYGWIN"* ]] || [[ "$os_name" == *"MINGW"* ]] || [[ "$os_name" == *"MSYS"* ]]; then
	autoreconf -fi
	./configure --with-schannel --without-brotli --without-nghttp2 --without-libidn2 --without-libpsl --without-zstd --disable-alt-svc --disable-hsts --disable-tls-srp --disable-proxy --disable-ipv6 --disable-ntlm-wb --disable-ntlm --disable-dict --disable-gopher --disable-gophers --disable-imap --disable-mqtt --disable-pop3 --disable-pop3s --disable-smtp --disable-smtps --disable-telnet --disable-tftp --disable-rtsp --disable-file --disable-ldap --disable-ldaps --disable-pthreads  --without-librtmp --disable-manual --with-zstd
	sed -i 's/^#define USE_UNIX_SOCKETS 1/\/\/#define USE_UNIX_SOCKETS 1/' lib/curl_config.h
elif [[ "$os_name" == *"Linux"* ]]; then
	autoreconf -fi
	./configure --host=aarch64-buildroot-linux-gnu --build=x86_64-pc-linux-gnu CFLAGS="-Os -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables -flto" LDFLAGS="-Wl,-s -Wl,-Bsymbolic -Wl,--gc-sections" --with-openssl --without-brotli --without-nghttp2 --without-libidn2 --without-libpsl --without-zstd --disable-alt-svc --disable-hsts --disable-tls-srp --disable-proxy --disable-ipv6 --disable-ntlm-wb --disable-ntlm --disable-dict --disable-gopher --disable-imap --disable-mqtt --disable-pop3 --disable-smtp --disable-telnet --disable-tftp --disable-rtsp --disable-file --disable-ldap --disable-ldaps --without-librtmp --disable-manual --with-zstd
#	./configure --host=arm-buildroot-linux-gnueabihf --build=x86_64-pc-linux-gnu CFLAGS="-Os -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables -flto" LDFLAGS="-Wl,-s -Wl,-Bsymbolic -Wl,--gc-sections" --with-openssl --without-brotli --without-nghttp2 --without-libidn2 --without-libpsl --without-zstd --disable-alt-svc --disable-hsts --disable-tls-srp --disable-proxy --disable-ipv6 --disable-ntlm-wb --disable-ntlm --disable-dict --disable-gopher --disable-imap --disable-mqtt --disable-pop3 --disable-smtp --disable-telnet --disable-tftp --disable-rtsp --disable-file --disable-ldap --disable-ldaps --without-librtmp --disable-manual --with-zstd
else
	echo "OS $os_name is unknown"
	exit 1
fi
cd ..

cd zziplib
cmake .
cd ../..    # back to Projects/luaxx/

mkdir -p obj
SYSROOT=`$CC -print-sysroot` make -f static.Makefile
