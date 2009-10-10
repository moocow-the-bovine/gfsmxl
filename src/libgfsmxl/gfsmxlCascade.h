
/*=============================================================================*\
 * File: gfsmxlCascade.h
 * Author: Bryan Jurish <moocow@ling.uni-potsdam.de>
 * Description: finite state machine library: lookup cascade
 *
 * Copyright (c) 2007-2009 Bryan Jurish.
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

/** \file gfsmxlCascade.h
 *  \brief First stab at transducer cascade
 */

#ifndef _GFSMXL_CASCADE_H
#define _GFSMXL_CASCADE_H

#include <gfsm.h>
#include <gfsmxlLabelIndex.h>
#include <gfsmxlSuffixIndex.h>

/*======================================================================
 * Types
 */

/** \brief Type for a raw cascade state id (array of \a csc->depth ::gfsmStateId, in cascade-order) */
typedef gfsmStateId* gfsmxlCascadeStateId;

//#define CASCADE_USE_RAW_BSEARCH 1
#define CASCADE_USE_BLOCK_INDEX 1
//#define CASCADE_EXPAND_BLOCK_BSEARCH 1
//#define CASCADE_USE_BLOCK_HASH 1
//#define CASCADE_USE_SUFFIX_INDEX 1

//-- expand arcs recursively?
//#define CASCADE_EXPAND_RECURSIVE 1

//-- sort temporary arc-lists on creation?
//   : this is pointless unless we're storing arc-lists with configs!
//#define CASCADE_SORT_ARCITER 1

#if !defined(CASCADE_EXPAND_RECURSIVE) && !defined(CASCADE_USE_BLOCK_INDEX)
# define CASCADE_USE_BLOCK_INDEX 1
#endif

#if defined(CASCADE_USE_BLOCK_INDEX) || defined(CASCADE_USE_BLOCK_HASH)
# define CASCADE_USE_BLOCKS 1
#else
# undef CASCADE_USE_BLOCKS
#endif

/// Type for an automaton cascade
typedef struct {
  guint                   depth;  /**< Number of automata in the cascade */
  gfsmSemiring           *sr;     /**< Common semiring */
  GPtrArray              *xfsms;  /**< Cascaded automata (indexed) */
#if   defined(CASCADE_USE_BLOCK_INDEX)
  GPtrArray              *xblks;  /**< Arc-block indices (::gfsmxlArcBlockIndex*) over \a xfsms */
#elif defined(CASCADE_USE_BLOCK_HASH)
  GPtrArray              *xblks;  /**< Arc-block hash indices (::gfsmxlArcBlockHash*) over \a xfsms */
#endif
#if   defined(CASCADE_USE_SUFFIX_INDEX)
  GPtrArray              *xslxs;   /**< Suffix-length indices (::gfsmxlSuffixLengthIndex*) over \a xfsms */
#endif
  GArray                  *roots;  /**< Array of ::gfsmStateId (root ids of corresponding \a xfsms) */
} gfsmxlCascade;

/*======================================================================
 * Constructors, etc.
 */
/// \name Constructors etc.
//@{

/** Create a new ::gfsmxlCascade */
GFSM_INLINE
gfsmxlCascade *gfsmxl_cascade_new_full(guint depth, gfsmSRType srtype);

/** Create a new indexed automaton, using some default values */
GFSM_INLINE
gfsmxlCascade *gfsmxl_cascade_new(void);

/** Clear a ::gfsmxlCascade */
void gfsmxl_cascade_clear(gfsmxlCascade *csc, gboolean free_automata);

/** Free a ::gfsmxlCascade */
GFSM_INLINE
void gfsmxl_cascade_free(gfsmxlCascade *csc, gboolean free_automata);

/** Append an automaton to a cascade.  Steals \a xfsm */
GFSM_INLINE
void gfsmxl_cascade_append_indexed(gfsmxlCascade *csc, gfsmIndexedAutomaton *xfsm);

//@}

/*======================================================================
 * High-level Access
 */
///\name High-level Access
//@{

/** Macro to get a pointer to the ::gfsmIndexedAutomaton at depth \a index_ in \a csc_.
 *  \a csc_ must have depth at least \a index_+1 for this to work
 */
#define gfsmxl_cascade_index(csc_,index_) \
  ((gfsmIndexedAutomaton*)g_ptr_array_index((csc_)->xfsms,(index_)))

