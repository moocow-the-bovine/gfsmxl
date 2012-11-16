
/*=============================================================================*\
 * File: gfsmxlCascade.c
 * Author: Bryan Jurish <moocow.bovine@gmail.com>
 * Description: finite state machine library: lookup cascade
 *
 * Copyright (c) 2007-2012 Bryan Jurish.
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

#include <gfsmxlCascade.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

//-- no-inline definitions
#ifndef GFSM_INLINE_ENABLED
# include <gfsmxlCascade.hi>
#endif

/*======================================================================
 * Constructors, etc.
 */

//--------------------------------------------------------------
void gfsmxl_cascade_clear_nth(gfsmxlCascade *csc, guint n, gboolean free_automaton)
{
  gfsmIndexedAutomaton *xfsm = (gfsmIndexedAutomaton*)g_ptr_array_index(csc->xfsms,n);

#if   defined(CASCADE_USE_BLOCK_INDEX)
  gfsmxlArcBlockIndex   *xblk = (gfsmxlArcBlockIndex  *)g_ptr_array_index(csc->xblks,n);
#elif defined(CASCADE_USE_BLOCK_HASH)
  gfsmxlArcBlockHash    *xblk = (gfsmxlArcBlockHash   *)g_ptr_array_index(csc->xblks,n);
#endif

#if   defined(CASCADE_USE_SUFFIX_INDEX)
  gfsmxlSuffixLengthIndex *xslx = (gfsmxlSuffixLengthIndex*)g_ptr_array_index(csc->xslxs,n);
#endif

  if (xfsm && free_automaton) gfsm_indexed_automaton_free(xfsm);
  g_ptr_array_index(csc->xfsms,n) = NULL;

#if   defined(CASCADE_USE_BLOCK_INDEX)
  if (xblk) gfsmxl_arc_block_index_free(xblk);
  g_ptr_array_index(csc->xblks,n) = NULL;
#elif defined(CASCADE_USE_BLOCK_HASH)
  if (xblk) gfsmxl_arc_block_hash_free(xblk);
  g_ptr_array_index(csc->xblks,n) = NULL;
#elif defined(CASCADE_USE_BLOCKS)
  g_ptr_array_index(csc->xblks,n) = NULL;
#endif

#if   defined(CASCADE_USE_SUFFIX_INDEX)
  if (xslx) gfsmxl_suffix_length_index_free(xslx);
#endif

}
//--------------------------------------------------------------
void gfsmxl_cascade_clear(gfsmxlCascade *csc, gboolean free_automata)
{
  if (free_automata) {
    guint i;
    for (i=0; i < csc->depth; i++) {
      gfsmxl_cascade_clear_nth(csc,i,free_automata);
    }
  }

  g_ptr_array_set_size(csc->xfsms,0);

#if defined(CASCADE_USE_BLOCKS)
  g_ptr_array_set_size(csc->xblks,0);
#endif

#if   defined(CASCADE_USE_SUFFIX_INDEX)
  g_ptr_array_set_size(csc->xslxs,0);
#endif

  /*
    g_array_set_size(csc->roots,1);
    g_array_index(csc->roots,gfsmStateId,1) = csc->depth;
  */
  csc->depth = 0;
}


/*======================================================================
 * Access: High-level
 */

//--------------------------------------------------------------
void gfsmxl_cascade_sort_all(gfsmxlCascade *csc, gfsmArcCompMask sort_mask)
{
  guint i;
  for (i=0; i < csc->depth; i++) {
    gfsmIndexedAutomaton *xfsm = gfsmxl_cascade_index(csc,i);
    if (!xfsm) continue;
    gfsm_indexed_automaton_sort(xfsm,sort_mask);
#if defined(CASCADE_USE_BLOCK_INDEX) || defined(CASCADE_USE_BLOCK_HASH)
    if (g_ptr_array_index(csc->xblks,i)) {
# if defined(CASCADE_USE_BLOCK_INDEX)
      gfsmxlArcBlockIndex *abi = (gfsmxlArcBlockIndex*)g_ptr_array_index(csc->xblks, i);
      gfsmxl_arc_block_index_free(abi);
      g_ptr_array_index(csc->xblks,i) = gfsmxl_arc_block_index_new_lower(xfsm);
# elif defined(CASCADE_USE_BLOCK_HASH)
      gfsmxlArcBlockHash  *abi = (gfsmxlArcBlockHash*)g_ptr_array_index(csc->xblks, i);
      gfsmxl_arc_block_hash_free(abi);
      g_ptr_array_index(csc->xblks,i) = gfsmxl_arc_block_hash_new_lower(xfsm);
# endif
    }
#endif
  }
}


