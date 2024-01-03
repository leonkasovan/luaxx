/*
HTTP Lua Library
** Curl wrapper library https://github.com/LuaDist/luacurl
** We've registered this lib and to be executed in linit.c
TODO:

TEST
	rc = http.get("http://127.0.0.1/1.zip","1.zip")
	rc, body = http.get("http://127.0.0.1/")
	1. normal (ok)
	2. partial (ok)
	
	3. cookie (ok)
	print(http.info(http.INFO_EFFECTIVE_URL))
	for k,v in pairs(http.info(http.INFO_COOKIELIST)) do print(v) end
	4. post
	5. upload

Dont forget to define CURL_STATICLIB while building liblua
*/


#define lhttplib_c
#define LUA_LIB

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "bstring.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include "curl/curl.h"
#include <stdbool.h>
#include <ctype.h>
#include <linux/limits.h>
#define _stat64 stat
#define MAX_PATH PATH_MAX
#define _snprintf_c snprintf
#define _stricmp strcasecmp
#define _strdup strdup
#define BOOL bool
#define TRUE true
#define FALSE false
#else
#include "curl/curl.h"
#endif

/* DEFINE is here ============================================================ */

/* Fast set table macro */
#define LUA_SET_TABLE(context, key_type, key, value_type, value) \
	lua_push##key_type(context, key); \
	lua_push##value_type(context, value); \
	lua_settable(context, -3);

#ifdef WIN32
#define C_OPT(n, t)
#define ALL_CURL_OPT \
	C_OPT(URL, string) \
	C_OPT(PORT, number) \
	C_OPT(PROXY, string) \
	C_OPT(USERPWD, string) \
	C_OPT(PROXYUSERPWD, string) \
	C_OPT(RANGE, string) \
	C_OPT(TIMEOUT, number) \
	C_OPT(POSTFIELDS, string) \
	C_OPT(REFERER, string) \
	C_OPT(USERAGENT, string) \
	C_OPT(RESUME_FROM, number) \
	C_OPT(COOKIE, string) \
	C_OPT(HEADER, boolean) \
	C_OPT(NOPROGRESS, boolean) \
	C_OPT(UPLOAD, boolean) \
	C_OPT(POST, boolean) \
	C_OPT(AUTOREFERER, boolean) \
	C_OPT(PROXYPORT, number) \
	C_OPT(POSTFIELDSIZE, number) \
	C_OPT(HTTPGET, boolean) \
	C_OPT(COOKIEJAR, string) \
	C_OPT(ENCODING, string) \
	C_OPT(PROXYTYPE, number) \
	C_OPT(HTTPAUTH, number) \
	C_OPT(PROXYAUTH, number) \
	C_OPT(CUSTOMREQUEST, string) \
	C_OPT(FRESH_CONNECT, number) \
	C_OPT(COOKIEFILE, string) \
	C_OPT(VERBOSE, boolean) \
	C_OPT(LOW_SPEED_LIMIT, number) \
	C_OPT(LOW_SPEED_TIME, number) \
	C_OPT(COOKIELIST, string) \
	C_OPT(POSTREDIR, number) \

#else
#define C_OPT(n, t)
#define ALL_CURL_OPT \
	C_OPT(URL, string) \
	C_OPT(PORT, number) \
	C_OPT(PROXY, string) \
	C_OPT(USERPWD, string) \
	C_OPT(PROXYUSERPWD, string) \
	C_OPT(RANGE, string) \
	C_OPT(TIMEOUT, number) \
	C_OPT(POSTFIELDS, string) \
	C_OPT(REFERER, string) \
	C_OPT(USERAGENT, string) \
	C_OPT(RESUME_FROM, number) \
	C_OPT(COOKIE, string) \
	C_OPT(HEADER, boolean) \
	C_OPT(NOPROGRESS, boolean) \
	C_OPT(UPLOAD, boolean) \
	C_OPT(POST, boolean) \
	C_OPT(AUTOREFERER, boolean) \
	C_OPT(PROXYPORT, number) \
	C_OPT(POSTFIELDSIZE, number) \
	C_OPT(HTTPGET, boolean) \
	C_OPT(COOKIEJAR, string) \
	C_OPT(ENCODING, string) \
	C_OPT(PROXYTYPE, number) \
	C_OPT(HTTPAUTH, number) \
	C_OPT(PROXYAUTH, number) \
	C_OPT(CUSTOMREQUEST, string) \
	C_OPT(FRESH_CONNECT, number) \
	C_OPT(COOKIEFILE, string) \
	C_OPT(VERBOSE, boolean) \
	C_OPT(LOW_SPEED_LIMIT, number) \
	C_OPT(LOW_SPEED_TIME, number) \
	C_OPT(COOKIELIST, string) \

#endif

struct feat {
	const char *name;
	int bitmask;
};

