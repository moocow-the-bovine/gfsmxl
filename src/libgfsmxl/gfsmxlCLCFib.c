/*-
 * Copyright 1997-2003 John-Mark Gurney.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	$Id$
 *
 * + modifications by moocow, Tue, 18 Dec 2007 08:26:12 +0100
 *   - specialized for gfsmxlCascadeLookupConfig* elements
 *
 */

#include <gfsmxlCLCFib.h>
#include <gfsmxlCLCFibPriv.h>
#include <gfsmxlCascadeLookup.h>

#include <limits.h>
#include <stdlib.h>

/*======================================================================
 * new inlined stuff
 */
static inline
int
gfsmxl_clc_fh_compare(struct gfsmxlCLCFibHeap_el *a, struct gfsmxlCLCFibHeap_el *b)
{
  return gfsmxl_cascade_lookup_config_fh_compare(a->fhe_data,b->fhe_data);
}

static inline
int
gfsmxl_clc_fh_comparedata(struct gfsmxlCascadeLookupConfig_ *data, struct gfsmxlCLCFibHeap_el *b)
{
  return gfsmxl_cascade_lookup_config_fh_compare(data,b->fhe_data);
}

/*======================================================================
 * Macros etc.
 */
#define swap(type, a, b)		\
		do {			\
			type c;		\
			c = a;		\
			a = b;		\
			b = c;		\
		} while (0)		\

#define INT_BITS        (sizeof(int) * 8)
static inline int
ceillog2(unsigned int a)
{
	int oa;
	int i;
	int b;

	oa = a;
	b = INT_BITS / 2;
	i = 0;
	while (b) {
		i = (i << 1);
		if (a >= (1 << b)) {
			a /= (1 << b);
			i = i | 1;
		} else
			a &= (1 << b) - 1;
		b /= 2;
	}
	if ((1 << i) == oa)
		return i;
	else
		return i + 1;
}

/*
 * Private Heap Functions
 */
static void
gfsmxl_clc_fh_deleteel(struct gfsmxlCLCFibHeap *h, struct gfsmxlCLCFibHeap_el *x)
{
        struct gfsmxlCascadeLookupConfig_ *data;

	data = x->fhe_data;

	gfsmxl_clc_fh_replacedata(h, x, h->fh_neginf);
	if (gfsmxl_clc_fh_extractminel(h) != x) {
		/*
		 * XXX - This should never happen as fh_replace should set it
		 * to min.
		 */
		abort();
	}

	x->fhe_data = data;
}

static void
gfsmxl_clc_fh_initheap(struct gfsmxlCLCFibHeap *new)
{
	new->fh_neginf = NULL;
	new->fh_n = 0;
	new->fh_Dl = -1;
	new->fh_cons = NULL;
	new->fh_min = NULL;
	new->fh_root = NULL;
#ifdef GFSMXL_CLC_FH_STATS
	new->fh_maxn = 0;
	new->fh_ninserts = 0;
	new->fh_nextracts = 0;
#endif
}

static void
gfsmxl_clc_fh_destroyheap(struct gfsmxlCLCFibHeap *h)
{
	h->fh_neginf = NULL;
	if (h->fh_cons != NULL)
		free(h->fh_cons);
	h->fh_cons = NULL;
	free(h);
}

struct gfsmxlCLCFibHeap *
gfsmxl_clc_fh_makeheap(void)
{
	struct gfsmxlCLCFibHeap *n;

	if ((n = malloc(sizeof *n)) == NULL)
		return NULL;

	gfsmxl_clc_fh_initheap(n);

	return n;
}

struct gfsmxlCascadeLookupConfig_ *
gfsmxl_clc_fh_setneginf(struct gfsmxlCLCFibHeap *h, struct gfsmxlCascadeLookupConfig_ *data)
{
	struct gfsmxlCascadeLookupConfig_ *old;

	old = h->fh_neginf;
	h->fh_neginf = data;

	return old;
}