/*======================================================================
 * Access: Low-level
 */

//--------------------------------------------------------------
gfsmWeight gfsmxl_cascade_get_final_weight(gfsmxlCascade *csc, gfsmxlCascadeStateId qids)
{
  guint i;
  gfsmWeight w=csc->sr->zero;
  if (!csc->depth) return w;
  for (i=0; i < csc->depth; i++) {
    gfsmIndexedAutomaton *xfsm = gfsmxl_cascade_index(csc,i);
    w  = (i
	  ? gfsm_sr_times(csc->sr,w,gfsm_indexed_automaton_get_final_weight(xfsm,qids[i]))
	  : gfsm_indexed_automaton_get_final_weight(xfsm,qids[i]));
    if (!gfsm_sr_less(csc->sr,w,csc->sr->zero)) break;
  }
  return w;
}

//--------------------------------------------------------------
gboolean gfsmxl_cascade_state_is_terminable(gfsmxlCascade *csc, gfsmxlCascadeStateId qids, gfsmxlSuffixLength ilen)
{
#ifndef CASCADE_USE_SUFFIX_INDEX
  return TRUE;
#else
  gfsmxlSuffixLength nxtlen;
  guint i;
  for (i=0; i<csc->depth; i++) {
    gfsmxlSuffixLengthIndex *slx = (gfsmxlSuffixLengthIndex*)csc->xslxs->pdata[i];
    nxtlen = gfsmxl_suffix_length_array_get(slx->lo,qids[i]);
    if (ilen < nxtlen) return FALSE;
    ilen   = gfsmxl_suffix_length_array_get(slx->hi,qids[i]);
  }
  return TRUE;
#endif
}

/*======================================================================
 * Arc Utilities
 */

/*======================================================================
 * Arc Iteration
 */

//--------------------------------------------------------------
static inline
gfsmxlCascadeArc *gfsmxl_cascade_arc_new_from_sourced_arcpp_(gfsmxlCascade        *csc,
							   gfsmxlCascadeStateId  sources,
							   gfsmxlArcPP           arcpp)
{
  int i;
  gfsmxlCascadeArc *carc = gfsmxl_cascade_arc_sized_new(csc->depth);
  carc->lower  = gfsmxl_arcpp_lower(arcpp);
  carc->upper  = gfsmxl_arcpp_upper(arcpp,csc->depth);
  carc->weight = csc->sr->one;
  for (i=0; i < csc->depth; i++) {
    if (arcpp[i]) {
      carc->targets[i] = arcpp[i]->target;
      carc->weight     = gfsm_sr_times(csc->sr,carc->weight,arcpp[i]->weight);
    } else {
      carc->targets[i] = sources[i];
    }
  }
  return carc;
}

//--------------------------------------------------------------
#if defined(CASCADE_USE_RAW_BSEARCH) && !defined(CASCADE_USE_BLOCKS)
static inline
gfsmArc *gfsmxl_arcrange_bsearch(gfsmArc *min, gfsmArc *max, gfsmLabelId lo)
{
  while (min < max) {
    gfsmArc *mid = min + (max-min)/2;
    if (mid->lower < lo)   { min = mid+1; }
    else                   { max = mid; }
  }
  return min;
}
#endif