static const struct feat feats[] = {
  {"AsynchDNS",      CURL_VERSION_ASYNCHDNS},
  {"Debug",          CURL_VERSION_DEBUG},
  {"TrackMemory",    CURL_VERSION_CURLDEBUG},
  {"IDN",            CURL_VERSION_IDN},
  {"IPv6",           CURL_VERSION_IPV6},
  {"Largefile",      CURL_VERSION_LARGEFILE},
  {"SSPI",           CURL_VERSION_SSPI},
  {"GSS-API",        CURL_VERSION_GSSAPI},
  {"Kerberos",       CURL_VERSION_KERBEROS5},
  {"SPNEGO",         CURL_VERSION_SPNEGO},
  {"NTLM",           CURL_VERSION_NTLM},
  {"NTLM_WB",        CURL_VERSION_NTLM_WB},
  {"SSL",            CURL_VERSION_SSL},
  {"libz",           CURL_VERSION_LIBZ},
  {"brotli",         CURL_VERSION_BROTLI},
  {"CharConv",       CURL_VERSION_CONV},
  {"TLS-SRP",        CURL_VERSION_TLSAUTH_SRP},
  {"HTTP2",          CURL_VERSION_HTTP2},
  {"UnixSockets",    CURL_VERSION_UNIX_SOCKETS},
  {"HTTPS-proxy",    CURL_VERSION_HTTPS_PROXY},
  {"MultiSSL",       CURL_VERSION_MULTI_SSL},
  {"PSL",            CURL_VERSION_PSL},
#ifdef WIN32
  {"alt-svc",        CURL_VERSION_ALTSVC},
  {"Unicode",        CURL_VERSION_UNICODE},
  {"HTTP3",          CURL_VERSION_HTTP3},
  {"zstd",           CURL_VERSION_ZSTD},
#endif
};

#define FIRST_ARG 1
#define SECOND_ARG 2
#define THIRD_ARG 3
#define FOURTH_ARG 4

/* TYPE is here ============================================================ */
union luaValueT
{
	struct curl_slist *slist;
	int nval;
	char* sval;
	void* ptr;
};

struct MemoryStruct {
  char *memory;
  size_t size;
};

/* Global Variable ========================================================= */
static CURL *curl;

/* Private Function ======================================================== */
void *myrealloc(void *ptr, size_t size){
  /* There might be a realloc() out there that doesn't like reallocing
     NULL pointers, so we take care of it here */
  if(ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data){
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)data;

  mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory) {
    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
  }
  return realsize;
}

/* convert argument n to string allowing nil values */
static union luaValueT get_string(lua_State* L, int n)
{
	union luaValueT v;
	//printf("lhttplib.c: argumen=%d\n", n);
	v.sval=(char*)lua_tostring(L, n);
	//printf("lhttplib.c: value_as_string=%s\n", v.sval);
	return v;
}

/* convert argument n to number allowing only number convertable values */
static union luaValueT get_number(lua_State* L, int n)
{
	union luaValueT v;
	//printf("lhttplib.c: argumen=%d\n", n);
	v.nval=(int)luaL_checknumber(L, n);
	//printf("lhttplib.c: value_as_number=%d\n", v.nval);
	return v;
}

/* get argument n as boolean but if not boolean argument then fail with Lua error */
static union luaValueT get_boolean(lua_State* L, int n)
{
	union luaValueT v;
	if (!lua_isboolean(L, n)){
		luaL_argerror(L, n, "boolean value expected");
	}
	v.nval=(int)lua_toboolean(L, n);
	return v;
}

static int hl_version (lua_State *L) {
	/* Pointer to libcurl's run-time version information */
	curl_version_info_data *curlinfo = curl_version_info(CURLVERSION_NOW);
	const char *const *proto;
	luaL_Buffer b;
	luaL_buffinit(L, &b);

	luaL_addstring(&b, "Lua HTTP using ");
	luaL_addstring(&b, curl_version());
	luaL_addstring(&b, "\nProtocols: ");
	if(curlinfo){
		if(curlinfo->protocols) {
			for(proto = curlinfo->protocols; *proto; ++proto) {
				luaL_addstring(&b, *proto);
				luaL_addchar(&b, ' ');
			}
		}
	}

	if(curlinfo->features) {
		char *featp[ sizeof(feats) / sizeof(feats[0]) + 1];
		size_t numfeat = 0;
		unsigned int i;
		luaL_addstring(&b, "\nFeatures:");
		for(i = 0; i < sizeof(feats)/sizeof(feats[0]); i++) {
			if(curlinfo->features & feats[i].bitmask)
				featp[numfeat++] = (char *)feats[i].name;
		}
		for(i = 0; i< numfeat; i++){
			luaL_addchar(&b, ' ');
			luaL_addlstring(&b, featp[i], strlen(featp[i]));
		}
	}

	luaL_pushresult(&b);
	return 1;
}