struct gfsmxlCLCFibHeap *
gfsmxl_clc_fh_union(struct gfsmxlCLCFibHeap *ha, struct gfsmxlCLCFibHeap *hb)
{
	struct gfsmxlCLCFibHeap_el *x;

	if (ha->fh_root == NULL || hb->fh_root == NULL) {
		/* either one or both are empty */
		if (ha->fh_root == NULL) {
			gfsmxl_clc_fh_destroyheap(ha);
			return hb;
		} else {
			gfsmxl_clc_fh_destroyheap(hb);
			return ha;
		}
	}
	ha->fh_root->fhe_left->fhe_right = hb->fh_root;
	hb->fh_root->fhe_left->fhe_right = ha->fh_root;
	x = ha->fh_root->fhe_left;
	ha->fh_root->fhe_left = hb->fh_root->fhe_left;
	hb->fh_root->fhe_left = x;
	ha->fh_n += hb->fh_n;
	/*
	 * we probably should also keep stats on number of unions
	 */

	/* set fh_min if necessary */
	if (gfsmxl_clc_fh_compare(hb->fh_min, ha->fh_min) < 0)
		ha->fh_min = hb->fh_min;

	gfsmxl_clc_fh_destroyheap(hb);
	return ha;
}

void
gfsmxl_clc_fh_deleteheap(struct gfsmxlCLCFibHeap *h)
{
	/*
	 * We could do this even faster by walking each binomial tree, but
	 * this is simpler to code.
	 */
	while (h->fh_min != NULL)
		gfsmxl_clc_fhe_destroy(gfsmxl_clc_fh_extractminel(h));

	gfsmxl_clc_fh_destroyheap(h);
}

/*
 * Public void * Heap Functions
 */
/*
 * this will return these values:
 *	NULL	failed for some reason
 *	ptr	token to use for manipulation of data
 */
struct gfsmxlCLCFibHeap_el *
gfsmxl_clc_fh_insert(struct gfsmxlCLCFibHeap *h, struct gfsmxlCascadeLookupConfig_ *data)
{
	struct gfsmxlCLCFibHeap_el *x;

	if ((x = gfsmxl_clc_fhe_newelem()) == NULL)
		return NULL;

	/* just insert on root list, and make sure it's not the new min */
	x->fhe_data = data;

	gfsmxl_clc_fh_insertel(h, x);

	return x;
}

struct gfsmxlCascadeLookupConfig_ *
gfsmxl_clc_fh_min(struct gfsmxlCLCFibHeap *h)
{
	if (h->fh_min == NULL)
		return NULL;
	return h->fh_min->fhe_data;
}

struct gfsmxlCascadeLookupConfig_ *
gfsmxl_clc_fh_extractmin(struct gfsmxlCLCFibHeap *h)
{
	struct gfsmxlCLCFibHeap_el *z;
	struct gfsmxlCascadeLookupConfig_ *ret=NULL;

	if (h->fh_min != NULL) {
		z = gfsmxl_clc_fh_extractminel(h);
		ret = z->fhe_data;
#ifndef NO_FREE
		gfsmxl_clc_fhe_destroy(z);
#endif

	}

	return ret;
}

struct gfsmxlCascadeLookupConfig_ *
gfsmxl_clc_fh_replacedata(struct gfsmxlCLCFibHeap *h, struct gfsmxlCLCFibHeap_el *x, struct gfsmxlCascadeLookupConfig_ *data)
{
  struct gfsmxlCascadeLookupConfig_ *odata=x->fhe_data;
  struct gfsmxlCLCFibHeap_el *y;
  int r;

  odata = x->fhe_data;

  /*
   * we can increase a key by deleting and reinserting, that
   * requires O(lgn) time.
   */
  if ((r = gfsmxl_clc_fh_comparedata(data, x)) > 0) {
    /* XXX - bad code! */
    abort();
    gfsmxl_clc_fh_deleteel(h, x);
    
    x->fhe_data = data;
    
    gfsmxl_clc_fh_insertel(h, x);
    
    return odata;
  }

  x->fhe_data = data;

  /* because they are equal, we don't have to do anything */
  if (r == 0)
    return odata;

  y = x->fhe_p;

  if (y != NULL && gfsmxl_clc_fh_compare(x, y) <= 0) {
    gfsmxl_clc_fh_cut(h, x, y);
    gfsmxl_clc_fh_cascading_cut(h, y);
  }

  /*
   * the = is so that the call from fh_delete will delete the proper
   * element.
   */
  if (gfsmxl_clc_fh_compare(x, h->fh_min) <= 0)
    h->fh_min = x;

  return odata;
}

struct gfsmxlCascadeLookupConfig_ *
gfsmxl_clc_fh_delete(struct gfsmxlCLCFibHeap *h, struct gfsmxlCLCFibHeap_el *x)
{
        struct gfsmxlCascadeLookupConfig_ *k;

	k = x->fhe_data;
	gfsmxl_clc_fh_replacedata(h, x, h->fh_neginf);
	gfsmxl_clc_fh_extractmin(h);

	return k;
}