//--------------------------------------------------------------
#if defined(CASCADE_EXPAND_RECURSIVE)
static
void gfsmxl_cascade_expand_arcs_(gfsmxlCascadeArcIter *cai,
				gfsmxlArcPP           arcpp,
				gint                 ix,
				gfsmLabelId          lo)
{
  //
  //-- check for full match
  if (ix >= cai->csc->depth) {
    gfsmxl_cascade_arciter_insert_arc_(cai,gfsmxl_cascade_arc_new_from_sourced_arcpp_(cai->csc,cai->qids,arcpp));
    return;
  }
  else {
#if defined(CASCADE_USE_BLOCKS)
# if defined(CASCADE_USE_BLOCK_INDEX)
#  if !defined(CASCADE_EXPAND_BLOCK_BSEARCH)
    gfsmArcRange blkrange;
    //
    //-- find matching block (linear search)
    gfsmxl_arcrange_open_block_index(&blkrange,
				    (gfsmxlArcBlockIndex*)g_ptr_array_index(cai->csc->xblks,ix),
				    cai->qids[ix], lo);
    //
    //-- recursively expand matching arcs
    for (arcpp[ix]=blkrange.min; arcpp[ix] < blkrange.max; arcpp[ix]++) {
      if (arcpp[ix]->upper == gfsmEpsilon) {
	//-- upper-epsilon match: skip rest
	gfsmxl_cascade_arciter_insert_arc_(cai,gfsmxl_cascade_arc_new_from_sourced_arcpp_(cai->csc,cai->qids,arcpp));
	continue;
      }
      //-- default: non-eps match: recurse
      gfsmxl_cascade_expand_arcs_(cai, arcpp, ix+1, arcpp[ix]->upper);
    }
#  else /* defined(CASCADE_EXPAND_BLOCK_BSEARCH) */
    gfsmArcRange blkrange;
    //
    //-- find matching block (binary search)
    gfsmxl_arcrange_open_block_index_bsearch(&blkrange,
					    (gfsmxlArcBlockIndex*)g_ptr_array_index(cai->csc->xblks,ix),
					    cai->qids[ix], lo);
    //
    //-- recursively expand matching arcs
    for (arcpp[ix]=blkrange.min; arcpp[ix] < blkrange.max && arcpp[ix]->lower==lo; arcpp[ix]++) {
      if (arcpp[ix]->upper == gfsmEpsilon) {
	//-- upper-epsilon match: skip rest
	gfsmxl_cascade_arciter_insert_arc_(cai,gfsmxl_cascade_arc_new_from_sourced_arcpp_(cai->csc,cai->qids,arcpp));
	continue;
      }
      //-- default: non-eps match: recurse
      gfsmxl_cascade_expand_arcs_(cai, arcpp, ix+1, arcpp[ix]->upper);
    }
#  endif /* defined(CASCADE_EXPAND_BLOCK_BSEARCH) */
# elif defined(CASCADE_USE_BLOCK_HASH)
    gfsmxlStateLabelPair key   = { cai->qids[ix], lo };
    gfsmArcRange       range;
    gfsm_arcrange_open_indexed(&range, gfsmxl_cascade_index(cai->csc,ix), key.qid);
    range.min = (gfsmArc*)g_hash_table_lookup((gfsmxlArcBlockHash*)g_ptr_array_index(cai->csc->xblks,ix), &key);
    //
    //-- recursively expand matching arcs
    for (arcpp[ix]=range.min; arcpp[ix] && arcpp[ix] < range.max && arcpp[ix]->lower==lo; arcpp[ix]++) {
      if (arcpp[ix]->upper == gfsmEpsilon) {
	//-- upper-epsilon match: skip rest
	gfsmxl_cascade_arciter_insert_arc_(cai,gfsmxl_cascade_arc_new_from_sourced_arcpp_(cai->csc,cai->qids,arcpp));
	continue;
      }
      //-- default: non-eps match: recurse
      gfsmxl_cascade_expand_arcs_(cai, arcpp, ix+1, arcpp[ix]->upper);
    }
# endif /* defined(CASCADE_USE_BLOCK_HASH) */
#else /* !defined(CASCADE_USE_BLOCKS) */
# if defined(CASCADE_USE_RAW_BSEARCH) /* && !defined(CASCADE_USE_BLOCKS) */
    gfsmArcRange range;
    gfsm_arcrange_open_indexed(&range, gfsmxl_cascade_index(cai->csc,ix), cai->qids[ix]);
    range.min = gfsmxl_arcrange_bsearch(range.min, range.max, lo);
    //
    //-- recursively expand matching arcs
    for (arcpp[ix]=range.min; arcpp[ix] < range.max && arcpp[ix]->lower==lo; arcpp[ix]++) {
      if (arcpp[ix]->upper == gfsmEpsilon) {
	//-- upper-epsilon match: skip rest
	gfsmxl_cascade_arciter_insert_arc_(cai,gfsmxl_cascade_arc_new_from_sourced_arcpp_(cai->csc,cai->qids,arcpp));
	continue;
      }
      //-- default: non-eps match: recurse
      gfsmxl_cascade_expand_arcs_(cai, arcpp, ix+1, arcpp[ix]->upper);
    }
# else /* !defined(CASCADE_USE_RAW_BSEARCH) && !defined(CASCADE_USE_BLOCKS) */
    gfsmArcRange range;
    gfsm_arcrange_open_indexed(&range, gfsmxl_cascade_index(cai->csc,ix), cai->qids[ix]);
    //
    //-- recursively expand matching arcs
    for (arcpp[ix]=range.min; arcpp[ix] < range.max && arcpp[ix]->lower <  lo; arcpp[ix]++) { ; }
    for (                   ; arcpp[ix] < range.max && arcpp[ix]->lower == lo; arcpp[ix]++) {
      if (arcpp[ix]->upper == gfsmEpsilon) {
	//-- upper-epsilon match: skip rest
	gfsmxl_cascade_arciter_insert_arc_(cai,gfsmxl_cascade_arc_new_from_sourced_arcpp_(cai->csc,cai->qids,arcpp));
	continue;
      }
      //-- default: non-eps match: recurse
      gfsmxl_cascade_expand_arcs_(cai, arcpp, ix+1, arcpp[ix]->upper);
    }
# endif /* defined(CASCADE_USE_RAW_BSEARCH) */
#endif /* !defined(CASCADE_USE_BLOCKS) */
  }
  //
  //-- always reset arcpp[ix]
  arcpp[ix] = NULL;
}

