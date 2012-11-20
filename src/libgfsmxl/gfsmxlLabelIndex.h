
/*=============================================================================*\
 * File: gfsmxlLabelIndex.h
 * Author: Bryan Jurish <moocow.bovine@gmail.com>
 * Description: finite state machine library: label index
 *
 * Copyright (c) 2007-2011 Bryan Jurish.
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

/** \file gfsmxlLabelIndex.h
 *  \brief Label-indexed automata
 */

#ifndef _GFSMXL_LABEL_INDEX_H
#define _GFSMXL_LABEL_INDEX_H

#include <gfsm.h>

/*======================================================================
 * gfsmxlArcPtrArray
 */
///\name gfsmxlArcPtrArray
//@{

/** \brief Array of first arcs for (state+label) pairs
 *  \detail
 *    \li \a data is (::gfsmArc*) to first outgoing arc per input label.
 *    \li \a len  is number of distinct (source+label) pairs in the indexed automaton.
 */
typedef GPtrArray gfsmxlArcPtrArray;

/** Create and populate a ::gfsmxlArcPtrArray from a ::gfsmxlIndexedAutomaton.
 *  \li xfsm is assumed to be sorted on source states and lower labels.
 */
gfsmxlArcPtrArray *gfsmxl_arc_ptr_array_new_lower(gfsmIndexedAutomaton *xfsm);

/** Free a ::gfsmxlArcPtrArray */
GFSM_INLINE
void gfsmxl_arc_ptr_array_free(gfsmxlArcPtrArray *apa);

//@}

/*======================================================================
 * gfsmxlArcBlockIndex
 */
///\name gfsmxlArcBlockIndex
//@{

/** \brief Source-state index on ::gfsmxlArcPtrArray
 *  \detail
 *   \li \a q2blocks->pdata is (::gfsmArc***) into the ::gfsmxlArcPtrArray \a blocks
 *   \li \a q2blocks->len   is number of states in the automaton, plus 1
 */
typedef struct {
  gfsmxlArcPtrArray *blocks;    /**< Array of (::gfsmArc*) to first arc in each (state+lower) block */
  GPtrArray        *q2block;    /**< Array of (::gfsmArc**) to first block for each source state */
} gfsmxlArcBlockIndex;

/** Create and populate a ::gfsmxlArcBlockIndex from a ::gfsmxlIndexedAutomaton.
 *  \li xfsm is assumed to be sorted on source states and lower labels.
 */
gfsmxlArcBlockIndex *gfsmxl_arc_block_index_new_lower(gfsmIndexedAutomaton *xfsm);

/** Free a ::gfsmxlArcBlockIndex */
GFSM_INLINE
void gfsmxl_arc_block_index_free(gfsmxlArcBlockIndex *abx);

/** Open a ::gfsmArcRange on a ::gfsmxlArcBlockIndex */
GFSM_INLINE
void gfsmxl_arcrange_open_block_index(gfsmArcRange *range, gfsmxlArcBlockIndex *abx, gfsmStateId qid, gfsmLabelId lo);

/** Open a ::gfsmArcRange on a ::gfsmxlArcBlockIndex, using binary search */
GFSM_INLINE
void gfsmxl_arcrange_open_block_index_bsearch(gfsmArcRange *range, gfsmxlArcBlockIndex *abx, gfsmStateId qid, gfsmLabelId lo);

//@}

/*======================================================================
 * gfsmxlArcBlockHash
 */
///\name gfsmxlArcBlockHash
//@{

/// Type for a (state+label) key
typedef struct {
  gfsmStateId  qid;  /**< Source state */
  gfsmLabelId  lab;  /**< (lower) label */
} gfsmxlStateLabelPair;

/** \brief type for an arc-block hash table
 *  \detail keys   are a hashtable-local (::gfsmxlStateLabelPair*)s
 *  \detail values are (::gfsmArc*)s into a ::gfsmArcTable for first outgoing arc from key->qid on (lower) label key->lab
 */
typedef GHashTable gfsmxlArcBlockHash;

/** Hash function for ::gfsmxlStateLabelPair, castable to ::GHashFunc */
guint gfsmxl_state_label_pair_hash(const gfsmxlStateLabelPair *slp);

/** Equality predicate for ::gfsmxlStateLabelPair, castable to ::GEqualFunc */
gboolean gfsmxl_state_label_pair_equal(const gfsmxlStateLabelPair *slp1, const gfsmxlStateLabelPair *slp2);

/** Constructor for ::gfsmxlStateLabelPair */
GFSM_INLINE
gfsmxlStateLabelPair *gfsmxl_state_label_pair_new(gfsmStateId qid, gfsmLabelId lab);

/** Destructor for ::gfsmxlStateLabelPair */
GFSM_INLINE
void gfsmxl_state_label_pair_free(gfsmxlStateLabelPair *slp);

/** Create and populate a new ::gfsmxlArcBlockHash from a ::gfsmxlIndexedAutomaton.
 *  \li xfsm is assumed to be sorted on source states and lower labels
 */
gfsmxlArcBlockHash *gfsmxl_arc_block_hash_new_lower(gfsmIndexedAutomaton *xfsm);

/** Free a ::gfsmxlArcBlockHash */
GFSM_INLINE
void gfsmxl_arc_block_hash_free(gfsmxlArcBlockHash *abh);

//@}

//-- inline definitions
#ifdef GFSM_INLINE_ENABLED
# include <gfsmxlLabelIndex.hi>
#endif

#endif /* _GFSMXL_LABEL_INDEX_H */
