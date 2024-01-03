/*
	Better String Library Lite version (hacked by novan)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <stddef.h>
#include "bstring.h"

/* =============== DEFINE =================================================== */
#ifndef bstr__alloc
#define bstr__alloc(x) malloc (x)
#endif

#ifndef bstr__free
#define bstr__free(p) free (p)
#endif

#ifndef bstr__realloc
#define bstr__realloc(p,x) realloc (p, x)
#endif

/* Just a length safe wrapper for memmove. */
#define bBlockCopy(D,S,L) { if ((L) > 0) memmove ((D),(S),(L)); }

/* Give MSVC some latitude for their non-support of vsnprintf */
#ifdef WIN32
#define exvsnprintf(r,b,n,f,a) {r = _vsnprintf (b,n,f,a);}
#else
#define exvsnprintf(r,b,n,f,a) {r = vsnprintf (b,n,f,a);}
#endif
#define START_VSNBUFF (16)

/* Reference building macros */
#define cstr2tbstr btfromcstr
#define btfromcstr(t,s) {                                            \
    (t).data = (unsigned char *) (s);                                \
    (t).slen = ((t).data) ? ((int) (strlen) ((char *)(t).data)) : 0; \
    (t).mlen = -1;                                                   \
}

/* ============================ internal function =========================== */

/* Compute the snapped size for a given requested size.  By snapping to powers
   of 2 like this, repeated reallocations are avoided. */
static int snapUpSize (int i) {
	if (i < 8) {
		i = 8;
	} else {
		unsigned int j;
		j = (unsigned int) i;

		j |= (j >>  1);
		j |= (j >>  2);
		j |= (j >>  4);
		j |= (j >>  8);		/* Ok, since int >= 16 bits */
#if (UINT_MAX != 0xffff)
		j |= (j >> 16);		/* For 32 bit int systems */
#if (UINT_MAX > 0xffffffffl)
		j |= (j >> 32);		/* For 64 bit int systems */
#endif
#endif
		/* Least power of two greater than i */
		j++;
		if ((int) j >= i) i = (int) j;
	}
	return i;
}

/* ============================ Implementation ============================== */

/*  int balloc (bstring b, int len)
 *
 *  Increase the size of the memory backing the bstring b to at least len.
 */
int balloc (bstring b, int len) {
	if (b == NULL || b->data == NULL || b->slen < 0 || b->mlen <= 0 ||
	    b->mlen < b->slen || len <= 0) {
		return BSTR_ERR;
	}

	if (len >= b->mlen) {
		unsigned char * x;

		if ((len = snapUpSize (len)) <= b->mlen) return BSTR_OK;

		/* Assume probability of a non-moving realloc is 0.125 */
		if (7 * b->mlen < 8 * b->slen) {

			/* If slen is close to mlen in size then use realloc to reduce
			   the memory defragmentation */

			x = (unsigned char *) bstr__realloc (b->data, len);
			if (x == NULL) return BSTR_ERR;
		} else {

			/* If slen is not close to mlen then avoid the penalty of copying
			   the extra bytes that are allocated, but not considered part of
			   the string */

			x = (unsigned char *) bstr__alloc (len);
			if (x == NULL) {
				x = (unsigned char *) bstr__realloc (b->data, len);
				if (x == NULL) return BSTR_ERR;
			} else {
				if (b->slen) memcpy ((char *) x, (char *) b->data, b->slen);
				bstr__free (b->data);
			}
		}
		b->data = x;
		b->mlen = len;
		b->data[b->slen] = (unsigned char) '\0';
	}

	return BSTR_OK;
}


/*  bstring bstrcpy (const bstring b)
 *
 *  Create a copy of the bstring b.
 */
