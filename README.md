# luaxx
Lua with eXtra library

```
sudo nano /etc/apt/sources.list
deb [arch=amd64] http://archive.ubuntu.com/ubuntu/ jammy main restricted universe multiverse
deb [arch=amd64] http://archive.ubuntu.com/ubuntu/ jammy-updates main restricted universe multiverse
deb [arch=amd64] http://archive.ubuntu.com/ubuntu/ jammy-backports main restricted universe multiverse
deb [arch=amd64] http://security.ubuntu.com/ubuntu/ jammy-security main restricted universe multiverse
deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy main restricted universe multiverse
deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-updates main restricted universe multiverse
deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-backports main restricted universe multiverse
deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-security main restricted universe multiverse
sudo dpkg --add-architecture arm64
sudo apt update
sudo apt install libssl-dev:arm64
sudo apt install gcc-aarch64-linux-gnu autoconf cmake libtool
git clone --recurse-submodules https://github.com/leonkasovan/luaxx.git
./build_native.sh
./build_cross_compile.sh
```
