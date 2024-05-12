#!/bin/bash
# Build luaxx native (OS arch) with static library: zlib, zzip and curl

id_name=""
# Check if the file /etc/os-release exists
if [ -f /etc/os-release ]; then
    # Extract the ID information using grep
    id_name=$(grep "^ID=" /etc/os-release | cut -d '=' -f 2-)
    
    # Print the extracted ID
    echo "ID: $id_name"
else
    echo "/etc/os-release not found."
	exit 1
fi
machine_name=$(uname -m)
os_name=$(uname -s)

cd external

cd zstd
make -j4 lib-release
cp lib/libzstd.a ../
cd ..

cd zlib
# cmake .
cd ..

cd curl
# autoreconf -fi
# ./configure CFLAGS="-Os -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables -flto" LDFLAGS="-Wl,-s -Wl,-Bsymbolic -Wl,--gc-sections" --with-openssl --without-brotli --without-nghttp2 --without-libidn2 --without-libpsl --disable-alt-svc --disable-hsts --disable-tls-srp --disable-proxy --disable-ipv6 --disable-ntlm-wb --disable-ntlm --disable-dict --disable-gopher --disable-imap --disable-mqtt --disable-pop3 --disable-smtp --disable-telnet --disable-tftp --disable-rtsp --disable-file --disable-ldap --disable-ldaps
if [ -f ../$id_name.$machine_name.curl_config.h ]; then
	echo "CURL config for $id_name.$machine_name found."
	cp ../$id_name.$machine_name.curl_config.h lib/curl_config.h
else
	echo "CURL config for $id_name.$machine_name not found. Generate it with autoreconf and ./configure"
	exit 1
fi
cd ..

cd zziplib
# cmake .
if [ -f ../$id_name.$machine_name.zzip_config.h ]; then
	echo "ZZIP config for $id_name.$machine_name found."
	cp ../$id_name.$machine_name.zzip_config.h zzip/_config.h
else
	echo "ZZIP config for $id_name.$machine_name not found. Generate it with cmake"
	exit 1
fi
cd ../..    # back to /f/Projects/luaxx/

mkdir -p obj
make -j4 -f static.Makefile