bstring bstrcpy (const bstring b) {
bstring b0;
int i,j;

	/* Attempted to copy an invalid string? */
	if (b == NULL || b->slen < 0 || b->data == NULL) return NULL;

	b0 = (bstring) bstr__alloc (sizeof (struct tagbstring));
	if (b0 == NULL) {
		/* Unable to allocate memory for string header */
		return NULL;
	}

	i = b->slen;
	j = snapUpSize (i + 1);

	b0->data = (unsigned char *) bstr__alloc (j);
	if (b0->data == NULL) {
		j = i + 1;
		b0->data = (unsigned char *) bstr__alloc (j);
		if (b0->data == NULL) {
			/* Unable to allocate memory for string data */
			bstr__free (b0);
			return NULL;
		}
	}

	b0->mlen = j;
	b0->slen = i;

	if (i) memcpy ((char *) b0->data, (char *) b->data, i);
	b0->data[b0->slen] = (unsigned char) '\0';

	return b0;
}

/*  bstring bfromcstr (const char * str)
 *
 *  Create a bstring which contains the contents of the '\0' terminated char *
 *  buffer str.
 */
bstring bfromcstr (const char * str) {
bstring b;
int i;
size_t j;

	if (str == NULL) return NULL;
	j = (strlen) (str);
	i = snapUpSize ((int) (j + (2 - (j != 0))));
	if (i <= (int) j) return NULL;

	b = (bstring) bstr__alloc (sizeof (struct tagbstring));
	if (NULL == b) return NULL;
	b->slen = (int) j;
	if (NULL == (b->data = (unsigned char *) bstr__alloc (b->mlen = i))) {
		bstr__free (b);
		return NULL;
	}

	memcpy (b->data, str, j+1);
	return b;
}

/*  char * bstr2cstr (const bstring s, char z)
 *
 *  Create a '\0' terminated char * buffer which is equal to the contents of
 *  the bstring s, except that any contained '\0' characters are converted
 *  to the character in z. This returned value should be freed with a
 *  bcstrfree () call, by the calling application.
 */
char * bstr2cstr (const bstring b) {
int i,l;
char * r;

	if (b == NULL || b->slen < 0 || b->data == NULL) return NULL;
	l = b->slen;
	r = (char *) bstr__alloc (l + 1);
	if (r == NULL) return r;

	for (i=0; i < l; i ++) {
		r[i] = (char) (b->data[i]);
	}

	r[l] = (unsigned char) '\0';

	return r;
}

/*  int bcstrfree (const char * s)
 *
 *  Frees a C-string generated by bstr2cstr ().  This is normally unnecessary
 *  since it just wraps a call to bstr__free (), however, if bstr__alloc ()
 *  and bstr__free () have been redefined as a macros within the bstrlib
 *  module (via defining them in memdbg.h after defining
 *  BSTRLIB_MEMORY_DEBUG) with some difference in behaviour from the std
 *  library functions, then this allows a correct way of freeing the memory
 *  that allows higher level code to be independent from these macro
 *  redefinitions.
 */
int bcstrfree (char * s) {
	if (s) {
		bstr__free (s);
		return BSTR_OK;
	}
	return BSTR_ERR;
}


/*  int bdestroy (bstring b)
 *
 *  bstr__free up the bstring.  Note that if b is detectably invalid or not writable
 *  then no action is performed and BSTR_ERR is returned.  Like a freed memory
 *  allocation, dereferences, writes or any other action on b after it has
 *  been bdestroyed is undefined.
 */
int bdestroy (bstring b) {
	if (b == NULL || b->slen < 0 || b->mlen <= 0 || b->mlen < b->slen ||
	    b->data == NULL)
		return BSTR_ERR;

	bstr__free (b->data);

	/* In case there is any stale usage, there is one more chance to
	   notice this error. */

	b->slen = -1;
	b->mlen = -__LINE__;
	b->data = NULL;

	bstr__free (b);
	return 0;
}


/*  int bconcat (bstring b0, const bstring b1)
 *
 *  Concatenate the bstring b1 to the bstring b0.
 */
