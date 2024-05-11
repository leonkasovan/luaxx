#!/bin/bash
# Build luaxx native (OS arch) with static library: zlib, zzip and curl

cd external

cd zstd
make -j4 lib-release
cp lib/libzstd.a ../
cd ..

cd zlib
# cmake .
cd ..

cd curl
os_name=$(uname -s)
machine_name=$(uname -m)
node_name=$(uname -n)
if [[ "$os_name" == *"CYGWIN"* ]] || [[ "$os_name" == *"MINGW"* ]] || [[ "$os_name" == *"MSYS"* ]]; then
	autoreconf -fi
	./configure --with-schannel --without-brotli --without-nghttp2 --without-libidn2 --without-libpsl --without-zstd --disable-alt-svc --disable-hsts --disable-tls-srp --disable-proxy --disable-ipv6 --disable-ntlm-wb --disable-ntlm --disable-dict --disable-gopher --disable-imap --disable-mqtt --disable-pop3 --disable-smtp --disable-telnet --disable-tftp --disable-rtsp --disable-file --disable-ldap --disable-ldaps --disable-pthreads
	sed -i 's/^#define USE_UNIX_SOCKETS 1/\/\/#define USE_UNIX_SOCKETS 1/' lib/curl_config.h
elif [[ "$os_name" == *"Linux"* ]]; then
	# autoreconf -fi
	# ./configure CFLAGS="-Os -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables -flto" LDFLAGS="-Wl,-s -Wl,-Bsymbolic -Wl,--gc-sections" --with-openssl --without-brotli --without-nghttp2 --without-libidn2 --without-libpsl --without-zstd --disable-alt-svc --disable-hsts --disable-tls-srp --disable-proxy --disable-ipv6 --disable-ntlm-wb --disable-ntlm --disable-dict --disable-gopher --disable-gophers --disable-imap --disable-mqtt --disable-pop3 --disable-pop3s --disable-smtp --disable-smtps --disable-telnet --disable-tftp --disable-rtsp --disable-file --disable-ldap --disable-ldaps
	cp ../$node_name.$os_name.$machine_name.curl_config.h lib/curl_config.h
else
	echo "OS $os_name is unknown"
	exit 1
fi
cd ..

cd zziplib
# cmake .
cd ../..    # back to /f/Projects/luaxx/

mkdir -p obj
make -j4 -f static.Makefile