/** Set semiring type */
GFSM_INLINE
void gfsmxl_cascade_set_semiring_type(gfsmxlCascade *csc, gfsmSRType srtype);

/** Set semiring (copy) */
GFSM_INLINE
void gfsmxl_cascade_set_semiring(gfsmxlCascade *csc, gfsmSemiring *sr);

/** Sort arcs of all sub-automata */
void gfsmxl_cascade_sort_all(gfsmxlCascade *csc, gfsmArcCompMask sort_mask);

//@}

/*======================================================================
 * State Utilities
 */
///\name State Utilities
//@{

/** Get root-id of cascade (never free this yourself!) */
GFSM_INLINE
gfsmxlCascadeStateId gfsmxl_cascade_get_root(gfsmxlCascade *csc);

/** Allocate a new ::gfsmxlCascadeStateId suitable for a cascade of \a depth */
GFSM_INLINE
gfsmxlCascadeStateId gfsmxl_cascade_stateid_sized_new(guint depth);

/** Allocate a new ::gfsmxlCascadeStateId suitable for \a csc */
GFSM_INLINE
gfsmxlCascadeStateId gfsmxl_cascade_stateid_new(gfsmxlCascade *csc);

/** Copy a ::gfsmxlCascadeStateId \a src to \a dst.  \returns dst */
GFSM_INLINE
gfsmxlCascadeStateId gfsmxl_cascade_stateid_sized_copy(gfsmxlCascadeStateId dst, gfsmxlCascadeStateId src, guint depth);

/** Clone a ::gfsmxlCascadeStateId of depth \a depth */
GFSM_INLINE
gfsmxlCascadeStateId gfsmxl_cascade_stateid_sized_clone(gfsmxlCascadeStateId src, guint depth);

/** Free a ::gfsmxlCascadeStateId as returned by gfsmxl_cascade_stateid_new() */
GFSM_INLINE
void gfsmxl_cascade_stateid_free(gfsmxlCascadeStateId qids);

//@}

/*======================================================================
 * Arc Utilities
 */
///\name Arc Utilities: gfsmxlArcPP
//@{

/** \brief Type for temporary cascade arcs (array of \a csc->depth ::gfsmArc*)
 *  \detail a NULL value indicates a pure-epsilon move
 */
typedef gfsmArc** gfsmxlArcPP;

/** Allocate a new ::gfsmxlArcPP suitable for a cascade of \a depth */
GFSM_INLINE
gfsmxlArcPP gfsmxl_arcpp_sized_new(guint depth);

/** Copy a ::gfsmxlArcPP \a src to \a dst.  \returns dst */
GFSM_INLINE
gfsmxlArcPP gfsmxl_arcpp_sized_copy(gfsmxlArcPP dst, gfsmxlArcPP src, guint depth);

/** Clone a ::gfsmxlArcPP of depth \a depth */
GFSM_INLINE
gfsmxlArcPP gfsmxl_arcpp_sized_clone(gfsmxlArcPP src, guint depth);

/** Free a ::gfsmxlArcPP as returned by gfsmxl_arcpp_sized_new() */
GFSM_INLINE
void gfsmxl_arcpp_free(gfsmxlArcPP arcpp);

/** Get ultimate lower label of a ::gfsmxlArcPP */
GFSM_INLINE
gfsmLabelId gfsmxl_arcpp_lower(gfsmxlArcPP arcpp);

/** Get ultimate upper label of a ::gfsmxlArcPP */
GFSM_INLINE
gfsmLabelId gfsmxl_arcpp_upper(gfsmxlArcPP arcpp, guint depth);

/** Get total weight of a ::gfsmxlArcPP */
GFSM_INLINE
gfsmWeight gfsmxl_arcpp_weight(gfsmxlArcPP arcpp, guint depth, gfsmSemiring *sr);

/** Copy target state of a ::gfsmxlArcPP to \a dst, assuming start state was \a src */
GFSM_INLINE
void gfsmxl_arcpp_target(gfsmxlArcPP arcpp, gfsmxlCascadeStateId src, gfsmxlCascadeStateId dst, guint depth);

//@}

/*======================================================================
 * Arc Utilities
 */
///\name Arc Utilities: gfsmxlCascadeArc
//@{