static int hl_set_conf (lua_State *L) {
	union luaValueT v;                   /* the result option value */
	int curlOpt;                         /* the provided option code  */
	CURLcode code;                       /* return error code from curl */

	luaL_checktype(L, 1, LUA_TNUMBER);   /* accept only number option codes */
	if (lua_gettop(L)<2)                 /* option value is always required */
	{
		luaL_error(L, "Invalid number of arguments %d to `set' method", lua_gettop(L));
	}
	curlOpt=(int)lua_tonumber(L, 1);     /* get the curl option code */
	v.nval=0;

/* Handle all supported curl options differently 
	according the specific option argument type */
#undef C_OPT
#define C_OPT(n, t)	\
	case CURLOPT_##n: v=get_##t(L, 2); break;

	switch (curlOpt){
/* Expands all the list of switch-case's here */
ALL_CURL_OPT
	}

	//if (CURLE_OK == (code=curl_easy_setopt(curl, curlOpt, v.nval))){
	if (CURLE_OK == (code=curl_easy_setopt(curl, curlOpt, v))){
		/* on success return true */
		lua_pushboolean(L, 1);
		lua_pushstring(L, "Success");
	}else{
		lua_pushnil(L);
		lua_pushstring(L, curl_easy_strerror(code));
	}
	return 2;
}

const char *find_url (const char *tag_element, const char *tag_attr, const char *html,bstring value){
	const char *ii;
	const char *start;
	int found_element,found_attr;
	bstring element,attr;

	found_element = 0;
	found_attr = 0;
	start = html;
	vstr_reset(value);	//rlink must be allocated outside and we just reset it here
	element = vstr_new_n(32,"");
	attr = vstr_new_n(32,"");
	while((ii = strchr(start,'<'))!=NULL && !found_element){

		//grab html element after <
		ii++;
		vstr_reset(element);
		while(*ii && isalpha(*ii)) vstr_add_chr(element,*ii++);

		//is the HTML element what we need? if yes, we will find specific attribute
		if (_stricmp(tag_element,(const char *)element->data)==0){
			found_element = 1;

			//Find specific attribute
			found_attr = 0;
			vstr_reset(value);
			while(*ii && *ii!='>' && !found_attr){

				//skip whitespace
				while(*ii && isspace(*ii)) ii++;

				//grab attr
				vstr_reset(attr);
				while(*ii && isalpha(*ii)) vstr_add_chr(attr,*ii++);

				//assumption: there is no space after attribute tag
				ii++;	//skip =

				//check, is it attribute that we are looking for?
				if(_stricmp((const char *)attr->data,tag_attr)==0){

					if (*ii=='\''){	//use single quote
						ii++;	//skip '
						while(*ii && *ii!='\'') vstr_add_chr(value,*ii++);
						if (*ii) ii++;	//skip closing '
					}else if(*ii=='"'){ //use double quote
						ii++;	//skip "
						while(*ii && *ii!='"') vstr_add_chr(value,*ii++);
						if (*ii) ii++;	//skip closing "
					}else{	//no quote
						while(*ii && !isspace(*ii) && *ii!='>') vstr_add_chr(value,*ii++);
					}
					found_attr = 1;
				}else{	//just eat the rest
					if (*ii=='\''){	//use single quote
						ii++;	//skip '
						while(*ii && *ii!='\'') ii++;
						if (*ii) ii++;	//skip closing '
					}else if(*ii=='"'){ //use double quote
						ii++;	//skip "
						while(*ii && *ii!='"') ii++;
						if (*ii) ii++;	//skip closing "
					}else{	//no quote
						while(*ii && !isspace(*ii)  && *ii!='>') ii++;
					}
				}
			}
		}

		start = ii;	//we are moving to the next
	}

	vstr_free(element);
	vstr_free(attr);
	return ii;
}


