
/*=============================================================================*\
 * File: gfsmxlCascadeLookup.c
 * Author: Bryan Jurish <moocow@ling.uni-potsdam.de>
 * Description: finite state machine library: lookup cascade: lookup
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

#include <gfsmxlCascadeLookup.h>
#include <stdlib.h>
#include <string.h>

//-- no-inline definitions
#ifndef GFSM_INLINE_ENABLED
# include <gfsmxlCascadeLookup.hi>
#endif

/*======================================================================
 * Constructors, etc.
 */

//--------------------------------------------------------------
void gfsmxl_cascade_lookup_reset(gfsmxlCascadeLookup *cl)
{
  //-- clear heap & hash
  while ( gfsmxl_clc_fh_extractmin(cl->heap) ) { ; }
  g_hash_table_remove_all(cl->configs);
  //
  //-- reset per-lookup data
  gfsm_automaton_clear(cl->otrie);
  gfsm_automaton_set_root(cl->otrie, gfsm_automaton_ensure_state(cl->otrie,0));
  cl->n_ops = 0;
}

/*======================================================================
 * Low-level: gfsmxlCascadeLookupConfig
 */

//--------------------------------------------------------------
int gfsmxl_cascade_lookup_config_fh_compare_extern(gfsmxlCascadeLookupConfig *lc1, gfsmxlCascadeLookupConfig *lc2)
{
  //-- compare: weight
  if (gfsm_sr_less(lc1->csc->sr, lc1->w, lc2->w)) return -1;
  if (gfsm_sr_less(lc1->csc->sr, lc2->w, lc1->w)) return  1;
  //
  //-- compare: input position
  if (lc1->ipos < lc2->ipos) return  1;
  if (lc1->ipos > lc2->ipos) return -1;
  //
  //-- default
  return 0;
}

//--------------------------------------------------------------
guint gfsmxl_cascade_lookup_config_ht_hash(gfsmxlCascadeLookupConfig *lc)
{
  guint h,i;
  for (i=0, h=127*lc->ipos+lc->oid ; i < lc->csc->depth; i++) {
    h = 127*h + lc->qids[i];
  }
  return h;
}

//--------------------------------------------------------------
gboolean gfsmxl_cascade_lookup_config_ht_equal(gfsmxlCascadeLookupConfig *lc1, gfsmxlCascadeLookupConfig *lc2)
{
  gfsm_assert(lc1!=NULL && lc2!=NULL && lc1->csc==lc2->csc);
  if (lc1==lc2) return TRUE;
  return (lc1->ipos==lc2->ipos
	  && lc1->oid==lc2->oid
	  && memcmp(lc1->qids, lc2->qids, lc1->csc->depth*sizeof(gfsmStateId))==0
	  );
}

//--------------------------------------------------------------
void gfsmxl_cascade_lookup_config_free(gfsmxlCascadeLookupConfig *lc)
{ gfsmxl_cascade_lookup_config_free_inline(lc); }


/*======================================================================
 * Usage
 */

