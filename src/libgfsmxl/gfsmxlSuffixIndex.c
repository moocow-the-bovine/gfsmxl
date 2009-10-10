
/*=============================================================================*\
 * File: gfsmxlSuffixIndex.c
 * Author: Bryan Jurish <moocow@ling.uni-potsdam.de>
 * Description: finite state machine library: label index: extern definitions
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

#include <gfsmxlSuffixIndex.h>

//-- no-inline definitions
#ifndef GFSM_INLINE_ENABLED
# include <gfsmxlSuffixIndex.hi>
#endif

/*======================================================================
 * gfsmxlSuffixLength
 */
const gfsmxlSuffixLength gfsmxlSuffixLengthNone = (guint32)-1;

/*======================================================================
 * gfsmxlSuffixLengthArray
 */

//--------------------------------------------------------------
gfsmxlSuffixLengthArray *gfsmxl_suffix_length_array_new(gfsmStateId n_states)
{
  gfsmxlSuffixLengthArray *slx = g_array_sized_new(FALSE,FALSE,sizeof(gfsmxlSuffixLength),n_states);
  gfsmStateId qi;
  
  //-- initialize all lengths to gfsmxlSuffixLengthNone
  g_array_set_size(slx,n_states);
  for (qi=0; qi<n_states; qi++) {
    g_array_index(slx,gfsmxlSuffixLength,qi) = gfsmxlSuffixLengthNone;
  }

  return slx;
}


/*======================================================================
 * gfsmxlSuffixLengthIndex
 */


//--------------------------------------------------------------
void gfsmxl_suffix_length_index_visit(gfsmxlSuffixLengthIndex *slx,
				      gfsmStateId              qid,
				      gfsmxlSuffixLength       len_lo,
				      gfsmxlSuffixLength       len_hi,
				      GPtrArray               *rarcs_first)

{
  gfsmxlSuffixLength oldlen_lo = g_array_index(slx->lo,gfsmxlSuffixLength,qid);
  gfsmxlSuffixLength oldlen_hi = g_array_index(slx->hi,gfsmxlSuffixLength,qid);
  gfsmArc **app;

  //-- check whether we've already found an equal or better path
  if (len_lo >= oldlen_lo && len_hi >= oldlen_hi) return;

  //-- sanitize lengths
  if (len_lo <  oldlen_lo) { g_array_index(slx->lo,gfsmxlSuffixLength,qid) = len_lo; }
  else { len_lo = oldlen_lo; }

  if (len_hi <  oldlen_hi) { g_array_index(slx->hi,gfsmxlSuffixLength,qid) = len_hi; }
  else { len_hi = oldlen_hi; }

  //-- recurse on incoming arcs
  for (app=(gfsmArc**)g_ptr_array_index(rarcs_first,qid); app && (*app) && (*app)->target==qid; app++) {
    gfsmxlSuffixLength alen_lo = ((*app)->lower==gfsmEpsilon ? 0 : 1);
    gfsmxlSuffixLength alen_hi = ((*app)->upper==gfsmEpsilon ? 0 : 1);
    gfsmxl_suffix_length_index_visit(slx, (*app)->source,  len_lo+alen_lo, len_hi+alen_hi, rarcs_first);
  }
}

//--------------------------------------------------------------
gint gfsmxl_suffix_array_rarcs_compare_func(const gfsmArc **a1, const gfsmArc **a2)
{
  if      (a1==NULL  && a2!=NULL)   return -1;
  else if (a1!=NULL  && a2==NULL)   return  1;
  else if ((*a1)->target < (*a2)->target) return -1;
  else if ((*a1)->target > (*a2)->target) return  1;
  else if ((*a1)->source < (*a2)->source) return -1;
  else if ((*a1)->source > (*a2)->source) return  1;
  return 0;
}