/*
* Concatenate a relative URL to a base URL making it absolute.
* URL-encodes any spaces.
* The returned pointer must be freed by the caller unless NULL
* (returns NULL on out of memory).
*/
static char *concat_url(const char *base, const char *relurl)
{
	/***
	TRY to append this new path to the old URL
	to the right of the host part. Oh crap, this is doomed to cause
	problems in the future...
	*/
	char *newest;
	char *protsep;
	char *pathsep;
	size_t newlen;
	BOOL host_changed = FALSE;
	char *url_clone;
	const char *useurl = relurl;
	size_t urllen;
	const char *sep;
	const char *query;

	if (strstr(relurl, "http://") || strstr(relurl, "https://") || strstr(relurl, "www.") || strstr(relurl, "ftp://") || strstr(relurl, "ftps://")) {
		return _strdup(relurl);
	}
	/* we must make our own copy of the URL to play with, as it may
	point to read-only data */
	url_clone = _strdup(base);

	if(!url_clone)
		return NULL; /* skip out of this NOW */

	/* protsep points to the start of the host name */
	protsep = strstr(url_clone, "//");
	if(!protsep)
		protsep = url_clone;
	else
		protsep += 2; /* pass the slashes */

	if('/' != relurl[0]) {
		int level = 0;

		/* First we need to find out if there's a ?-letter in the URL,
		and cut it and the right-side of that off */
		pathsep = strchr(protsep, '?');
		if(pathsep)
			*pathsep = 0;

		/* we have a relative path to append to the last slash if there's one
		available, or if the new URL is just a query string (starts with a
		'?')  we append the new one at the end of the entire currently worked
		out URL */
		if(useurl[0] != '?') {
			pathsep = strrchr(protsep, '/');
			if(pathsep)
				*pathsep = 0;
		}

		/* Check if there's any slash after the host name, and if so, remember
		that position instead */
		pathsep = strchr(protsep, '/');
		if(pathsep)
			protsep = pathsep + 1;
		else
			protsep = NULL;

		/* now deal with one "./" or any amount of "../" in the newurl
		and act accordingly */

		if((useurl[0] == '.') && (useurl[1] == '/'))
			useurl += 2; /* just skip the "./" */

		while((useurl[0] == '.') &&
			(useurl[1] == '.') &&
			(useurl[2] == '/')) {
				level++;
				useurl += 3; /* pass the "../" */
		}

		if(protsep) {
			while(level--) {
				/* cut off one more level from the right of the original URL */
				pathsep = strrchr(protsep, '/');
				if(pathsep)
					*pathsep = 0;
				else {
					*protsep = 0;
					break;
				}
			}
		}
	}
	else {
		/* We got a new absolute path for this server */

		if(relurl[1] == '/') {
			/* the new URL starts with //, just keep the protocol part from the
			original one */
			*protsep = 0;
			useurl = &relurl[2]; /* we keep the slashes from the original, so we
								 skip the new ones */
			host_changed = TRUE;
		}
		else {
			/* cut off the original URL from the first slash, or deal with URLs
			without slash */
			pathsep = strchr(protsep, '/');
			if(pathsep) {
				/* When people use badly formatted URLs, such as
				"http://www.url.com?dir=/home/daniel" we must not use the first
				slash, if there's a ?-letter before it! */
				char *sep = strchr(protsep, '?');
				if(sep && (sep < pathsep))
					pathsep = sep;
				*pathsep = 0;
			}
			else {
				/* There was no slash. Now, since we might be operating on a badly
				formatted URL, such as "http://www.url.com?id=2380" which doesn't
				use a slash separator as it is supposed to, we need to check for a
				?-letter as well! */
				pathsep = strchr(protsep, '?');
				if(pathsep)
					*pathsep = 0;
			}
		}
	}

	/* If the new part contains a space, this is a mighty stupid redirect
	but we still make an effort to do "right". To the left of a '?'
	letter we replace each space with %20 while it is replaced with '+'
	on the right side of the '?' letter.
	*/
	{
	const char *ptr;	
	BOOL left = TRUE; /* left side of the ? */
	const char *host_sep = (const char *) useurl;
	
	newlen = 0;
	if(host_changed){
		/* Find the start of the hostname */
		sep = strstr(useurl, "//");
		if(!sep) sep = useurl;	else sep += 2;
		query = strchr(sep, '?');
		sep = strchr(sep, '/');
		if(!sep) sep = useurl + strlen(useurl);
		if(!query) query = useurl + strlen(useurl);
		host_sep =  sep < query ? sep : query;
	}


	for(ptr = useurl; *ptr; ptr++) {

		if(ptr < host_sep) {
			++newlen;
			continue;
		}

		switch(*ptr) {
		case '?':
			left = FALSE;
			/* FALLTHROUGH */
		default:
			if(!(iscntrl(*ptr) || isspace(*ptr) || isgraph(*ptr)))
				newlen += 2;
			newlen++;
			break;
		case ' ':
			if(left)
				newlen += 3;
			else
				newlen++;
			break;
		}
	}
	}

	urllen = strlen(url_clone);

	newest = (char *)malloc(urllen + 1 + /* possible slash */
		newlen + 1 /* zero byte */);

	if(!newest) {
		free(url_clone); /* don't leak this */
		return NULL;
	}

	/* copy over the root url part */
	memcpy(newest, url_clone, urllen);

	/* check if we need to append a slash */
	if(('/' == useurl[0]) || (protsep && !*protsep) || ('?' == useurl[0]))
		;
	else
		newest[urllen++]='/';

	/* then append the new piece on the right side */
	{
	BOOL left = TRUE;
	const char *iptr;
	char *optr = &newest[urllen];
	const char *host_sep = useurl;

	if(host_changed){
		/* Find the start of the hostname */
		sep = strstr(useurl, "//");
		if(!sep) sep = useurl;	else sep += 2;
		query = strchr(sep, '?');
		sep = strchr(sep, '/');
		if(!sep) sep = useurl + strlen(useurl);
		if(!query) query = useurl + strlen(useurl);
		host_sep =  sep < query ? sep : query;
	}

	for(iptr = useurl;    /* read from here */
		*iptr;         /* until zero byte */
		iptr++) {

			if(iptr < host_sep) {
				*optr++ = *iptr;
				continue;
			}

			switch(*iptr) {
			case '?':
				left = FALSE;
				/* FALLTHROUGH */
			default:
				if(!(iscntrl(*iptr) || isspace(*iptr) || isgraph(*iptr))) {
					_snprintf_c(optr, 4, "%%%02x", *iptr);
					optr += 3;
				}
				else
					*optr++=*iptr;
				break;
			case ' ':
				if(left) {
					*optr++='%'; /* add a '%' */
					*optr++='2'; /* add a '2' */
					*optr++='0'; /* add a '0' */
				}
				else
					*optr++='+'; /* add a '+' here */
				break;
			}
	}
	*optr = 0; /* null-terminate output buffer */
	}

	free(url_clone);

	return newest;
}

