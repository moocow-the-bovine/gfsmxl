
/*=============================================================================*\
 * File: gfsmxlCascadeLookup.h
 * Author: Bryan Jurish <moocow@ling.uni-potsdam.de>
 * Description: finite state machine library: lookup cascade: lookup routines
 *
 * Copyright (c) 2007,2008 Bryan Jurish.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *=============================================================================*/

/** \file gfsmxlCascadeLookup.h
 *  \brief First stab at transducer cascade lookup routines
 */

#ifndef _GFSMXL_CASCADE_LOOKUP_H
#define _GFSMXL_CASCADE_LOOKUP_H

#include <gfsmxlCascade.h>
#include <gfsmxlCLCFib.h>

/*======================================================================
 * Types
 */

/// Alias for 'struct gfsmxlCLCFibHeap'
typedef struct gfsmxlCLCFibHeap gfsmxlFibHeap;
//typedef struct fibheap gfsmxlFibHeap;

/// struct for cascade lookup heap element data
typedef struct gfsmxlCascadeLookupConfig_ {
  gfsmxlCascade        *csc;     /**< Underlying cascade */
  gfsmxlCascadeStateId  qids;    /**< current state in cascade */
  guint32              ipos;    /**< current position in input string */
  gfsmStateId          oid;     /**< current state in output trie (identifies output string) */
  gfsmStateId          rid;     /**< current state in output automaton */
  gfsmWeight           w;       /**< accumulated weight */
  //struct gfsmxlCascadeLookupConfig_ *parent; /* parent configuration, for backtracking */
} gfsmxlCascadeLookupConfig;

/// Persistent type for best-first cascade lookups
typedef struct {
  //
  //-- user data
  gfsmxlCascade  *csc;        /**< Underlying cascade */
  gfsmWeight     max_w;      /**< Maximum weight */
  guint          max_paths;  /**< Maximum number of output paths to create */
  guint          max_ops;    /**< Maximum number of elementary lookup operations */
  //
  //-- low-level data
  gfsmxlFibHeap             *heap;        /**< Heap for storing lookup configurations */
  GHashTable               *configs;     /**< Set of extant lookup-configurations */
  gfsmAutomaton            *otrie;       /**< output-string trie */
  guint                     n_ops;       /**< Number of elementary operations in current run */
  gfsmxlCascadeLookupConfig  heap_neginf; /**< "Negative infinity" element for heap comparison */
} gfsmxlCascadeLookup;

/*======================================================================
 * Constructors, etc.
 */
/// \name Constructors etc.
//@{

/** Create a new ::gfsmxlCascadeLookup, specifying initial data (csc will be freed with the returned lookup object) */
GFSM_INLINE
gfsmxlCascadeLookup *gfsmxl_cascade_lookup_new_full(gfsmxlCascade *csc, gfsmWeight max_w, guint max_paths, guint max_ops);

/** Create a new empty ::gfsmxlCascadeLookup */
GFSM_INLINE
gfsmxlCascadeLookup *gfsmxl_cascade_lookup_new(void);

/** (re-)set underlying cascade of a ::gfsmxlCascadeLookup.
 *  \li frees old cascade, if any
 *  \li \a csc will be freed with \a cl
 */
GFSM_INLINE
void gfsmxl_cascade_lookup_set_cascade(gfsmxlCascadeLookup *cl, gfsmxlCascade *csc);

/** Destroy a ::gfsmxlCascadeLookup.  Also frees \a cl->heap, \a cl->csc, and its indexed automata  */
GFSM_INLINE
void gfsmxl_cascade_lookup_free(gfsmxlCascadeLookup *cl);

//@}

/*======================================================================
 * Usage
 */
//@{

/** Resets state of a ::gfsmxlCascadeLookup, preparing it for another lookup
 *  \li implicitly called by gfsm_cascade_lookup_nbest()
 */
void gfsmxl_cascade_lookup_reset(gfsmxlCascadeLookup *cl);

