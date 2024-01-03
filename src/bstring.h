/*
	Better String Library Lite version (hacked by novan)
	16/06/2007, Jakarta, Taman Rawa Pening, Bendungan Hilir
	
Usage:
int main(int argc,char *argv[]){
	bstring a;

	a = vstr_new("");
	
	//use this one, if we've known the size of additional string 
	//thus avoiding realloc and improve perfomance
	//a = vstr_new_n(100,"");	
	vstr_add_str(a,"Hello World is not enough");
	vstr_add_str(a," for me");
	vstr_add_chr(a,'!');
	vstr_add_strf(a," Digit:%d\t(%s)",12345,"string");

	printf("Data: [%s]\nSLen: %d\nMLen: %d\n",a->data,a->slen,a->mlen);
	
	vstr_free(a);
	return 0;
}
*/

#ifndef BSTRLIB_INCLUDE
#define BSTRLIB_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

/* ========================== DEFINE =============================== */
#define BSTR_ERR (-1)
#define BSTR_OK (0)
#define BSTR_BS_BUFF_LENGTH_GET (0)

/* For novan's convenient */
#define vstr_new bfromcstr
#define vstr_new_n bfromcstralloc
#define vstr_add_chr bconchar
#define vstr_add_str bcatcstr
#define vstr_add_str_n bcatblk
#define vstr_add_strf bformata
#define vstr_free bdestroy
#define vstr_reset(s) btrunc((s),0)

/* ================ STRUCT ========================================= */
typedef struct tagbstring * bstring;
struct tagbstring {
	int mlen;
	int slen;
	unsigned char * data;
};


/* ======================== Prototype =============================== */
/* Space allocation hinting function */
extern int balloc (bstring s, int len);

/* Copy functions */
#define cstr2bstr bfromcstr
extern bstring bfromcstr (const char * str);
extern bstring bfromcstralloc (int mlen, const char * str) ;
extern char * bstr2cstr (const bstring s);
extern int bcstrfree (char * s);
extern bstring bstrcpy (const bstring b);

/* Destroy function */
extern int bdestroy (bstring b);

/* Various standard manipulations */
extern int bconchar (bstring b0, char c);
extern int bcatcstr (bstring b, const char * s);
extern int bconcat (bstring b0, const bstring b1);
extern int bcatblk (bstring b, const unsigned char * s, int len);
extern int bdelete (bstring s1, int pos, int len);
extern int btrunc (bstring b, int n);
extern bstring bformat (const char * fmt, ...);
extern int bformata (bstring b, const char * fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