/* Exported Function =========================================================== */
static int hl_error (lua_State *L) {
	int rc = luaL_checkinteger(L, 1);

	lua_pushstring(L, curl_easy_strerror(rc));
	return 1;
}

static int hl_info (lua_State *L) {
	int nInfo = luaL_checkinteger(L, 1);
	CURLcode code = -1;
	CURL *c;
	c = curl;

	if (nInfo>CURLINFO_SLIST)
	{
		/* string list */
		struct curl_slist *slist=0;
		if (CURLE_OK == (code=curl_easy_getinfo(c, nInfo, &slist)))
		{
			if (slist)
			{
				int i;
				lua_newtable(L);
				for (i=1; slist; i++, slist=slist->next)
				{
					lua_pushnumber(L, i);
					lua_pushstring(L, slist->data);
					lua_settable(L, -3);
				}
				curl_slist_free_all(slist);
			} else
			{
				lua_pushnil(L);
			}
			return 1;
		} else
		{
			/* curl_easy_getinfo returns error */
		}
	} else
	if (nInfo>CURLINFO_DOUBLE)
	{
		/* double */
		double value;
		if (CURLE_OK == (code=curl_easy_getinfo(c, nInfo, &value)))
		{
			lua_pushnumber(L, value);
			return 1;
		} else
		{
			/* curl_easy_getinfo returns error */
		}
	} else
	if (nInfo>CURLINFO_LONG)
	{
		/* long */
		long value;
		if (CURLE_OK == (code=curl_easy_getinfo(c, nInfo, &value)))
		{
			lua_pushinteger(L, (lua_Integer)value);
			return 1;
		} else
		{
			/* curl_easy_getinfo returns error */
		}
	} else
	if (nInfo>CURLINFO_STRING)
	{
		/* string */
		char* value;
		if (CURLE_OK == (code=curl_easy_getinfo(c, nInfo, &value)))
		{
			lua_pushstring(L, value);
			return 1;
		} else
		{
			/* curl_easy_getinfo returns error */
		}
	}
/* on error */
	/* return nil, error message, error code */
	lua_pushnil(L);
	if (code>CURLE_OK)
	{
		lua_pushstring(L, curl_easy_strerror(code));
		return 2;
	}
	else
	{
		lua_pushfstring(L, "Invalid CURLINFO number: %d", nInfo);
		return 2;
	}
}

/*static int hl_upload_file (lua_State *L) {
	CURLcode rc;
	const char *url = NULL;
	const char *fname = NULL;
	FILE *f = NULL;
	struct MemoryStruct chunk;

	chunk.memory=NULL; // we expect realloc(NULL, size) to work
	chunk.size = 0;    // no data at this point 

	url = (char*)luaL_checkstring(L, 1);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);

	fname = (char*)luaL_checkstring(L, 2);
	if (fname[0] == 0){	// if string is empty like this ""
		lua_pushinteger(L, CURLE_FILE_COULDNT_READ_FILE);
		return 1;
	}

	f = fopen(fname,"rb");
	if (f == NULL){
		lua_pushinteger(L, CURLE_FILE_COULDNT_READ_FILE);
		return 1;
	}
	curl_easy_setopt(curl, CURLOPT_READDATA, f);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, fread);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	rc = curl_easy_perform(curl);
	lua_pushinteger(L, rc);
	lua_pushlstring(L, chunk.memory, chunk.size);
	if(chunk.memory) free(chunk.memory);
	fclose(f);
	return 2;
}*/

static int hl_collect_resource(lua_State *L) {
	size_t l;
	const char *element = luaL_checklstring(L, 1, &l);
	const char *attr = luaL_checklstring(L, 2, &l);
	const char *html = luaL_checklstring(L, 3, &l);
	const char *baseurl = luaL_checklstring(L, 4, &l);
	const char *next;
	bstring link;
	int i;
	char *abs_url;

	link = vstr_new_n(1024,"");
	lua_newtable(L);
	next = html;
	i = 1;

	do{
		next = find_url(element,attr,next,link);
		if (link->slen){
			abs_url = concat_url(baseurl, (const char *)link->data);
			if (abs_url){
				lua_pushstring(L, abs_url);
				lua_rawseti(L, -2, i++);
				free(abs_url);
			}
		}
	}while(next);

	vstr_free(link);
	return 1;
}