/** \brief Type for cascade arcs */
typedef struct {
  gfsmLabelId         lower;   /**< Ultimate lower label */
  gfsmLabelId         upper;   /**< Ultimate upper label */
  gfsmWeight          weight;  /**< Ultimate weight */
  gfsmxlCascadeStateId targets; /**< Target state (local copy) */
} gfsmxlCascadeArc;

/** Allocate a new ::gfsmxlCascadeArc suitable for a cascade \a csc */
GFSM_INLINE
gfsmxlCascadeArc* gfsmxl_cascade_arc_sized_new(guint depth);

/** Copy a ::gfsmxlCascadeArc \a src for \a csc to \a dst. \returns \a dst */
GFSM_INLINE
gfsmxlCascadeArc* gfsmxl_cascade_arc_sized_copy(gfsmxlCascadeArc *dst, gfsmxlCascadeArc *src, guint depth);

/** Clone a ::gfsmxlCascadeArc of depth \a depth */
GFSM_INLINE
gfsmxlCascadeArc* gfsmxl_cascade_arc_sized_clone(gfsmxlCascadeArc *src, guint depth);

/** Free a ::gfsmxlCascadeArc as returned by gfsmxl_cascade_arc_sized_new() */
GFSM_INLINE
void gfsmxl_cascade_arc_free(gfsmxlCascadeArc *arc);

//@}


/*======================================================================
 * Arc Iteration & Low-level Access
 */
///\name Arc Iteration & Low-level Access
//@{

/** Get state final weight or \a csc->sr->zero if state is non-final */
gfsmWeight gfsmxl_cascade_get_final_weight(gfsmxlCascade *csc, gfsmxlCascadeStateId qids);

/** Check whether a ::gfsmxlCascadeStateId is terminable
 *  Only useful if GFSMXL_USE_SUFFIX_INDEX is defined
 */
gboolean gfsmxl_cascade_state_is_terminable(gfsmxlCascade *csc, gfsmxlCascadeStateId qids, gfsmxlSuffixLength ilen);

/** \brief Type for cascade arc iteration ?! */
typedef struct {
  gfsmxlCascade         *csc;     /**< underlying cascade */
  gfsmxlCascadeStateId   qids;    /**< source state (vector of ::gfsmStateId, +depth): local copy */
  GSList               *arclist; /**< GSList* of ::gfsmxlCascadeArc* */
  GSList               *cur;     /**< Current node in arclist */
#if !defined(CASCADE_EXPAND_RECURSIVE)
  gfsmArcRange         *ranges;  /**< block range stack */
#endif
} gfsmxlCascadeArcIter;

/** Open outgoing arcs from state \a qids in \a csc with ultimate lower label eithr epsilon or \a lo.
 *  \li Clones \a qids into \a arcs->qids.
 *  \li Allocates some temporaries, including any and all arcs found
 */
void gfsmxl_cascade_arciter_open(gfsmxlCascadeArcIter *cai, gfsmxlCascade *csc, gfsmxlCascadeStateId qids, gfsmLabelId lo); 

/** Close a cascade arc iterator \a cai, freeing any data associated with it */
void gfsmxl_cascade_arciter_close(gfsmxlCascadeArcIter *cai);

/** Check whether there are arcs remaining in a ::gfsmxlCascadeArcIter */
GFSM_INLINE
gboolean gfsmxl_cascade_arciter_ok(gfsmxlCascadeArcIter *cai);

/** Get current arc from a ::gfsmxlCascadeArcIter.
 *  \li Assumes that \a cai is valid.
 *  \li Returned arc will be freed when \a cai is closed
 */
GFSM_INLINE
gfsmxlCascadeArc *gfsmxl_cascade_arciter_arc(gfsmxlCascadeArcIter *cai);

/** Increment an open ::gfsmxlCascadeArcIter.  Assumes that \a cai is valid. */
GFSM_INLINE
void gfsmxl_cascade_arciter_next(gfsmxlCascadeArcIter *cai);

/** Reset an open ::gfsmxlCascadeArcIter to the first arc for which it was opened. */
GFSM_INLINE
void gfsmxl_cascade_arciter_reset(gfsmxlCascadeArcIter *cai);

#ifdef CASCADE_SORT_ARCITER
/** Low-level utility: insert an arc into a \cai->arclist, sorted by weight */
GFSM_INLINE
void gfsmxl_cascade_arciter_insert_arc_(gfsmxlCascadeArcIter *cai, gfsmxlCascadeArc *carc);
#else
/** Low-level utility: insert an arc into a \cai->arclist, unsorted */
GFSM_INLINE
void gfsmxl_cascade_arciter_insert_arc_(gfsmxlCascadeArcIter *cai, gfsmxlCascadeArc *carc);
#endif