int bconcat (bstring b0, const bstring b1) {
int len, d;
bstring aux = b1;

	if (b0 == NULL || b1 == NULL || b0->data == NULL || b1->data == NULL) return BSTR_ERR;

	d = b0->slen;
	len = b1->slen;
	if ((d | (b0->mlen - d) | len) < 0) return BSTR_ERR;

	if (b0->mlen <= d + len + 1) {
		ptrdiff_t pd;
		if (0 <= (pd = b1->data - b0->data) && pd < b0->mlen) {
			if (NULL == (aux = bstrcpy (b1))) return BSTR_ERR;
		}
		if (balloc (b0, d + len + 1) != BSTR_OK) {
			if (aux != b1) bdestroy (aux);
			return BSTR_ERR;
		}
	}

	bBlockCopy (&b0->data[d], &aux->data[0], len);
	b0->data[d + len] = (unsigned char) '\0';
	b0->slen += len;
	if (aux != b1) bdestroy (aux);
	return 0;
}

/*  int bconchar (bstring b, char c)
 *
 *  Concatenate the single character c to the bstring b.
 */
int bconchar (bstring b, char c) {
int d;

	if (b == NULL) return BSTR_ERR;
	d = b->slen;
	if ((d | (b->mlen - d)) < 0 || balloc (b, d + 2) != BSTR_OK) return BSTR_ERR;
	b->data[d] = (unsigned char) c;
	b->data[d + 1] = (unsigned char) '\0';
	b->slen++;
	return 0;
}


/*  int bcatcstr (bstring b, const char * s)
 *
 *  Concatenate a char * string to a bstring.
 */
int bcatcstr (bstring b, const char * s) {
struct tagbstring t;
char * d;
int i, l;

	if (b == NULL || b->data == NULL || b->slen < 0 || b->mlen < b->slen
	 || b->mlen <= 0 || s == NULL) return BSTR_ERR;

	/* Optimistically concatenate directly */
	l = b->mlen - b->slen;
	d = (char *) &b->data[b->slen];
	for (i=0; i < l; i++) {
		if ((*d++ = *s++) == '\0') {
			b->slen += i;
			return BSTR_OK;
		}
	}
	b->slen += i;

	/* Need to explicitely resize and concatenate tail */
	cstr2tbstr (t, s);
	return bconcat (b, &t);
}


/*  int bcatblk (bstring b, unsigned char * s, int len)
 *
 *  Concatenate a fixed length buffer to a bstring.
 */
int bcatblk (bstring b, const unsigned char * s, int len) {
struct tagbstring t;
int nl;

	if (b == NULL || b->data == NULL || b->slen < 0 || b->mlen < b->slen
	 || b->mlen <= 0 || s == NULL || len < 0) return BSTR_ERR;

	if ((nl = b->slen + len) < 0) return BSTR_ERR; /* Overflow? */
	if (nl < b->mlen) {
		bBlockCopy (&b->data[b->slen], s, len);
		b->slen = nl;
		b->data[nl] = '\0';
		return BSTR_OK;
	}
	t.slen = len;
    t.mlen = -1;
    t.data = (unsigned char *) (s);
	return bconcat (b, &t);
}


/*  int bdelete (bstring b, int pos, int len)
 *
 *  Removes characters from pos to pos+len-1 inclusive and shifts the tail of
 *  the bstring starting from pos+len to pos.  len must be positive for this
 *  call to have any effect.  The section of the string described by (pos,
 *  len) is clamped to boundaries of the bstring b.
 */
int bdelete (bstring b, int pos, int len) {
	/* Clamp to left side of bstring */
	if (pos < 0) {
		len += pos;
		pos = 0;
	}

	if (len < 0 || b == NULL || b->data == NULL || b->slen < 0 ||
	    b->mlen < b->slen || b->mlen <= 0)
		return BSTR_ERR;
	if (len > 0 && pos < b->slen) {
		if (pos + len >= b->slen) {
			b->slen = pos;
		} else {
			bBlockCopy ((char *) (b->data + pos),
			            (char *) (b->data + pos + len),
			            b->slen - (pos+len));
			b->slen -= len;
		}
		b->data[b->slen] = (unsigned char) '\0';
	}
	return BSTR_OK;
}


/*  int btrunc (bstring b, int n)
 *
 *  Truncate the bstring to at most n characters.
 */
int btrunc (bstring b, int n) {
	if (n < 0 || b == NULL || b->data == NULL || b->mlen < b->slen ||
	    b->slen < 0 || b->mlen <= 0) return BSTR_ERR;
	if (b->slen > n) {
		b->slen = n;
		b->data[n] = (unsigned char) '\0';
	}
	return 0;
}