static int hl_collect_link(lua_State *L) {
	size_t l;
	const char *html = luaL_checklstring(L, 1, &l);
	const char *baseurl = luaL_checklstring(L, 2, &l);
	const char *next;
	bstring link;
	int i;
	char *abs_url;

	link = vstr_new_n(1024,"");
	lua_newtable(L);
	next = html;
	i = 1;

	do{
		next = find_url("a","href",next,link);
		if (link->slen){
			abs_url = concat_url(baseurl, (const char *)link->data);
			if (abs_url){
				char *p;
				//replace char # with NULL, because it's just bookmark link
				p = (char *)link->data;
				while(*p && *p!='#'){
					p++;
				}
				if (*p=='#') *p = 0;

				//push it stack
				lua_pushstring(L, abs_url);
				lua_rawseti(L, -2, i++);
				free(abs_url);
			}
		}
	}while(next);

	vstr_free(link);
	return 1;
}

static int hl_collect_image(lua_State *L) {
	size_t l;
	const char *html = luaL_checklstring(L, 1, &l);
	const char *baseurl = luaL_checklstring(L, 2, &l);
	const char *next;
	bstring link;
	int i;
	char *abs_url;

	link = vstr_new_n(1024,"");
	lua_newtable(L);
	next = html;
	i = 1;

	do{
		next = find_url("img","src",next,link);
		if (link->slen){
			abs_url = concat_url(baseurl, (const char *)link->data);
			if (abs_url){
				lua_pushstring(L, abs_url);
				lua_rawseti(L, -2, i++);
				free(abs_url);
			}
		}
	}while(next);

	vstr_free(link);
	return 1;
}


static int hl_resource_name(lua_State* L){
	const char *url = luaL_checkstring(L, 1);
	char *ptr,*c,*resname;

	ptr = (char *)strrchr(url, '/');
	if(ptr == NULL){
		/* Root level request, apparently */
		lua_pushnil(L);
		return 1;
	}

	if (*(ptr-1)=='/'){
		/* url is http://xxxxxxxx */
		lua_pushnil(L);
		return 1;
	}

	ptr++;
	if(*ptr == '\0') {
		lua_pushnil(L);
		return 1;
	}

	resname = (char *)malloc(strlen(ptr)+1);
	strcpy(resname, ptr);

	/* cut if any trailing ?*/
	c = resname;
	while(*c){
		if (*c == '?')
			*c = '\0';
		c++;
	}
	lua_pushstring(L, resname);
	free(resname);
	return 1;
}

/* garbage collect the curl object */
/* do we have to do this ???? It wont called by lua in the end */
static int hl_gc(lua_State* L){
	//printf("Cleaning up...");
	if (curl){
		curl_easy_cleanup(curl);
	}
	return 0;
}

static int hl_unescape (lua_State *L) {
	const char *input;
	char *output;
	size_t input_len;

	input = luaL_checklstring(L, 1, &input_len);
	output = curl_unescape(input, input_len);
	if (output){
		lua_pushstring(L, output);
		curl_free(output);
		return 1;
	}else{
		lua_pushnil(L);
		return 1;
	}
}

static int hl_escape (lua_State *L) {
	const char *input;
	char *output;
	size_t input_len;

	
	input = luaL_checklstring(L, 1, &input_len);
	output = curl_escape(input, input_len);
	if (output){
		lua_pushstring(L, output);
		curl_free(output);
		return 1;
	}else{
		lua_pushnil(L);
		return 1;
	}
	
}