//@}

/*======================================================================
 * I/O
 */
///\name Binary I/O
//@{

/// Header info for binary cascade files (::gfsmAutomatonHeader pseudo-compatible)
typedef struct {
  gchar              magic[16];    /**< magic header string "gfsm_cascade" */
  gfsmVersionInfo    version;      /**< gfsm version which created the stored file */
  gfsmVersionInfo    version_min;  /**< minimum gfsm version required to load the file */
  gfsmAutomatonFlags _flags;       /**< [unused] automaton flags (set to flags of 0th xfsm, if available) */
  gfsmStateId        _root_id;     /**< [unused] Id of root node */
  gfsmStateId        _n_states;    /**< [unused] number of stored states */
  gfsmStateId        _n_arcs;      /**< [unused] number of stored arcs */
  guint32            srtype;       /**< semiring type (cast to ::gfsmSRType) */
  guint32            _unused1;     /**< [unused] reserved */
  guint32            depth;        /**< cascade depth */
  guint32            _unused3;     /**< [unused] reserved */
} gfsmxlCascadeHeader;

/** Magic header string for stored ::gfsmxlCascade files */
extern const gchar gfsm_cascade_header_magic[16];

/** Minimum libgfsm version required for loading files stored by this version of libgfsm */
extern const gfsmVersionInfo gfsm_cascade_version_bincompat_min_store;

/** Minimum libgfsm version whose binary files this version of libgfsm can read */
extern const gfsmVersionInfo gfsm_cascade_version_bincompat_min_check;

/*--------------------------------------------------------------*/

/** Load an automaton header from a stored binary file.
 *  Returns TRUE iff the header looks valid. */
gboolean gfsmxl_cascade_load_header(gfsmxlCascadeHeader *hdr, gfsmIOHandle *ioh, gfsmError **errp);

/** Load an automaton from a named binary file (implicitly clear()s \a fsm) */
gboolean gfsmxl_cascade_load_bin_handle(gfsmxlCascade *csc, gfsmIOHandle *ioh, gfsmError **errp);

/** Load an automaton from a stored binary file (implicitly clear()s \a fsm) */
gboolean gfsmxl_cascade_load_bin_file(gfsmxlCascade *csc, FILE *f, gfsmError **errp);

/** Load an automaton from a named binary file (implicitly clear()s \a fsm) */
gboolean gfsmxl_cascade_load_bin_filename(gfsmxlCascade *csc, const gchar *filename, gfsmError **errp);

/** Load an automaton from an in-memory buffer */
gboolean gfsmxl_cascade_load_bin_gstring(gfsmxlCascade *csc, GString *gs, gfsmError **errp);

/*--------------------------------------------------------------*/

/** Store an automaton in binary form to a gfsmIOHandle* */
gboolean gfsmxl_cascade_save_bin_handle(gfsmxlCascade *csc, gfsmIOHandle *ioh, gfsmError **errp);

/** Store an automaton in binary form to a file */
gboolean gfsmxl_cascade_save_bin_file(gfsmxlCascade *csc, FILE *f, gfsmError **errp);

/** Store an automaton to a named binary file (no compression) */
gboolean gfsmxl_cascade_save_bin_filename_nc(gfsmxlCascade *csc, const gchar *filename, gfsmError **errp);

/** Store an automaton to a named binary file, possibly compressing.
 *  Set \a zlevel=-1 for default compression, and
 *  set \a zlevel=0  for no compression, otherwise should be as for zlib (1 <= zlevel <= 9)
 */
gboolean gfsmxl_cascade_save_bin_filename(gfsmxlCascade *csc, const gchar *filename, int zlevel, gfsmError **errp);

/** Append an uncompressed binary automaton to an in-memory buffer */
gboolean gfsmxl_cascade_save_bin_gstring(gfsmxlCascade *csc, GString *gs, gfsmError **errp);

//@}

/*======================================================================
 * END
 */

//-- inline definitions
#ifdef GFSM_INLINE_ENABLED
# include <gfsmxlCascade.hi>
#endif

#endif /* _GFSMXL_CASCADE_H */
