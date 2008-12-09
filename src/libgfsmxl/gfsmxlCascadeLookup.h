
/*=============================================================================*\
 * File: gfsmxlCascadeLookup.h
 * Author: Bryan Jurish <moocow@ling.uni-potsdam.de>
 * Description: finite state machine library: lookup cascade: lookup routines
 *
 * Copyright (c) 2007-2008 Bryan Jurish.
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

/// struct for cascade lookup heap element backtrace "pointers"
typedef struct gfsmxlCascadeLookupBacktrace_ {
  struct gfsmxlCascadeLookupConfig_ *prev; /**< backtrace: parent configuration */
  gfsmLabelId                        lo;   /**< backtrace: arc: lower label */
  gfsmLabelId                        hi;   /**< backtrace: arc: upper label */
  gfsmWeight                         aw;   /**< backtrace: arc: weight */
  gfsmWeight                         fw;   /**< backtrace: final: weight */
} gfsmxlCascadeLookupBacktrace;

/// struct for cascade lookup heap element data
typedef struct gfsmxlCascadeLookupConfig_ {
  gfsmxlCascade         *csc;     /**< Underlying cascade */
  gfsmxlCascadeStateId   qids;    /**< current state in cascade */
  guint32                ipos;    /**< current position in input string */
  gfsmStateId            oid;     /**< current state in output trie (identifies output string) */
  gfsmStateId            rid;     /**< current state in output automaton (on backtrace only) */
  gfsmWeight             w;       /**< accumulated weight */
  gfsmxlCascadeLookupBacktrace bt; /**< backtrace */
} gfsmxlCascadeLookupConfig;

/** \brief Singly-linked list of ::gfsmxlCascadeLookupConfig elements.
 *  \detail Used by ::gfsmxlCascadeLookup.configs
 */
typedef GSList gfsmxlCascadeLookupConfigList;

/// Persistent type for best-first cascade lookups
typedef struct {
  //
  //-- user data
  gfsmxlCascade  *csc;       /**< Underlying cascade */
  gfsmWeight     max_w;      /**< Maximum weight */
  guint          max_paths;  /**< Maximum number of output paths to create */
  guint          max_ops;    /**< Maximum number of elementary lookup operations */
  //
  //-- low-level data
  gfsmxlFibHeap             *heap;        /**< Heap for storing lookup configurations */
  GHashTable                *configs;     /**< Set of extant lookup-configurations: cfg_key => GSList(configs_for_key) */
  gfsmAutomaton             *otrie;       /**< output-string trie */
  gfsmxlCascadeLookupConfigList *finals;  /**< GSList of collected final ::gfsmxlCascadeLookupConfig */
  guint                      n_ops;       /**< Number of elementary operations in current run */
  gfsmxlCascadeLookupConfig  heap_neginf; /**< "Negative infinity" element for heap comparison */
} gfsmxlCascadeLookup;


/*======================================================================
 * Constants
 */
extern const gfsmxlCascadeLookupBacktrace gfsmxl_bt_null; /*< "empty" backtrace */

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
 * gfsmxlCascadeLookup API
 */
/// \name gfsmxlCascadeLookup API
//@{

/** Get best config matching \a cfg_tmp (according to gfsmxl_lookup_config_ht_hash(), gfsmxl_lookup_config_ht_equal()).
 *  Updates administrative data <tt>(cl->heap, cl->configs)</tt> appropriately.
 *  \param cl      lookup object
 *  \param cfg_key configuration to match or create
 *  \returns NULL if an equal-or-better config is already known to \a cl, otherwise a clone of \a cfg_tmp
 */
GFSM_INLINE
gfsmxlCascadeLookupConfig *gfsmxl_cascade_lookup_ensure_config(gfsmxlCascadeLookup *cl, gfsmxlCascadeLookupConfig *cfg_key);

/** Resets state of a ::gfsmxlCascadeLookup, preparing it for another lookup
 *  \li implicitly called by gfsm_cascade_lookup_nbest()
 */
void gfsmxl_cascade_lookup_reset(gfsmxlCascadeLookup *cl);

/** Lookup first n "best" path(s) for input labels \a input in \a cl->csc, and populate \a result.
 *  If \a result is passed as NULL, a new ::gfsmAutomaton will be created.
 *  \returns \a result if non-NULL, otherwise a new ::gfsmAutomaton.
 *  \sa gfsmxl_cascade_lookup_nbest_search_()
 *  \sa gfsmxl_cascade_lookup_nbest_backtrace_()
 */