/*
 * Statistics Functions
 */
#ifdef GFSMXL_CLC_FH_STATS
int
gfsmxl_clc_fh_maxn(struct gfsmxlCLCFibHeap *h)
{
	return h->fh_maxn;
}

int
gfsmxl_clc_fh_ninserts(struct gfsmxlCLCFibHeap *h)
{
	return h->fh_ninserts;
}

int
gfsmxl_clc_fh_nextracts(struct gfsmxlCLCFibHeap *h)
{
	return h->fh_nextracts;
}
#endif

/*
 * begin of private element fuctions
 */
static struct gfsmxlCLCFibHeap_el *
gfsmxl_clc_fh_extractminel(struct gfsmxlCLCFibHeap *h)
{
	struct gfsmxlCLCFibHeap_el *ret;
	struct gfsmxlCLCFibHeap_el *x, *y, *orig;

	ret = h->fh_min;

	orig = NULL;
	/* put all the children on the root list */
	/* for true consistancy, we should use fhe_remove */
	for(x = ret->fhe_child; x != orig && x != NULL;) {
		if (orig == NULL)
			orig = x;
		y = x->fhe_right;
		x->fhe_p = NULL;
		gfsmxl_clc_fh_insertrootlist(h, x);
		x = y;
	}
	/* remove minimum from root list */
	gfsmxl_clc_fh_removerootlist(h, ret);
	h->fh_n--;

	/* if we aren't empty, consolidate the heap */
	if (h->fh_n == 0)
		h->fh_min = NULL;
	else {
		h->fh_min = ret->fhe_right;
		gfsmxl_clc_fh_consolidate(h);
	}

#ifdef GFSMXL_CLC_FH_STATS
	h->fh_nextracts++;
#endif

	return ret;
}

static void
gfsmxl_clc_fh_insertrootlist(struct gfsmxlCLCFibHeap *h, struct gfsmxlCLCFibHeap_el *x)
{
	if (h->fh_root == NULL) {
		h->fh_root = x;
		x->fhe_left = x;
		x->fhe_right = x;
		return;
	}

	gfsmxl_clc_fhe_insertafter(h->fh_root, x);
}

static void
gfsmxl_clc_fh_removerootlist(struct gfsmxlCLCFibHeap *h, struct gfsmxlCLCFibHeap_el *x)
{
	if (x->fhe_left == x)
		h->fh_root = NULL;
	else
		h->fh_root = gfsmxl_clc_fhe_remove(x);
}

static void
gfsmxl_clc_fh_consolidate(struct gfsmxlCLCFibHeap *h)
{
	struct gfsmxlCLCFibHeap_el **a;
	struct gfsmxlCLCFibHeap_el *w;
	struct gfsmxlCLCFibHeap_el *y;
	struct gfsmxlCLCFibHeap_el *x;
	int i;
	int d;
	int D;

	gfsmxl_clc_fh_checkcons(h);

	/* assign a the value of h->fh_cons so I don't have to rewrite code */
	D = h->fh_Dl + 1;
	a = h->fh_cons;

	for (i = 0; i < D; i++)
		a[i] = NULL;

	while ((w = h->fh_root) != NULL) {
		x = w;
		gfsmxl_clc_fh_removerootlist(h, w);
		d = x->fhe_degree;
		/* XXX - assert that d < D */
		while(a[d] != NULL) {
			y = a[d];
			if (gfsmxl_clc_fh_compare(x, y) > 0)
				swap(struct gfsmxlCLCFibHeap_el *, x, y);
			gfsmxl_clc_fh_heaplink(h, y, x);
			a[d] = NULL;
			d++;
		}
		a[d] = x;
	}
	h->fh_min = NULL;
	for (i = 0; i < D; i++)
		if (a[i] != NULL) {
			gfsmxl_clc_fh_insertrootlist(h, a[i]);
			if (h->fh_min == NULL || gfsmxl_clc_fh_compare(a[i], h->fh_min) < 0)
			  h->fh_min = a[i];
		}
}

static void
gfsmxl_clc_fh_heaplink(struct gfsmxlCLCFibHeap *h, struct gfsmxlCLCFibHeap_el *y, struct gfsmxlCLCFibHeap_el *x)
{
	/* make y a child of x */
	if (x->fhe_child == NULL)
		x->fhe_child = y;
	else
		gfsmxl_clc_fhe_insertbefore(x->fhe_child, y);
	y->fhe_p = x;
	x->fhe_degree++;
	y->fhe_mark = 0;
}