/** Lookup first n "best" path(s) for input labels \a input in \a cl->csc,
 *  populating \a result as we go.  If \a result is passed as NULL, a new
 *  ::gfsmAutomaton will be created.
 *  \returns result if non-NULL, otherwise a new ::gfsmAutomaton.
 */
gfsmAutomaton *gfsmxl_cascade_lookup_nbest(gfsmxlCascadeLookup *cl, gfsmLabelVector *input, gfsmAutomaton *result);

//@}

/*======================================================================
 * Low-level: gfsmxlCascadeLookupConfig
 */
//@{

/** Heap-element comparison function for ::gfsmxlCascadeLookupConfig (inline)
 *  \li Compares only weights and input positions!
 *  \li Behaves something like the perl code:
 *      <tt>(lc1->w <=> lc2->w) || (lc2->ipos <=> lc1->ipos)</tt>
 */
GFSM_INLINE
int gfsmxl_cascade_lookup_config_fh_compare(gfsmxlCascadeLookupConfig *lc1, gfsmxlCascadeLookupConfig *lc2);

/** Hash function for ::gfsmxlCascadeLookupConfig; castable to GHashFunc
 *  \li Hashes only (lc->ipos,lc->oid,lc->qids)
 */
guint gfsmxl_cascade_lookup_config_ht_hash(gfsmxlCascadeLookupConfig *lc);

/** Equality predicate for ::gfsmxlCascadeLookupConfig; castable to GEqualFunc
 *  \li Tests only (lcX->ipos,lcX->oid,lcX->qids)
 */
gboolean gfsmxl_cascade_lookup_config_ht_equal(gfsmxlCascadeLookupConfig *lc1, gfsmxlCascadeLookupConfig *lc2);

/** Allocate a ::gfsmxlCascadeLookupConfig.
 *  \param csc  underlying cascade
 *  \param qids state ids for the config (will be copied into cfg->qids)
 *  \param w    initial weight for the config
 */
GFSM_INLINE
gfsmxlCascadeLookupConfig *gfsmxl_cascade_lookup_config_new_full(gfsmxlCascade        *csc,
							       gfsmxlCascadeStateId  qids,
							       guint32              ipos,
							       gfsmStateId          oid,
							       gfsmStateId          rid,
							       gfsmWeight           w
							       //,gfsmxlCascadeLookupConfig *parent
							       );

/** Allocate a ::gfsmxlCascadeLookupConfig for \a cl, including state-id data */
GFSM_INLINE
gfsmxlCascadeLookupConfig *gfsmxl_cascade_lookup_config_new(gfsmxlCascadeLookup *cl);

/** Copy a ::gfsmCascadeLookupConfig \a src to \a dst, which must already be allocated. \returns \a dst */
GFSM_INLINE
void gfsmxl_cascade_lookup_config_copy(gfsmxlCascadeLookupConfig *dst, gfsmxlCascadeLookupConfig *src);

/** Create and return an exact copy of a ::gfsmxlCascadeLookupConfig \a src, including state-id data
 *  \li currently just a wrapper for gfsmxl_cascade_lookup_config_new_full()
 */
GFSM_INLINE
gfsmxlCascadeLookupConfig *gfsmxl_cascade_lookup_config_clone(gfsmxlCascadeLookupConfig *src);

/** Free a ::gfsmxlCascadeLookupConfig as allocated by one of the gfsmxl_cascade_lookup_config_new() functions,
 *  including state-id data (inline version)
 */
GFSM_INLINE
void gfsmxl_cascade_lookup_config_free_inline(gfsmxlCascadeLookupConfig *lc);

/** Free a ::gfsmxlCascadeLookupConfig, including state-id data (extern version) */
void gfsmxl_cascade_lookup_config_free(gfsmxlCascadeLookupConfig *lc);

//@}

/*======================================================================
 * END
 */

//-- inline definitions
#ifdef GFSM_INLINE_ENABLED
# include <gfsmxlCascadeLookup.hi>
#endif

#endif /* _GFSMXL_CASCADE_LOOKUP_H */