static int hl_request (lua_State *L) {
	//luaL_Buffer b;
	FILE *fb = NULL;
	const char *url = NULL;
	const char *formdata = NULL;
	const char *method = NULL;
	struct curl_slist *myheaders = NULL;
	struct MemoryStruct header_chunk;
	struct MemoryStruct body_chunk;
	CURLcode rc;

	body_chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
	body_chunk.size = 0;    /* no data at this point */
	header_chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
	header_chunk.size = 0;    /* no data at this point */
	
	if (lua_type(L, FIRST_ARG) == LUA_TSTRING) {
		url = luaL_checkstring(L, FIRST_ARG); // Get 1st Argument : URL
		formdata = luaL_optstring(L, SECOND_ARG, ""); // Get 2nd Argument : Form Data

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)0);
		curl_easy_setopt(curl, CURLOPT_RANGE, NULL);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
		if (formdata[0] == '\0') {
			//printf("[lhttplib.c|hl_request|1402] GET url=%s\n", url);
			curl_easy_setopt(curl, CURLOPT_HTTPGET, (long)1);
		}else{
			//printf("[lhttplib.c|hl_request|1405] POST url=%s\n", url);
			curl_easy_setopt(curl, CURLOPT_POST, (long)1);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, formdata);
		}
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&body_chunk);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	}else{	// FIRST_ARG is LUA_TTABLE
		luaL_checktype(L, FIRST_ARG, LUA_TTABLE);
		lua_settop(L, 1);  /* make sure table is at the top */

		lua_getfield(L, -1, "url");		// Get Parameter for url
		if (lua_isstring(L, -1)){
			url = lua_tostring(L, -1);
		}else{
			url = "";
		}
		curl_easy_setopt(curl, CURLOPT_URL, url);
		
		lua_pop(L, 1);

		lua_getfield(L, -1, "formdata");	// Get Parameter for formdata
		if (lua_isstring(L, -1)){
			//printf("[lhttplib.c|hl_request|1427] POST url=%s\n", url);
			formdata = lua_tostring(L, -1);
			curl_easy_setopt(curl, CURLOPT_POST, (long)1);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, formdata);
		}else{
			//printf("[lhttplib.c|hl_request|1432] GET url=%s\n", url);
			curl_easy_setopt(curl, CURLOPT_HTTPGET, (long)1);
		}
		lua_pop(L, 1);

		lua_getfield(L, -1, "method");	// Get Parameter for method
		if (lua_isstring(L, -1)){
			method = lua_tostring(L, -1);
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
		}
		lua_pop(L, 1);

		lua_getfield(L, -1, "output_filename");	// Get Parameter for output_filename
		if (lua_isstring(L, -1)){
			const char *output_filename = lua_tostring(L, -1);

			fb = fopen(output_filename,"rb");
			if (fb == NULL){	//from scratch
				//printf("[lhttplib.c|hl_request] Create new file %s\n", output_filename);
				fb = fopen(output_filename, "wb");
				curl_easy_setopt(curl, CURLOPT_RANGE, NULL);
				curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)0);
			}else{		//resume download
				struct _stat64 buf;
				fclose(fb);
				if (_stat64(output_filename, &buf) != 0){
					curl_easy_setopt(curl, CURLOPT_RANGE, NULL);
					curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)0);	//can't get file size, start from 0 beginning
					//printf("[lhttplib.c|hl_request] Resuming %s from 0\n", output_filename);
				}else{
					curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)buf.st_size); //this will override CURLOPT_RANGE if any
					//printf("[lhttplib.c|hl_request] Resuming %s from %ld\n", output_filename, buf.st_size);
				}
				fb = fopen(output_filename, "ab");
			}
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fb);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);		
		}else{	//output to memory
			curl_easy_setopt(curl, CURLOPT_RANGE, NULL);
			curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, (long)0);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&body_chunk);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		}
		lua_pop(L, 1);

		lua_getfield(L, -1, "headers");	// Get Parameter for custom header
		if (lua_type(L, -1) == LUA_TTABLE){
			int headers_idx = lua_gettop(L);
			lua_pushnil(L);  /* first key */			
			while (lua_next(L, headers_idx) != 0) {
				/* not use 'key' (at index -2) and use 'value' (at index -1) append it slist*/				
				myheaders = curl_slist_append(myheaders, lua_tostring(L, -1));
				/* removes 'value'; keeps 'key' for next iteration */
				lua_pop(L, 1);
			}
			if (myheaders) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, myheaders);
		}else{
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
		}
		lua_pop(L, 1);
	}
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void*)&header_chunk);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteMemoryCallback);
	rc = curl_easy_perform(curl);

	lua_pushinteger(L, rc);
	lua_pushlstring(L, header_chunk.memory, header_chunk.size);
	lua_pushlstring(L, body_chunk.memory, body_chunk.size);
	if (header_chunk.memory) free(header_chunk.memory);
	if (body_chunk.memory) free(body_chunk.memory);
	if (fb) fclose(fb);
	if (method)	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);
	if (myheaders) curl_slist_free_all(myheaders);
	return 3;
}

/* LUA Registration ============================================================== */
static const luaL_Reg httplib[] = {
	{"request",hl_request},
	{"escape", hl_escape},
	{"unescape", hl_unescape},
	{"version", hl_version},
	{"set_conf",hl_set_conf},
	{"error",hl_error},
	{"info",hl_info},
	// {"post_file",hl_post_file},
	//{"upload_file",hl_upload_file},
	{"collect_resource",hl_collect_resource},
	{"collect_link",hl_collect_link},
	{"collect_image",hl_collect_image},
	{"resource_name",hl_resource_name},
	{"__gc", hl_gc},
	//{"",hl_},
	{NULL, NULL}
};

LUALIB_API int luaopen_http (lua_State *L) {
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1);
	curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 100000);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Opera/9.80 (J2ME/MIDP; Opera Mini/7.1.32052/29.3417; U; en) Presto/2.8.119 Version/11.10");
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	curl_easy_setopt(curl, CURLOPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);
#ifdef _DEBUG
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
#endif
    luaL_register(L, LUA_HTTPLIBNAME, httplib);

