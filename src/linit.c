/*
** $Id: linit.c,v 1.14.1.1 2007/12/27 13:02:25 roberto Exp $
** Initialization of libraries for lua.c
** See Copyright Notice in lua.h
*/


#define linit_c
#define LUA_LIB

#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"


static const luaL_Reg lualibs[] = {
  {"", luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_IOLIBNAME, luaopen_io},
  {LUA_OSLIBNAME, luaopen_os},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_DBLIBNAME, luaopen_debug},
  {LUA_HTTPLIBNAME, luaopen_http},
  {LUA_JSONLIBNAME, luaopen_cjson},
  {LUA_JSONSAFELIBNAME, luaopen_cjson_safe},
  // {REX_LIBNAME, luaopen_rex_pcre},
  {LUA_CSVLIBNAME, luaopen_csv},
#ifdef WIN32  
  // {LUA_IUPLIBNAME, luaopen_iuplua},
#endif  
  {LUA_GZIOLIBNAME, luaopen_gzio},
  {LUA_LFSLIBNAME, luaopen_lfs},
  {LUA_ZIPLIBNAME, luaopen_zip},
  {NULL, NULL}
};


LUALIB_API void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
}