#else /* !defined(CASCADE_EXPAND_RECURSIVE) */

static inline
void gfsmxl_cascade_expand_arcs_(gfsmxlCascadeArcIter *cai,
				 gfsmxlArcPP           arcpp,
				 gint                 ix,
				 gfsmLabelId          lo)
{
  gint                    i = ix;
  gfsmxlCascade         *csc = cai->csc;
  gfsmxlCascadeStateId  qids = cai->qids;
  gfsmArcRange      *ranges = cai->ranges;
  gint                depth = csc->depth;
  GPtrArray          *xblks = csc->xblks;
  gfsmLabelId           lab;
  //
  //-- ye olde loope
  while (i >= ix && i <= depth) {
    if (i==depth) {
      //-- found match: expand & continue
      gfsmxl_cascade_arciter_insert_arc_(cai,gfsmxl_cascade_arc_new_from_sourced_arcpp_(csc,qids,arcpp));
      --i;
      continue;
    }
    lab = (i==ix ? lo : arcpp[i-1]->upper);
    //
    //-- open or increment arc
    if (!arcpp[i]) {
      //-- open arc range
#ifndef CASCADE_EXPAND_BLOCK_BSEARCH
      gfsmxl_arcrange_open_block_index(&ranges[i], (gfsmxlArcBlockIndex*)g_ptr_array_index(xblks,i), qids[i], lab);
#else
      gfsmxl_arcrange_open_block_index_bsearch(&ranges[i], (gfsmxlArcBlockIndex*)g_ptr_array_index(xblks,i), qids[i], lab);
#endif
      arcpp[i] = ranges[i].min;
    } else {
      ++arcpp[i];
    }
    //
    //-- check for arcpp[i] overrun
    if (arcpp[i] >= ranges[i].max || arcpp[i]->lower != lab) {
      //-- arc range overrun: "pop" the stack by resetting arcpp[i]=NULL and decrementing i
      arcpp[i] = NULL;
      --i;
      continue;
    }
    //
    //-- try to expand this match
    if (arcpp[i]->upper == gfsmEpsilon) {
      //-- upper-epsilon match: skip rest of cascade
      gfsmxl_cascade_arciter_insert_arc_(cai,gfsmxl_cascade_arc_new_from_sourced_arcpp_(csc,qids,arcpp));
      continue;
    }
    ++i;
  }
}

#endif /* defined(CASCADE_EXPAND_RECURSIVE) */

