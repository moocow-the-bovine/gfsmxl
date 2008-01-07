/*-
 * Copyright 1997, 1999-2003 John-Mark Gurney.
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
 * + modified by moocow Tue, 18 Dec 2007 08:27:14 +0100
 *   - specialized for gfsmxlCascadeLookupConfig elements
 *
 */

#ifndef _GFSMXL_CLC_FIBPRIV_H_
#define _GFSMXL_CLC_FIBPRIV_H_

struct gfsmxlCLCFibHeap_el;
struct gfsmxlCascadeLookupConfig_;

/*
 * global heap operations
 */
struct gfsmxlCLCFibHeap {
	int	fh_n;
	int	fh_Dl;
	struct	gfsmxlCLCFibHeap_el **fh_cons;
	struct	gfsmxlCLCFibHeap_el *fh_min;
	struct	gfsmxlCLCFibHeap_el *fh_root;
        struct  gfsmxlCascadeLookupConfig_ *fh_neginf; /**< "minimum" possible element data */
  /*int	fh_keys		: 1; */
#ifdef GFSMXL_CLC_FH_STATS
	int	fh_maxn;
	int	fh_ninserts;
	int	fh_nextracts;
#endif
};

static void gfsmxl_clc_fh_initheap(struct gfsmxlCLCFibHeap *);
static void gfsmxl_clc_fh_insertrootlist(struct gfsmxlCLCFibHeap *, struct gfsmxlCLCFibHeap_el *);
static void gfsmxl_clc_fh_removerootlist(struct gfsmxlCLCFibHeap *, struct gfsmxlCLCFibHeap_el *);
static void gfsmxl_clc_fh_consolidate(struct gfsmxlCLCFibHeap *);
static void gfsmxl_clc_fh_heaplink(struct gfsmxlCLCFibHeap *h, struct gfsmxlCLCFibHeap_el *y,
			struct gfsmxlCLCFibHeap_el *x);
static void gfsmxl_clc_fh_cut(struct gfsmxlCLCFibHeap *, struct gfsmxlCLCFibHeap_el *, struct gfsmxlCLCFibHeap_el *);
static void gfsmxl_clc_fh_cascading_cut(struct gfsmxlCLCFibHeap *, struct gfsmxlCLCFibHeap_el *);
static struct gfsmxlCLCFibHeap_el *gfsmxl_clc_fh_extractminel(struct gfsmxlCLCFibHeap *);
static void gfsmxl_clc_fh_checkcons(struct gfsmxlCLCFibHeap *h);
static void gfsmxl_clc_fh_destroyheap(struct gfsmxlCLCFibHeap *h);

/*
static int gfsmxl_clc_fh_compare    (struct gfsmxlCLCFibHeap *h,
				    struct gfsmxlCLCFibHeap_el *a,
				    struct gfsmxlCLCFibHeap_el *b
				    );
static int gfsmxl_clc_fh_comparedata(struct gfsmxlCLCFibHeap *h,
				    int key,
				    struct gfsmxlCascadeLookupConfig_ *data,
				    struct gfsmxlCLCFibHeap_el *b
				    );
*/
static int gfsmxl_clc_fh_compare    (struct gfsmxlCLCFibHeap_el *a, struct gfsmxlCLCFibHeap_el *b);
static int gfsmxl_clc_fh_comparedata(struct gfsmxlCascadeLookupConfig_ *data, struct gfsmxlCLCFibHeap_el *b);


static void gfsmxl_clc_fh_insertel(struct gfsmxlCLCFibHeap *h, struct gfsmxlCLCFibHeap_el *x);
static void gfsmxl_clc_fh_deleteel(struct gfsmxlCLCFibHeap *h, struct gfsmxlCLCFibHeap_el *x);

/*
 * specific node operations
 */
struct gfsmxlCLCFibHeap_el {
	int	fhe_degree;
	int	fhe_mark;
	struct	gfsmxlCLCFibHeap_el *fhe_p;
	struct	gfsmxlCLCFibHeap_el *fhe_child;
	struct	gfsmxlCLCFibHeap_el *fhe_left;
	struct	gfsmxlCLCFibHeap_el *fhe_right;
  //--
  //int	fhe_key;
  struct gfsmxlCascadeLookupConfig_ *fhe_data;
};

static struct gfsmxlCLCFibHeap_el *gfsmxl_clc_fhe_newelem(void);
static void gfsmxl_clc_fhe_initelem(struct gfsmxlCLCFibHeap_el *);
static void gfsmxl_clc_fhe_insertafter(struct gfsmxlCLCFibHeap_el *a, struct gfsmxlCLCFibHeap_el *b);
static inline void gfsmxl_clc_fhe_insertbefore(struct gfsmxlCLCFibHeap_el *a, struct gfsmxlCLCFibHeap_el *b);
static struct gfsmxlCLCFibHeap_el *gfsmxl_clc_fhe_remove(struct gfsmxlCLCFibHeap_el *a);
#define	gfsmxl_clc_fhe_destroy(x)	free((x))

/*
 * general functions
 */
static inline int ceillog2(unsigned int a);

#endif /* _FIBPRIV_H_ */