//--------------------------------------------------------------
gfsmxlSuffixLengthIndex *gfsmxl_suffix_length_index_new(gfsmIndexedAutomaton *xfsm)
{
  gfsmStateId nq = gfsm_indexed_automaton_n_states(xfsm);
  guint       na = gfsm_indexed_automaton_n_arcs(xfsm);
  gfsmxlSuffixLengthIndex *slx = g_new0(gfsmxlSuffixLengthIndex,1);
  GPtrArray *rarcs       = g_ptr_array_sized_new(na);   //-- [ai] -> arc (sorted by "ts...")
  GPtrArray *rarcs_first = g_ptr_array_sized_new(nq);   //-- [qi] -> first &&arc to qi in rarcs
  gfsmArcTable *atab     = xfsm->arcs->tab;
  gfsmStateId qid;
  guint        ai;
  
  //-- initialize: index sub-arrays
  gfsm_assert(slx!=NULL);
  slx->lo = gfsmxl_suffix_length_array_new(nq);
  slx->hi = gfsmxl_suffix_length_array_new(nq);

  //-- initialize: reverse arc-index: rarcs
  for (ai=0; ai<na; ai++) {
    gfsmArc *ap = &(g_array_index(atab,gfsmArc,ai));
    g_ptr_array_add(rarcs, ap);
  }
  g_ptr_array_sort(rarcs, (GCompareFunc)gfsmxl_suffix_array_rarcs_compare_func);

  //-- initialize: reverse arc-index: rarcs_first
  for (ai=0; ai<na; ai++) {
    gfsmArc **app = (gfsmArc**)&(g_ptr_array_index(rarcs,ai));
    gfsmStateId target = (*app)->target;
    if (target < rarcs_first->len) continue;
    g_ptr_array_set_size(rarcs_first,target+1);
    g_ptr_array_index(rarcs_first,target) = app;
  }
  g_ptr_array_set_size(rarcs_first,nq);

  //-- guts
  for (qid=0; qid<nq; qid++) {
    if (!gfsm_indexed_automaton_state_is_final(xfsm,qid)) continue;
    gfsmxl_suffix_length_index_visit(slx,qid, 0,0, rarcs_first);
  }

  //-- cleanup: reverse arc-index
  g_ptr_array_free(rarcs,TRUE);
  g_ptr_array_free(rarcs_first,TRUE);

  //-- return
  return slx;
}

//--------------------------------------------------------------
gboolean gfsmxl_suffix_length_index_write_bin_handle(gfsmxlSuffixLengthIndex *slx, gfsmIOHandle *ioh, gfsmError **errp)
{
  gfsmStateId nq = slx->lo->len;
  if (!gfsmio_write(ioh, &nq, sizeof(nq))) {
    g_set_error(errp,
		g_quark_from_static_string("gfsmxl"),
		g_quark_from_static_string("suffix_length_index_write_bin_handle:size"),
		"could not write suffix-length index size");
    return FALSE;
  }
  if (!gfsmio_write(ioh, slx->lo->data, nq*sizeof(gfsmxlSuffixLength))) {
    g_set_error(errp,
		g_quark_from_static_string("gfsmxl"),
		g_quark_from_static_string("suffix_length_index_write_bin_handle:lo"),
		"could not write suffix-length index lower-side data");
    return FALSE;
  }
  if (!gfsmio_write(ioh, slx->hi->data, nq*sizeof(gfsmxlSuffixLength))) {
    g_set_error(errp,
		g_quark_from_static_string("gfsmxl"),
		g_quark_from_static_string("suffix_length_index_write_bin_handle:hi"),
		"could not write suffix-length index upper-side data");
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
gboolean gfsmxl_suffix_length_index_read_bin_handle(gfsmxlSuffixLengthIndex *slx, gfsmIOHandle *ioh, gfsmError **errp)
{
  gfsmStateId nq, qi;

  if (!gfsmio_read(ioh, &nq, sizeof(nq))) {
    g_set_error(errp,
		g_quark_from_static_string("gfsmxl"),
		g_quark_from_static_string("suffix_length_index_read_bin_handle:size"),
		"could not read suffix-length index size");
    return FALSE;
  }

  g_array_set_size(slx->lo,nq);
  g_array_set_size(slx->hi,nq);
  for (qi=0; qi<nq; qi++) {
    g_array_index(slx->lo,gfsmxlSuffixLength,qi) = gfsmxlSuffixLengthNone;
    g_array_index(slx->hi,gfsmxlSuffixLength,qi) = gfsmxlSuffixLengthNone;
  }

  if (!gfsmio_read(ioh, &slx->lo->data, nq*sizeof(gfsmxlSuffixLength))) {
    g_set_error(errp,
		g_quark_from_static_string("gfsmxl"),
		g_quark_from_static_string("suffix_length_index_read_bin_handle:lo"),
		"could not read suffix-length index lower-side data");
    return FALSE;
  }

  if (!gfsmio_read(ioh, &slx->hi->data, nq*sizeof(gfsmxlSuffixLength))) {
    g_set_error(errp,
		g_quark_from_static_string("gfsmxl"),
		g_quark_from_static_string("suffix_length_index_read_bin_handle:hi"),
		"could not read suffix-length index upper-side data");
    return FALSE;
  }

  return TRUE;
}

/*======================================================================
 * END
 */
//--------------------------------------------------------------