static void
gfsmxl_clc_fh_cut(struct gfsmxlCLCFibHeap *h, struct gfsmxlCLCFibHeap_el *x, struct gfsmxlCLCFibHeap_el *y)
{
	gfsmxl_clc_fhe_remove(x);
	y->fhe_degree--;
	gfsmxl_clc_fh_insertrootlist(h, x);
	x->fhe_p = NULL;
	x->fhe_mark = 0;
}

static void
gfsmxl_clc_fh_cascading_cut(struct gfsmxlCLCFibHeap *h, struct gfsmxlCLCFibHeap_el *y)
{
	struct gfsmxlCLCFibHeap_el *z;

	while ((z = y->fhe_p) != NULL) {
		if (y->fhe_mark == 0) {
			y->fhe_mark = 1;
			return;
		} else {
			gfsmxl_clc_fh_cut(h, y, z);
			y = z;
		}
	}
}

/*
 * begining of handling elements of fibheap
 */
static struct gfsmxlCLCFibHeap_el *
gfsmxl_clc_fhe_newelem()
{
	struct gfsmxlCLCFibHeap_el *e;

	if ((e = malloc(sizeof *e)) == NULL)
		return NULL;

	gfsmxl_clc_fhe_initelem(e);

	return e;
}

static void
gfsmxl_clc_fhe_initelem(struct gfsmxlCLCFibHeap_el *e)
{
	e->fhe_degree = 0;
	e->fhe_mark = 0;
	e->fhe_p = NULL;
	e->fhe_child = NULL;
	e->fhe_left = e;
	e->fhe_right = e;
	e->fhe_data = NULL;
}

static void
gfsmxl_clc_fhe_insertafter(struct gfsmxlCLCFibHeap_el *a, struct gfsmxlCLCFibHeap_el *b)
{
	if (a == a->fhe_right) {
		a->fhe_right = b;
		a->fhe_left = b;
		b->fhe_right = a;
		b->fhe_left = a;
	} else {
		b->fhe_right = a->fhe_right;
		a->fhe_right->fhe_left = b;
		a->fhe_right = b;
		b->fhe_left = a;
	}
}

static inline void
gfsmxl_clc_fhe_insertbefore(struct gfsmxlCLCFibHeap_el *a, struct gfsmxlCLCFibHeap_el *b)
{
	gfsmxl_clc_fhe_insertafter(a->fhe_left, b);
}

static struct gfsmxlCLCFibHeap_el *
gfsmxl_clc_fhe_remove(struct gfsmxlCLCFibHeap_el *x)
{
	struct gfsmxlCLCFibHeap_el *ret;

	if (x == x->fhe_left)
		ret = NULL;
	else
		ret = x->fhe_left;

	/* fix the parent pointer */
	if (x->fhe_p != NULL && x->fhe_p->fhe_child == x)
		x->fhe_p->fhe_child = ret;

	x->fhe_right->fhe_left = x->fhe_left;
	x->fhe_left->fhe_right = x->fhe_right;

	/* clear out hanging pointers */
	x->fhe_p = NULL;
	x->fhe_left = x;
	x->fhe_right = x;

	return ret;
}

static void
gfsmxl_clc_fh_checkcons(struct gfsmxlCLCFibHeap *h)
{
	int oDl;

	/* make sure we have enough memory allocated to "reorganize" */
	if (h->fh_Dl == -1 || h->fh_n > (1 << h->fh_Dl)) {
		oDl = h->fh_Dl;
		if ((h->fh_Dl = ceillog2(h->fh_n) + 1) < 8)
			h->fh_Dl = 8;
		if (oDl != h->fh_Dl)
			h->fh_cons = (struct gfsmxlCLCFibHeap_el **)realloc(h->fh_cons,
			    sizeof *h->fh_cons * (h->fh_Dl + 1));
		if (h->fh_cons == NULL)
			abort();
	}
}

static void
gfsmxl_clc_fh_insertel(struct gfsmxlCLCFibHeap *h, struct gfsmxlCLCFibHeap_el *x)
{
	gfsmxl_clc_fh_insertrootlist(h, x);

	if (h->fh_min == NULL || gfsmxl_clc_fh_compare(x,h->fh_min) < 0)
	  h->fh_min = x;

	h->fh_n++;

#ifdef GFSMXL_CLC_FH_STATS
	if (h->fh_n > h->fh_maxn)
		h->fh_maxn = h->fh_n;
	h->fh_ninserts++;
#endif

}