//Set curl options
#undef C_OPT
#define C_OPT(n, t) LUA_SET_TABLE(L, literal, "OPT_"#n, number, CURLOPT_##n);
ALL_CURL_OPT

	/* enum curl_proxytype */
	LUA_SET_TABLE(L, literal, "PROXY_HTTP", number, CURLPROXY_HTTP);
	LUA_SET_TABLE(L, literal, "PROXY_SOCKS4", number, CURLPROXY_SOCKS4);
	LUA_SET_TABLE(L, literal, "PROXY_SOCKS5", number, CURLPROXY_SOCKS5);

	/* auth types */
	LUA_SET_TABLE(L, literal, "AUTH_NONE", number, CURLAUTH_NONE);
	LUA_SET_TABLE(L, literal, "AUTH_BASIC", number, CURLAUTH_BASIC);
	LUA_SET_TABLE(L, literal, "AUTH_DIGEST", number, CURLAUTH_DIGEST);
	LUA_SET_TABLE(L, literal, "AUTH_GSSNEGOTIATE", number, CURLAUTH_GSSNEGOTIATE);
	LUA_SET_TABLE(L, literal, "AUTH_NTLM", number, CURLAUTH_NTLM);
	LUA_SET_TABLE(L, literal, "AUTH_ANY", number, CURLAUTH_ANY);
	LUA_SET_TABLE(L, literal, "AUTH_ANYSAFE", number, CURLAUTH_ANYSAFE);

	/* get info */
	LUA_SET_TABLE(L, literal, "INFO_NONE", number, CURLINFO_NONE);
	LUA_SET_TABLE(L, literal, "INFO_EFFECTIVE_URL", number, CURLINFO_EFFECTIVE_URL);
	LUA_SET_TABLE(L, literal, "INFO_RESPONSE_CODE", number, CURLINFO_RESPONSE_CODE);
	LUA_SET_TABLE(L, literal, "INFO_TOTAL_TIME", number, CURLINFO_TOTAL_TIME);
	LUA_SET_TABLE(L, literal, "INFO_NAMELOOKUP_TIME", number, CURLINFO_NAMELOOKUP_TIME);
	LUA_SET_TABLE(L, literal, "INFO_CONNECT_TIME", number, CURLINFO_CONNECT_TIME);
	LUA_SET_TABLE(L, literal, "INFO_PRETRANSFER_TIME", number, CURLINFO_PRETRANSFER_TIME);
	LUA_SET_TABLE(L, literal, "INFO_SIZE_UPLOAD", number, CURLINFO_SIZE_UPLOAD_T);
	LUA_SET_TABLE(L, literal, "INFO_SIZE_DOWNLOAD", number, CURLINFO_SIZE_DOWNLOAD_T);
	LUA_SET_TABLE(L, literal, "INFO_SPEED_DOWNLOAD", number, CURLINFO_SPEED_DOWNLOAD_T);
	LUA_SET_TABLE(L, literal, "INFO_SPEED_UPLOAD", number, CURLINFO_SPEED_UPLOAD_T);
	LUA_SET_TABLE(L, literal, "INFO_HEADER_SIZE", number, CURLINFO_HEADER_SIZE);
	LUA_SET_TABLE(L, literal, "INFO_REQUEST_SIZE", number, CURLINFO_REQUEST_SIZE);
	LUA_SET_TABLE(L, literal, "INFO_SSL_VERIFYRESULT", number, CURLINFO_SSL_VERIFYRESULT);
	LUA_SET_TABLE(L, literal, "INFO_FILETIME", number, CURLINFO_FILETIME);
	LUA_SET_TABLE(L, literal, "INFO_CONTENT_LENGTH_DOWNLOAD", number, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T);
	LUA_SET_TABLE(L, literal, "INFO_CONTENT_LENGTH_UPLOAD", number, CURLINFO_CONTENT_LENGTH_UPLOAD_T);
	LUA_SET_TABLE(L, literal, "INFO_STARTTRANSFER_TIME", number, CURLINFO_STARTTRANSFER_TIME);
	LUA_SET_TABLE(L, literal, "INFO_CONTENT_TYPE", number, CURLINFO_CONTENT_TYPE);
	LUA_SET_TABLE(L, literal, "INFO_REDIRECT_TIME", number, CURLINFO_REDIRECT_TIME);
	LUA_SET_TABLE(L, literal, "INFO_REDIRECT_COUNT", number, CURLINFO_REDIRECT_COUNT);
	LUA_SET_TABLE(L, literal, "INFO_PRIVATE", number, CURLINFO_PRIVATE);
	LUA_SET_TABLE(L, literal, "INFO_HTTP_CONNECTCODE", number, CURLINFO_HTTP_CONNECTCODE);
	LUA_SET_TABLE(L, literal, "INFO_HTTPAUTH_AVAIL", number, CURLINFO_HTTPAUTH_AVAIL);
	LUA_SET_TABLE(L, literal, "INFO_PROXYAUTH_AVAIL", number, CURLINFO_PROXYAUTH_AVAIL);
	LUA_SET_TABLE(L, literal, "INFO_OS_ERRNO", number, CURLINFO_OS_ERRNO);
	LUA_SET_TABLE(L, literal, "INFO_NUM_CONNECTS", number, CURLINFO_NUM_CONNECTS);
	LUA_SET_TABLE(L, literal, "INFO_SSL_ENGINES", number, CURLINFO_SSL_ENGINES);
	LUA_SET_TABLE(L, literal, "INFO_COOKIELIST", number, CURLINFO_COOKIELIST);
	LUA_SET_TABLE(L, literal, "INFO_ACTIVESOCKET", number, CURLINFO_ACTIVESOCKET);
	LUA_SET_TABLE(L, literal, "INFO_FTP_ENTRY_PATH", number, CURLINFO_FTP_ENTRY_PATH);

    return 1;
}