//--------------------------------------------------------------
void gfsmxl_cascade_arciter_open(gfsmxlCascadeArcIter *cai, gfsmxlCascade *csc, gfsmxlCascadeStateId qids, gfsmLabelId lo)
{
  guint         i;
  gfsmArc **arcpp = NULL;       //-- computation state tracker
  //
  //-- sanity checks
  gfsm_assert(cai!=NULL);
  gfsm_assert(qids!=NULL);
  //
  //-- initialize arciter object
  cai->csc    = csc;
  cai->qids   = gfsmxl_cascade_stateid_sized_clone(qids, csc->depth);
  cai->arclist = NULL;
#if !defined(CASCADE_EXPAND_RECURSIVE)
  cai->ranges = gfsm_slice_new_n(gfsmArcRange,csc->depth);
#endif
  //
  //-- allocate & initialize temps
  arcpp = gfsmxl_arcpp_sized_new(csc->depth);
  //
  //-- check for (eps:X) arcs on each cascade component csc[i]
  for (i=0; i < csc->depth; i++) {
    gfsmxl_cascade_expand_arcs_(cai,arcpp,i,gfsmEpsilon);
  }
  //
  //-- check for (lo:X) arcs on initial cascade component csc[0]
  if (lo != gfsmEpsilon) gfsmxl_cascade_expand_arcs_(cai,arcpp,0,lo);
  //
  //-- cleanup & return
  gfsmxl_arcpp_free(arcpp,csc->depth);
  cai->cur = cai->arclist;
#if !defined(CASCADE_EXPAND_RECURSIVE)
  gfsm_slice_free_n(gfsmArcRange,cai->ranges,csc->depth);
#endif
  return;
}

//--------------------------------------------------------------
void gfsmxl_cascade_arciter_close(gfsmxlCascadeArcIter *cai)
{
  gfsm_assert(cai!=NULL);
  if (cai->qids) { gfsm_slice_free_n(gfsmStateId,cai->qids,cai->csc->depth); cai->qids=NULL; }
  for (cai->cur=cai->arclist; cai->cur != NULL; cai->cur=cai->cur->next) {
    gfsmxl_cascade_arc_free((gfsmxlCascadeArc*)cai->cur->data, cai->csc->depth);
  }
  g_slist_free(cai->arclist);
  cai->arclist=NULL;
  cai->csc    =NULL;
  //memset(cai,0,sizeof(gfsmxlCascadeArcIter));
}

/*======================================================================
 * Binary I/O: constants
 */

const gchar gfsm_cascade_header_magic[16] = "gfsm_cascade\0";
const gfsmVersionInfo gfsm_cascade_version_bincompat_min_store = { 0,0,10 };
const gfsmVersionInfo gfsm_cascade_version_bincompat_min_check = { 0,0,10 };

/*======================================================================
 * Binary I/O: save
 */

/*--------------------------------------------------------------
 * save_bin_handle()
 */
gboolean gfsmxl_cascade_save_bin_handle(gfsmxlCascade *csc, gfsmIOHandle *ioh, gfsmError **errp)
{
  gfsmxlCascadeHeader hdr;
  int i;
  //
  //-- create & store header
  memset(&hdr, 0, sizeof(gfsmxlCascadeHeader));
  strcpy(hdr.magic, gfsm_cascade_header_magic);
  hdr.version     = gfsm_version;
  hdr.version_min = gfsm_cascade_version_bincompat_min_store;
  hdr.srtype      = csc->sr->type;
  hdr.depth       = csc->depth;
  //
  //-- (maybe) adopt some header info from 0th automaton (for basic debugging with gfsmheader)
  if (csc->depth > 0) {
    gfsmIndexedAutomaton *xfsm = gfsmxl_cascade_index(csc,0);
    hdr._flags     = xfsm->flags;
  }
  if (!gfsmio_write(ioh, &hdr, sizeof(gfsmxlCascadeHeader))) {
    g_set_error(errp, g_quark_from_static_string("gfsmxl"),
		      g_quark_from_static_string("cascade_save_bin:header"),
		      "could not store header");
    return FALSE;
  }
  //
  //-- store automata
  for (i=0; i < csc->depth; i++) {
    gfsmIndexedAutomaton *xfsm = gfsmxl_cascade_index(csc,i);
    if ( !gfsm_indexed_automaton_save_bin_handle(xfsm,ioh,errp) ) {
      if (!(*errp)) {
	g_set_error(errp, g_quark_from_static_string("gfsmxl"),
		    g_quark_from_static_string("cascade_save_bin:xfsm"),
		    "could not store indexed automaton at depth=%d", i);
      }
      return FALSE;
    }
  }
  return TRUE;
}

/*--------------------------------------------------------------
 * save_bin_file()
 */
