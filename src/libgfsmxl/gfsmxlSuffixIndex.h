
/*=============================================================================*\
 * File: gfsmxlSuffixIndex.h
 * Author: Bryan Jurish <moocow@ling.uni-potsdam.de>
 * Description: finite state machine library: suffix-length indices
 *
 * Copyright (c) 2009 Bryan Jurish.
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

/** \file gfsmxlSuffixIndex.h
 *  \brief Automaton suffix-length indices
 */

#ifndef _GFSMXL_SUFFIX_INDEX_H
#define _GFSMXL_SUFFIX_INDEX_H

#include <gfsm.h>

/*======================================================================
 * gfsmxlSuffixLength
 */
///\name gfsmxlSuffixLength
//@{

/** Suffix length type (unsigned) */
typedef gfsmStateId gfsmxlSuffixLength;

/** constant for missing suffix lengths ((gfsmxlSuffixLength)-1) */
extern const gfsmxlSuffixLength gfsmxlSuffixLengthNone;

//@}

/*======================================================================
 * gfsmxlSuffixLengthArray
 */
///\name gfsmxlSuffixLengthArray
//@{

/** \brief Array of suffix lengths (::gfsmxlSuffixLength), indexed by ::gfsmStateId 
 * \details
 *  Array \a sla such that:
 *  <code>len = g_array_index(sla,gfsmxlSuffixLength,qid)</code>
 *  iff \a len is the minimum number of non-eps symbols on \a side in any
 *  suffix from state \a qid to a final state.
 */
typedef GArray gfsmxlSuffixLengthArray;

/** Create a new ::gfsmxlSuffixLengthIndex, pre-allocating space for \a n_states states.
 *  Initial values are set to ::gfsmxlSuffixLengthNone.
 */
gfsmxlSuffixLengthArray *gfsmxl_suffix_length_array_new(gfsmStateId n_states);

/** Free a ::gfsmxlSuffixLengthArray */
GFSM_INLINE
void gfsmxl_suffix_length_array_free(gfsmxlSuffixLengthArray *sla);

/** ::gfsmxlSuffixLengthArray read/write access (data pointer) */
GFSM_INLINE
gfsmxlSuffixLength *gfsmxl_suffix_length_array_data(gfsmxlSuffixLengthArray *sla);

/** ::gfsmxlSuffixLengthArray read-only access */
GFSM_INLINE
gfsmxlSuffixLength gfsmxl_suffix_length_array_get(gfsmxlSuffixLengthArray *sla, gfsmStateId qid);

/** ::gfsmxlSuffixLengthArray write-only access */
GFSM_INLINE
void gfsmxl_suffix_length_array_set(gfsmxlSuffixLengthArray *sla, gfsmStateId qid, gfsmxlSuffixLength pl);

//@}

/*======================================================================
 * gfsmxlSuffixLengthIndex
 */
///\name gfsmxlSuffixLengthIndex
//@{

/** \brief Suffix-length index on ::gfsmIndexedAutomaton
 *  \detail
 *   \li \a  is (::gfsmArc***) into the ::gfsmxlArcPtrArray \a blocks
 *   \li \a q2blocks->len   is number of states in the automaton, plus 1
 */
typedef struct {
  gfsmxlSuffixLengthArray *lo;   /**< Minimum suffix length index for lower side */
  gfsmxlSuffixLengthArray *hi;   /**< Minimum suffix length index for upper side */
} gfsmxlSuffixLengthIndex;


/** Create and populate a ::gfsmxlSuffixLengthIndex from a ::gfsmxlIndexedAutomaton */
gfsmxlSuffixLengthIndex *gfsmxl_suffix_length_index_new(gfsmIndexedAutomaton *xfsm);

/** Free a ::gfsmxlSuffixLengthIndex */
GFSM_INLINE
void gfsmxl_suffix_length_index_free(gfsmxlSuffixLengthIndex *slx);

/** Write the contents of a ::gfsmSuffixLengthIndex to a (binary) ::gfsmIOHandle.
 *  \param slx index to write
 *  \param ioh handle to which data is to be written
 *  \param errp if an error occurs, \a *errp will hold an error message
 *  \returns true on success
 */
gboolean gfsmxl_suffix_length_index_write_bin_handle(gfsmxlSuffixLengthIndex *slx, gfsmIOHandle *ioh, gfsmError **errp);
/** Read the contents of a ::gfsmSuffixLengthIndex from a (binary) ::gfsmIOHandle.
 *  \param slx index into which data is to be read
 *  \param ioh handle from which data is to be read
 *  \param errp if an error occurs, \a *errp will hold an error message
 *  \returns true on success
 */
gboolean gfsmxl_suffix_length_index_read_bin_handle(gfsmxlSuffixLengthIndex *slx, gfsmIOHandle *ioh, gfsmError **errp);

//@}

//-- inline definitions
#ifdef GFSM_INLINE_ENABLED
# include <gfsmxlSuffixIndex.hi>
#endif

#endif /* _GFSMXL_SUFFIX_INDEX_H */
