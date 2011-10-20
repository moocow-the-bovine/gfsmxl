
/*=============================================================================*\
 * File: gfsmxlLabelIndex.c
 * Author: Bryan Jurish <moocow.bovine@gmail.com>
 * Description: finite state machine library: label index: extern definitions
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

#include <gfsmxlLabelIndex.h>

//-- no-inline definitions
#ifndef GFSM_INLINE_ENABLED
# include <gfsmxlLabelIndex.hi>
#endif

/*======================================================================
 * gfsmxlArcPtrArray
 */

//--------------------------------------------------------------
gfsmxlArcPtrArray *gfsmxl_arc_ptr_array_new_lower(gfsmIndexedAutomaton *xfsm)
{
  guint n_entries=0;
  gfsmxlArcPtrArray *apa=NULL;
  guint         ai;
  gfsmArcTable *atab = xfsm->arcs->tab;
  gfsmArc      *a=NULL, *prev=NULL;

  //-- get number of entries
  if (atab->len > 0) {
    n_entries=1;
    prev = &g_array_index(atab,gfsmArc,0);
  }
  for (ai=1; ai < atab->len; prev=a,ai++) {
    a = &g_array_index(atab,gfsmArc,ai);
    if (prev->source != a->source || prev->lower != a->lower) { ++n_entries; }
  }

  //-- allocate & populate pointers to arc-blocks
  ++n_entries;
  apa = g_ptr_array_sized_new(n_entries);
  if (atab->len > 0) {
    prev = &g_array_index(atab,gfsmArc,0);
    g_ptr_array_add(apa,prev);
  }
  for (ai=1; ai < atab->len; prev=a,ai++) {
    a = &g_array_index(atab,gfsmArc,ai);
    if (prev->source != a->source || prev->lower != a->lower) { g_ptr_array_add(apa,a); }
  }
  //-- add terminator to n_entries
  g_ptr_array_add(apa, ((gfsmArc*)atab->data)+atab->len);

  return apa;
}

/*======================================================================
 * gfsmxlArcBlockIndex
 */

//--------------------------------------------------------------
gfsmxlArcBlockIndex *gfsmxl_arc_block_index_new_lower(gfsmIndexedAutomaton *xfsm)
{
  gfsmxlArcBlockIndex *abx = g_new0(gfsmxlArcBlockIndex,1);
  gfsmStateId n_states=0, src_prev=gfsmNoState;
  gfsmArc **blocks_pdata = NULL;
  guint bi;

  //-- populate arc blocks
  abx->blocks = gfsmxl_arc_ptr_array_new_lower(xfsm);

  //-- get number of states
  n_states = gfsm_indexed_automaton_n_states(xfsm);
  if (n_states==gfsmNoState) { n_states=0; } //-- sanity check

  //-- populate q2block
  abx->q2block = g_ptr_array_sized_new(n_states+1);
  g_ptr_array_set_size(abx->q2block, n_states+1);
  blocks_pdata = (gfsmArc**)abx->blocks->pdata;
  for (bi=0; bi < abx->blocks->len-1; bi++) {
    gfsmArc *a = blocks_pdata[bi];
    if (a->source != src_prev) {
      for (src_prev++; src_prev <= a->source; src_prev++) {
	g_ptr_array_index(abx->q2block, src_prev) = blocks_pdata+bi;
      }
      src_prev = a->source;
    }
  }
  //-- upper bound (non-inclusive)
  //g_ptr_array_index(abx->q2block, n_states) = &g_ptr_array_index(xfsm->arcs->first, n_states);
  g_ptr_array_index(abx->q2block, n_states) = blocks_pdata + (abx->blocks->len - 1);

  return abx;
}

/*======================================================================
 * gfsmxlArcBlockHash
 */

//--------------------------------------------------------------
guint gfsmxl_state_label_pair_hash(const gfsmxlStateLabelPair *slp)
{ return 127*slp->qid + slp->lab; }

//--------------------------------------------------------------
gboolean gfsmxl_state_label_pair_equal(const gfsmxlStateLabelPair *slp1, const gfsmxlStateLabelPair *slp2)
{ return slp1->qid==slp2->qid && slp1->lab==slp2->lab; }

//--------------------------------------------------------------
gfsmxlArcBlockHash *gfsmxl_arc_block_hash_new_lower(gfsmIndexedAutomaton *xfsm)
{
  gfsmxlArcBlockHash *abh = g_hash_table_new_full((GHashFunc)gfsmxl_state_label_pair_hash,
						 (GEqualFunc)gfsmxl_state_label_pair_equal,
						 (GDestroyNotify)gfsmxl_state_label_pair_free,
						 NULL);
  gfsmxlStateLabelPair slp={gfsmNoState,gfsmNoLabel};
  gfsmArcTable *atab = xfsm->arcs->tab;
  guint ai;

  for (ai=0; ai < atab->len; ai++) {
    gfsmArc *a = &g_array_index(atab,gfsmArc,ai);
    if (a->source != slp.qid || a->lower != slp.lab) {
      gfsmxlStateLabelPair *slp_key = gfsmxl_state_label_pair_new(a->source,a->lower);
      *slp_key = slp;
      g_hash_table_insert(abh,slp_key,a);
    }
  }

  return abh;
}

/*======================================================================
 * END
 */
//--------------------------------------------------------------
