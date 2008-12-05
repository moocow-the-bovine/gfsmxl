
/*=============================================================================*\
 * File: gfsmxlCascadeLookup.c
 * Author: Bryan Jurish <moocow@ling.uni-potsdam.de>
 * Description: finite state machine library: lookup cascade: lookup
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

#include <gfsmxlCascadeLookup.h>
#include <stdlib.h>
#include <string.h>

//-- no-inline definitions
#ifndef GFSM_INLINE_ENABLED
# include <gfsmxlCascadeLookup.hi>
#endif

/*======================================================================
 * Constants
 */
const gfsmxlCascadeLookupBacktrace gfsmxl_bt_null = { NULL, 0,0,0,0 };

/*======================================================================
 * Constructors, etc.
 */

//--------------------------------------------------------------
void gfsmxl_cascade_lookup_reset(gfsmxlCascadeLookup *cl)
{
  //-- clear heap, hash, finals
  while ( gfsmxl_clc_fh_extractmin(cl->heap) ) { ; }
  g_hash_table_remove_all(cl->configs);
  g_slist_free(cl->finals);
  cl->finals = NULL;
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
 * Low-level: gfsmxlCascadeLookupConfigList
 */

//--------------------------------------------------------------
void gfsmxl_cascade_lookup_config_list_free(gfsmxlCascadeLookupConfigList *lcl)
{
  gfsmxlCascadeLookupConfigList *l = lcl;
  while (l != NULL) {
    gfsmxl_cascade_lookup_config_free_inline((gfsmxlCascadeLookupConfig*)l->data);
    l = l->next;
  }
  g_slist_free(lcl);
}


/*======================================================================
 * gfsmxlCascadeLookup API
 */

//--------------------------------------------------------------
gfsmAutomaton *gfsmxl_cascade_lookup_nbest(gfsmxlCascadeLookup *cl, gfsmLabelVector *input, gfsmAutomaton *result)
{
  GSList *finals;

  //-- maybe create result automaton
  if (!result) {
    result = gfsm_automaton_new();
    gfsm_automaton_set_semiring(result,cl->csc->sr);
  } else {
    gfsm_automaton_clear(result);
  }
  result->flags.is_transducer = TRUE;
  result->flags.sort_mode     = gfsmASMNone;

  //-- search guts (generate backtrace)
  gfsmxl_cascade_lookup_nbest_search_(cl, input);

  //-- generate backtrace
  for (finals=cl->finals; finals != NULL; finals=finals->next) {
    gfsmxlCascadeLookupConfig *cfg = (gfsmxlCascadeLookupConfig *)finals->data;
    gfsmxl_cascade_lookup_nbest_backtrace_(cl,cfg,result);
  }

  return result;
}

//--------------------------------------------------------------
//#define DEBUG_NCONFIGS 1
void gfsmxl_cascade_lookup_nbest_search_(gfsmxlCascadeLookup *cl, gfsmLabelVector *input)
{
  gfsmxlCascadeLookupConfig *cfg;
  gfsmxlCascade             *csc = cl->csc;
  gfsmSemiring              *sr = csc->sr;
  gfsmxlCascadeArcIter       cai;
  guint                      n_paths = 0;
  gfsmxlCascadeLookupBacktrace  bt = {NULL, gfsmEpsilon,gfsmEpsilon, sr->one, sr->zero};
#ifdef DEBUG_NCONFIGS
  guint n_configs = 1;
#endif

  //-- implicit reset
  gfsmxl_cascade_lookup_reset(cl);

  //-- get initial (root) config
  cfg = gfsmxl_cascade_lookup_config_new_full(cl->csc,                            //-- csc
					      gfsmxl_cascade_get_root(cl->csc),   //-- qids
					      0,                                  //-- ipos
					      gfsm_automaton_get_root(cl->otrie), //-- oid
					      gfsmNoState,                        //-- rid
					      sr->one,                            //-- w
					      bt);                                //-- bt (= {prev,lo,hi,aw.fw})
  gfsmxl_clc_fh_insert(cl->heap, cfg);
  g_hash_table_insert(cl->configs, cfg, g_slist_prepend(NULL, cfg));
  //
  //-- ye olde loope
  while ( (cl->n_ops <= cl->max_ops) && (cfg=gfsmxl_clc_fh_extractmin(cl->heap)) ) {
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
	if (!gfsm_sr_less(sr,cl->max_w,new_w) && gfsm_sr_less(sr,fw,cfg->bt.fw)) {
	  //-- !(max_w < fw) <---> (max_w >= fw) <---> (fw <= max_w) : add this config as a complete path
	  cfg->bt.fw = fw;
	  cl->finals = g_slist_prepend(cl->finals, cfg);
	  ++n_paths;
	  if (n_paths >= cl->max_paths) break; //-- found enough paths: get outta here..
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
      gfsmxlCascadeLookupConfig cfg_tmp = { csc, carc->targets, cfg->ipos, cfg->oid, cfg->rid, cfg->w, cfg->bt };
      gfsmxlCascadeLookupConfig *cfg_new;

      //-- adjust cfg_tmp.w
      cfg_tmp.w = gfsm_sr_times(sr,cfg->w,carc->weight);

      //-- check whether this arc would exceed max weight
      if (gfsm_sr_less(sr,cl->max_w,cfg_tmp.w)) continue;

      if (carc->lower != gfsmEpsilon) {
	//-- non-input-epsilon arc: adjust cfg_tmp.ipos
	cfg_tmp.ipos++;
      }
      if (carc->upper != gfsmEpsilon) {
	//-- non-output-epsilon arc: adjust cfg_tmp.oid
	cfg_tmp.oid = gfsm_trie_get_arc_lower(cl->otrie, cfg->oid, carc->upper, 0,FALSE);
      }

      //-- check whether an equal-or-better old config already exists, adding to heap
      if ( (cfg_new = gfsmxl_cascade_lookup_ensure_config(cl,&cfg_tmp)) == NULL ) continue;
#ifdef DEBUG_NCONFIGS
      ++n_configs;
#endif

      //-- setup new config backtrace
      cfg_new->bt.prev = cfg;
      cfg_new->bt.lo   = carc->lower;
      cfg_new->bt.hi   = carc->upper;
      cfg_new->bt.aw   = carc->weight;
      cfg_new->bt.fw   = sr->zero;
    }
    gfsmxl_cascade_arciter_close(&cai);
  }
#ifdef DEBUG_NCONFIGS
  fprintf(stderr, "(DEBUG): n_configs=%u\n", n_configs);
#endif
}

//--------------------------------------------------------------
void gfsmxl_cascade_lookup_nbest_backtrace_(gfsmxlCascadeLookup *cl, gfsmxlCascadeLookupConfig *cfg, gfsmAutomaton *result)
{
  //-- config: get state
  if (cfg->rid == gfsmNoState)
    cfg->rid = gfsm_automaton_add_state(result);

  //-- config: check for finality
  if (cfg->bt.fw != cfg->csc->sr->zero)
    gfsm_automaton_set_final_state_full(result,cfg->rid,TRUE,cfg->bt.fw);

  //-- config: parent
  if (cfg->bt.prev == NULL) {
    //-- no parent: set root
    gfsm_automaton_set_root(result, cfg->rid);
    return;
  }

  //-- parent: state
  if (cfg->bt.prev->rid == gfsmNoState) cfg->bt.prev->rid = gfsm_automaton_add_state(result);

  //-- parent: arc
  gfsm_automaton_add_arc(result, cfg->bt.prev->rid,cfg->rid, cfg->bt.lo,cfg->bt.hi,cfg->bt.aw);

  //-- parent: recurse
  gfsmxl_cascade_lookup_nbest_backtrace_(cl, cfg->bt.prev, result);
}

/*======================================================================
 * END
 */
//--------------------------------------------------------------