//--------------------------------------------------------------
gfsmAutomaton *gfsmxl_cascade_lookup_nbest(gfsmxlCascadeLookup *cl, gfsmLabelVector *input, gfsmAutomaton *result)
{
  gfsmxlCascadeLookupConfig *cfg;
  gfsmxlCascade             *csc = cl->csc;
  gfsmSemiring             *sr  = csc->sr;
  gfsmxlCascadeArcIter       cai;
  guint                     n_paths = 0;

  //-- implicit reset
  gfsmxl_cascade_lookup_reset(cl);

  //-- maybe create result automaton
  if (!result) {
    result = gfsm_automaton_new();
    gfsm_automaton_set_semiring(result,sr);
  } else {
    gfsm_automaton_clear(result);
  }
  result->flags.is_transducer = TRUE;
  result->flags.sort_mode     = gfsmASMNone;
  //
  //-- get initial (root) config
  gfsm_automaton_set_root(result, gfsm_automaton_add_state(result));
  cfg = gfsmxl_cascade_lookup_config_new_full(cl->csc,
					     gfsmxl_cascade_get_root(cl->csc),
					     0,
					     gfsm_automaton_get_root(cl->otrie),
					     gfsm_automaton_get_root(result),
					     sr->one);
  gfsmxl_clc_fh_insert(cl->heap, cfg);
  g_hash_table_insert(cl->configs, cfg, NULL);
  //
  //-- ye olde loope
  while ( (cl->n_ops <= cl->max_ops) && (cfg=gfsmxl_clc_fh_extractmin(cl->heap)) ) {
    gpointer old_cfg_key, old_cfg_val;
    gfsmWeight fw, new_w;
    gfsmLabelId lab;

    //-- chalk up another elementary lookup operation
    ++cl->n_ops;

    //------ CHECK FOR FINALITY
    if (cfg->ipos >= input->len) {
      fw = gfsmxl_cascade_get_final_weight(csc, cfg->qids);
      if (gfsm_sr_less(sr,fw,sr->zero)) {
	//-- state is final: get total weight of path
	new_w = gfsm_sr_times(sr,fw,cfg->w);
	if (!gfsm_sr_less(sr,cl->max_w,new_w)) {
	  //-- !(max_w < fw) === (max_w >= fw) === (fw <= max_w) : add this config as a complete path
	  ++n_paths;
	  gfsm_automaton_set_final_state_full(result,cfg->rid,TRUE,fw);
	  if (n_paths >= cl->max_paths) {
	    //-- found enough paths: get outta here...
	    break;
	  }
	}
	//-- bummer: final but too costly: keep on truckin...
      }
    }

    //------ CHECK FOR OUTGOING ARCS
    lab = (cfg->ipos < input->len
	   ? GPOINTER_TO_UINT(g_ptr_array_index(input,cfg->ipos))
	   : gfsmNoLabel);
    gfsmxl_cascade_arciter_open(&cai, csc, cfg->qids, lab);
    for (; gfsmxl_cascade_arciter_ok(&cai); gfsmxl_cascade_arciter_next(&cai)) {
      gfsmxlCascadeArc            *carc = gfsmxl_cascade_arciter_arc(&cai);
      gfsmxlCascadeLookupConfig cfg_tmp = { csc, carc->targets, cfg->ipos, cfg->oid, cfg->rid, cfg->w };

      //-- adjust cfg_tmp.w
      cfg_tmp.w = gfsm_sr_times(sr,cfg->w,carc->weight);

      //-- check whether this arc would exceed max weight
      if (gfsm_sr_less(sr,cl->max_w,cfg_tmp.w)) continue;

      if (carc->lower != gfsmEpsilon) {
	//-- input-epsilon arc: adjust cfg_tmp.ipos
	cfg_tmp.ipos++;
      }
      if (carc->upper != gfsmEpsilon) {
	//-- output-epsilon arc: adjust cfg_tmp.oid
	cfg_tmp.oid = gfsm_trie_get_arc_lower(cl->otrie, cfg->oid, carc->upper, 0,FALSE);
      }

      //-- check whether an equal-or-better new config already exists
      if (g_hash_table_lookup_extended(cl->configs, &cfg_tmp, &old_cfg_key, &old_cfg_val)) {
	//-- an old config exists...
	gfsmxlCascadeLookupConfig *old_cfg = (gfsmxlCascadeLookupConfig*)old_cfg_key;
	if (!gfsm_sr_less(sr, cfg_tmp.w, old_cfg->w)) {
	  //-- old config is better: skip new (cfg_tmp)
	  continue;
	} else {
	  //-- ... new config (cfg_tmp) is better: grab old result-id into new config
	  cfg_tmp.rid = old_cfg->rid;
	}
      } else {
	//-- no old config exists: add a state for this one
	cfg_tmp.rid = gfsm_automaton_add_state(result);
      }

      //-- add arc in output automaton
      gfsm_automaton_add_arc(result, cfg->rid, cfg_tmp.rid, carc->lower, carc->upper, carc->weight);

      //-- add new config to the heap
      gfsmxl_clc_fh_insert(cl->heap, gfsmxl_cascade_lookup_config_clone(&cfg_tmp));
    }
    gfsmxl_cascade_arciter_close(&cai);
  }

  return result;
}

/*======================================================================
 * END
 */
//--------------------------------------------------------------