gboolean gfsmxl_cascade_save_bin_file(gfsmxlCascade *csc, FILE *f, gfsmError **errp)
{
  gfsmIOHandle *ioh = gfsmio_new_file(f);
  gboolean rc = ioh && !(*errp) && gfsmxl_cascade_save_bin_handle(csc, ioh, errp);
  if (ioh) {
    //gfsmio_close(ioh);
    gfsmio_handle_free(ioh);
  }
  return rc;
}

/*--------------------------------------------------------------
 * save_bin_filename_nc()
 */
gboolean gfsmxl_cascade_save_bin_filename_nc(gfsmxlCascade *csc, const gchar *filename, gfsmError **errp)
{
  FILE *f;
  gboolean rc;
  if (!(f=gfsm_open_filename(filename,"wb",errp))) return FALSE;
  rc = gfsmxl_cascade_save_bin_file(csc, f, errp);
  if (f != stdout) fclose(f);
  return rc;
}

/*--------------------------------------------------------------
 * save_bin_filename()
 */
gboolean gfsmxl_cascade_save_bin_filename(gfsmxlCascade *csc, const gchar *filename, int zlevel, gfsmError **errp)
{
  gfsmIOHandle *ioh = gfsmio_new_filename(filename, "wb", zlevel, errp);
  gboolean rc = ioh && !(*errp) && gfsmxl_cascade_save_bin_handle(csc, ioh, errp);
  if (ioh) {
    gfsmio_close(ioh);
    gfsmio_handle_free(ioh);
  }
  return rc;
}

/*--------------------------------------------------------------
 * save_bin_gstring()
 */
gboolean gfsmxl_cascade_save_bin_gstring(gfsmxlCascade *csc, GString *gs, gfsmError **errp)
{
  gfsmPosGString pgs = { gs, gs->len };
  gfsmIOHandle *ioh = gfsmio_new_gstring(&pgs);
  gboolean rc = ioh && !(*errp) && gfsmxl_cascade_save_bin_handle(csc, ioh, errp);
  if (ioh) {
    gfsmio_close(ioh);
    gfsmio_handle_free(ioh);
  }
  return rc;
}


/*======================================================================
 * Binary I/O: load
 */

/*--------------------------------------------------------------
 * load_bin_header()
 */
gboolean gfsmxl_cascade_load_bin_header(gfsmxlCascadeHeader *hdr, gfsmIOHandle *ioh, gfsmError **errp)
{
  if (!gfsmio_read(ioh, hdr, sizeof(gfsmxlCascadeHeader))) {
    g_set_error(errp,
		g_quark_from_static_string("gfsmxl"),
		g_quark_from_static_string("cascade_load_bin_header:size"),
		"could not read header");
    return FALSE;
  }
  else if (strcmp(hdr->magic, gfsm_cascade_header_magic) != 0) {
    g_set_error(errp,
		g_quark_from_static_string("gfsmxl"),
		g_quark_from_static_string("cascade_load_bin_header:magic"),
		"bad magic");
    return FALSE;
  }
  else if (gfsm_version_compare(hdr->version, gfsm_version_bincompat_min_check) < 0) {
    g_set_error(errp,
		g_quark_from_static_string("gfsmxl"),
		g_quark_from_static_string("cascade_load_bin_header:version"),
		"stored format v%u.%u.%u is obsolete - need at least v%u.%u.%u",
		hdr->version.major,
		hdr->version.minor,
		hdr->version.micro,
		gfsm_indexed_version_bincompat_min_check.major,
		gfsm_indexed_version_bincompat_min_check.minor,
		gfsm_indexed_version_bincompat_min_check.micro);
    return FALSE;
  }
  else if (gfsm_version_compare(gfsm_version, hdr->version_min) < 0) {
    g_set_error(errp,
		g_quark_from_static_string("gfsmxl"),
		g_quark_from_static_string("cascade_load_bin_header:version"),
		"libgfsmxl v%u.%u.%u is obsolete - stored cascade needs at least v%u.%u.%u",
		gfsm_version.major,
		gfsm_version.minor,
		gfsm_version.micro,
		hdr->version_min.major,
		hdr->version_min.minor,
		hdr->version_min.micro);
    return FALSE;
  }
  return TRUE;
}

/*--------------------------------------------------------------
 * load_bin_handle_0_0_10()
 */