/*  bstring bfromcstralloc (int mlen, const char * str)
 *
 *  Create a bstring which contains the contents of the '\0' terminated char *
 *  buffer str.  The memory buffer backing the string is at least len
 *  characters in length.
 */
bstring bfromcstralloc (int mlen, const char * str) {
bstring b;
int i;
size_t j;

	if (str == NULL) return NULL;
	j = (strlen) (str);
	i = snapUpSize ((int) (j + (2 - (j != 0))));
	if (i <= (int) j) return NULL;

	b = (bstring) bstr__alloc (sizeof (struct tagbstring));
	if (b == NULL) return NULL;
	b->slen = (int) j;
	if (i < mlen) i = mlen;

	if (NULL == (b->data = (unsigned char *) bstr__alloc (b->mlen = i))) {
		bstr__free (b);
		return NULL;
	}

	memcpy (b->data, str, j+1);
	return b;
}


/*  bstring bformat (const char * fmt, ...)
 *
 *  Takes the same parameters as printf (), but rather than outputting results
 *  to stdio, it forms a bstring which contains what would have been output.
 *  Note that if there is an early generation of a '\0' character, the
 *  bstring will be truncated to this end point.
 */
bstring bformat (const char * fmt, ...) {
va_list arglist;
bstring buff;
int n, r;

	if (fmt == NULL) return NULL;

	/* Since the length is not determinable beforehand, a search is
	   performed using the truncating "vsnprintf" call (to avoid buffer
	   overflows) on increasing potential sizes for the output result. */

	if ((n = (int) (2*strlen (fmt))) < START_VSNBUFF) n = START_VSNBUFF;
	if (NULL == (buff = bfromcstralloc (n + 2, ""))) {
		n = 1;
		if (NULL == (buff = bfromcstralloc (n + 2, ""))) return NULL;
	}

	for (;;) {
		va_start (arglist, fmt);
		exvsnprintf (r, (char *) buff->data, n + 1, fmt, arglist);
		va_end (arglist);

		buff->data[n] = (unsigned char) '\0';
		buff->slen = (int) (strlen) ((char *) buff->data);

		if (buff->slen < n) break;

		if (r > n) n = r; else n += n;

		if (BSTR_OK != balloc (buff, n + 2)) {
			bdestroy (buff);
			return NULL;
		}
	}

	return buff;
}

/*  int bformata (bstring b, const char * fmt, ...)
 *
 *  After the first parameter, it takes the same parameters as printf (), but
 *  rather than outputting results to stdio, it appends the results to
 *  a bstring which contains what would have been output. Note that if there
 *  is an early generation of a '\0' character, the bstring will be truncated
 *  to this end point.
 */
int bformata (bstring b, const char * fmt, ...) {
va_list arglist;
bstring buff;
int n, r;

	if (b == NULL || fmt == NULL || b->data == NULL || b->mlen <= 0
	 || b->slen < 0 || b->slen > b->mlen) return BSTR_ERR;

	/* Since the length is not determinable beforehand, a search is
	   performed using the truncating "vsnprintf" call (to avoid buffer
	   overflows) on increasing potential sizes for the output result. */

	if ((n = (int) (2*strlen (fmt))) < START_VSNBUFF) n = START_VSNBUFF;
	if (NULL == (buff = bfromcstralloc (n + 2, ""))) {
		n = 1;
		if (NULL == (buff = bfromcstralloc (n + 2, ""))) return BSTR_ERR;
	}

	for (;;) {
		va_start (arglist, fmt);
		exvsnprintf (r, (char *) buff->data, n + 1, fmt, arglist);
		va_end (arglist);

		buff->data[n] = (unsigned char) '\0';
		buff->slen = (int) (strlen) ((char *) buff->data);

		if (buff->slen < n) break;

		if (r > n) n = r; else n += n;

		if (BSTR_OK != balloc (buff, n + 2)) {
			bdestroy (buff);
			return BSTR_ERR;
		}
	}

	r = bconcat (b, buff);
	bdestroy (buff);
	return r;
}
