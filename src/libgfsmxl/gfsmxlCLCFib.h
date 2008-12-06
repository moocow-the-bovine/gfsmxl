/*-
 * Copyright 1997, 1998-2003 John-Mark Gurney.
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

#ifndef _GFSMXL_CLC_FIB_H_
#define _GFSMXL_CLC_FIB_H_

#include <gfsm.h>
#include <gfsmxlConfig.h>

/// type for heaps of ::gfsmxlCascadeLookupConfig*
struct gfsmxlCLCFibHeap;

/// type for ::gfsmxlCascadeLookupConfig* heap elements
struct gfsmxlCLCFibHeap_el;

/// forward declaration
struct gfsmxlCascadeLookupConfig_;

/*======================================================================
 * Constructors etc.
 */

/** create a new ::gfsmxlCLCFibHeap */
struct gfsmxlCLCFibHeap *gfsmxl_clc_fh_makeheap(void);

/** set "negative infinity" data element (absolute minimum), \returns old neginf */
struct gfsmxlCascadeLookupConfig_ *gfsmxl_clc_fh_setneginf(struct gfsmxlCLCFibHeap *h, struct gfsmxlCascadeLookupConfig_ *data);

/** insert a new config into the heap */
struct gfsmxlCLCFibHeap_el *gfsmxl_clc_fh_insert(struct gfsmxlCLCFibHeap *h, struct gfsmxlCascadeLookupConfig_ *data);

/*======================================================================
 * Manipulation
 */

/** Extract minimum element from \a heap */
struct gfsmxlCascadeLookupConfig_ *gfsmxl_clc_fh_extractmin(struct gfsmxlCLCFibHeap *);

/** Peek at minimum element from \a heap */
struct gfsmxlCascadeLookupConfig_ *gfsmxl_clc_fh_min(struct gfsmxlCLCFibHeap *);

/** Replace an element's data \returns old data */
struct gfsmxlCascadeLookupConfig_ *
gfsmxl_clc_fh_replacedata(struct gfsmxlCLCFibHeap *, struct gfsmxlCLCFibHeap_el *, struct gfsmxlCascadeLookupConfig_ *);

/** Delete an element \returns element data */
struct gfsmxlCascadeLookupConfig_ *gfsmxl_clc_fh_delete(struct gfsmxlCLCFibHeap *, struct gfsmxlCLCFibHeap_el *);

/** Destroy a heap */
void gfsmxl_clc_fh_deleteheap(struct gfsmxlCLCFibHeap *);

/** Union of two heaps */
struct gfsmxlCLCFibHeap *gfsmxl_clc_fh_union(struct gfsmxlCLCFibHeap *, struct gfsmxlCLCFibHeap *);

#ifdef GFSMXL_CLC_FH_STATS
int gfsmxl_clc_fh_maxn(struct gfsmxlCLCFibHeap *);
int gfsmxl_clc_fh_ninserts(struct gfsmxlCLCFibHeap *);
int gfsmxl_clc_fh_nextracts(struct gfsmxlCLCFibHeap *);
#endif

#endif /* _GFSMXL_CLC_FIB_H_ */