gboolean gfsmxl_cascade_load_bin_handle_0_0_10(gfsmxlCascadeHeader *hdr,
					      gfsmxlCascade *csc,
					      gfsmIOHandle *ioh,
					      gfsmError **errp)
{
  int i;

  //-- set cascade-global properties
  gfsmxl_cascade_set_semiring_type(csc, hdr->srtype);
  gfsmxl_cascade_set_depth(csc, hdr->depth);

  //-- load automata
  for (i=0; i < csc->depth; i++) {
    gfsmIndexedAutomaton *xfsm = gfsm_indexed_automaton_new();
    g_ptr_array_index(csc->xfsms,i) = xfsm;
    if (!gfsm_indexed_automaton_load_bin_handle(xfsm,ioh,errp)) {
      if (!(*errp)) {
	g_set_error(errp, g_quark_from_static_string("gfsmxl"),
		    g_quark_from_static_string("cascade_load_bin:xfsm"),
		    "could not load indexed automaton at depth=%d", i);
      }
      return FALSE;
    }
    g_array_index(csc->roots,gfsmStateId,i) = xfsm->root_id; //-- restore root

    //-- hack: re-generate arc-block index
#if defined(CASCADE_USE_BLOCK_INDEX)
    if (g_ptr_array_index(csc->xblks,i)) { gfsmxl_arc_block_index_free(g_ptr_array_index(csc->xblks,i)); }
    g_ptr_array_index(csc->xblks,i) = gfsmxl_arc_block_index_new_lower(xfsm);
#elif defined(CASCADE_USE_BLOCK_HASH)
    g_ptr_array_index(csc->xblks,i) = gfsmxl_arc_block_hash_new_lower(xfsm);
#endif

    //-- hack: re-generate suffix-length index
#if defined(CASCADE_USE_SUFFIX_INDEX)
    if (g_ptr_array_index(csc->xslxs,i)) { gfsmxl_suffix_length_index_free(g_ptr_array_index(csc->xslxs,i)); }
    g_ptr_array_index(csc->xslxs,i) = gfsmxl_suffix_length_index_new(xfsm);
#endif
  }

  return TRUE;
}


/*--------------------------------------------------------------
 * load_bin_handle()
 *   + dispatch
 */
gboolean gfsmxl_cascade_load_bin_handle(gfsmxlCascade *csc, gfsmIOHandle *ioh, gfsmError **errp)
{
  gfsmxlCascadeHeader hdr;
  gfsmxl_cascade_clear(csc,TRUE);
  if (!gfsmxl_cascade_load_bin_header(&hdr,ioh,errp)) return FALSE; //-- load header
  return gfsmxl_cascade_load_bin_handle_0_0_10(&hdr,csc,ioh,errp);  //-- guts
}

/*--------------------------------------------------------------
 * load_bin_file()
 */
gboolean gfsmxl_cascade_load_bin_file(gfsmxlCascade *csc, FILE *f, gfsmError **errp)
{
  gfsmIOHandle *ioh = gfsmio_new_zfile(f,"rb",-1);
  gboolean rc = gfsmxl_cascade_load_bin_handle(csc, ioh, errp);
  if (ioh) {
    gfsmio_close(ioh);
    gfsmio_handle_free(ioh);
  }
  return rc;
}

/*--------------------------------------------------------------
 * load_bin_filename()
 */
gboolean gfsmxl_cascade_load_bin_filename(gfsmxlCascade *csc, const gchar *filename, gfsmError **errp)
{
  gfsmIOHandle *ioh = gfsmio_new_filename(filename, "rb", -1, errp);
  gboolean rc = ioh && !(*errp) && gfsmxl_cascade_load_bin_handle(csc, ioh, errp);
  if (ioh) {
    gfsmio_close(ioh);
    gfsmio_handle_free(ioh);
  }
  return rc;
}

/*--------------------------------------------------------------
 * load_bin_gstring()
 */
gboolean gfsmxl_cascade_load_bin_gstring(gfsmxlCascade *csc, GString *gs, gfsmError **errp)
{
  gfsmPosGString pgs = { gs, 0 };
  gfsmIOHandle *ioh = gfsmio_new_gstring(&pgs);
  gboolean rc = ioh && !(*errp) && gfsmxl_cascade_load_bin_handle(csc, ioh, errp);
  if (ioh) {
    gfsmio_close(ioh);
    gfsmio_handle_free(ioh);
  }
  return rc;
}


/*======================================================================
 * END
 */
//--------------------------------------------------------------