gfsmAutomaton *gfsmxl_cascade_lookup_nbest(gfsmxlCascadeLookup *cl, gfsmLabelVector *input, gfsmAutomaton *result);

/** Search guts for gfsmxl_cascade_lookup_nbest().  Performs n-best search, storing only backtraces.
 *  \sa gfsmxl_cascade_lookup_nbest()
 *  \sa gfsmxl_cascade_lookup_nbest_backtrace_()
 */
void gfsmxl_cascade_lookup_nbest_search_(gfsmxlCascadeLookup *cl, gfsmLabelVector *input);

/** Backtrace construction guts for gfsmxl_cascade_lookup_nbest().
 *  Expands backtraces for config \a cfg in \a cl into \a result.
 *  \sa gfsmxl_cascade_lookup_nbest()
 *  \sa gfsmxl_cascade_lookup_nbest_search_()
 */
void gfsmxl_cascade_lookup_nbest_backtrace_(gfsmxlCascadeLookup *cl, gfsmxlCascadeLookupConfig *cfg, gfsmAutomaton *result);

//@}

/*======================================================================
 * gfsmxlCascadeLookup API: DEBUG
 */
/// \name gfsmxlCascadeLookup API: DEBUG
//@{

/** Old version of gfsmxl_cascade_lookup_nbest(), useful for debugging
 */
gfsmAutomaton *gfsmxl_cascade_lookup_nbest_debug(gfsmxlCascadeLookup *cl, gfsmLabelVector *input, gfsmAutomaton *result);

//@}


/*======================================================================
 * Low-level: gfsmxlCascadeLookupConfig
 */
/// \name Low-level: gfsmxlCascadeLookupConfig
//@{

/** Heap-element comparison function for ::gfsmxlCascadeLookupConfig (inline)
 *  \li Compares only weights and input positions!
 *  \li Behaves basically like the perl code:
 *      <tt>(lc1->w <=> lc2->w) || (lc2->ipos <=> lc1->ipos)</tt>
 */
GFSM_INLINE
int gfsmxl_cascade_lookup_config_fh_compare(gfsmxlCascadeLookupConfig *lc1, gfsmxlCascadeLookupConfig *lc2);

/** Hash function for ::gfsmxlCascadeLookupConfig; castable to GHashFunc
 *  \li Hashes only <tt>(lc->ipos,lc->oid,lc->qids)</tt>
 */
guint gfsmxl_cascade_lookup_config_ht_hash(gfsmxlCascadeLookupConfig *lc);

/** Equality predicate for ::gfsmxlCascadeLookupConfig; castable to GEqualFunc
 *  \li Tests only <tt>(lcX->ipos,lcX->oid,lcX->qids)</tt>
 */
gboolean gfsmxl_cascade_lookup_config_ht_equal(gfsmxlCascadeLookupConfig *lc1, gfsmxlCascadeLookupConfig *lc2);

/** Allocate a ::gfsmxlCascadeLookupConfig.
 *  \param csc  underlying cascade
 *  \param qids state ids for the config (will be copied into cfg->qids)
 *  \param w    initial accumulated weight for the config
 *  \param bt   configuration backtrace data (parent configuration)
 */
GFSM_INLINE
gfsmxlCascadeLookupConfig *gfsmxl_cascade_lookup_config_new_full(gfsmxlCascade        *csc,
								 gfsmxlCascadeStateId  qids,
								 guint32               ipos,
								 gfsmStateId           oid,
								 gfsmStateId           rid,
								 gfsmWeight            w,
								 gfsmxlCascadeLookupBacktrace bt
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
 * Low-level: gfsmxlCascadeLookupConfigList
 */
/// \name Low-level: gfsmxlCascadeLookupConfigList
//@{

/** Destroy a ::GSList of ::gfsmxlCascadeLookupConfig. Calls gfsmxl_cascade_lookup_config_free_inline()  */
void gfsmxl_cascade_lookup_config_list_free(gfsmxlCascadeLookupConfigList *lcl);

//@}


/*======================================================================
 * END
 */

//-- inline definitions
#ifdef GFSM_INLINE_ENABLED
# include <gfsmxlCascadeLookup.hi>
#endif

#endif /* _GFSMXL_CASCADE_LOOKUP_H */
