/*
   gfsmxl-utils : finite state automaton utilities
   Copyright (C) 2008-2014 by Bryan Jurish <moocow.bovine@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include <gfsm.h>
#include <gfsmxl.h>

#include "gfsmxl_cascade_nbest_strings_cmdparser.h"

/*======================================================================
 * Globals
 */
const char *progname = "gfsmxl_cascade_nbest_strings";

#define TOKS_PER_DOT 128

//-- options
struct gengetopt_args_info args;

//-- files
const char *infilename  = "-";
const char *outfilename = "-";
const char *cscfilename = NULL;
const char *abetfilename = NULL;

//-- flags
gboolean att_mode = FALSE;

//-- global structs
gfsmAlphabet        *abet=NULL;
gfsmxlCascade       *csc=NULL;
gfsmxlCascadeLookup *cl=NULL;
gfsmError           *err=NULL;

gfsmLabelVector *labvec=NULL;
gfsmAutomaton   *result=NULL;
gfsmSet         *paths =NULL;
gfsmxlPathArray *pathsa=NULL;
GString         *gstr  =NULL;

/*======================================================================
 * Coverage tracking
 */
long unsigned int ntoks=0;
long unsigned int ncovered=0;
long unsigned int nops_total=0;
long unsigned int nstates_total=0;
long unsigned int nchars_total=0;

/*======================================================================
 * Utils
 */
gboolean analyze_token_foreach_(gfsmPath *path, gpointer val_unused, gfsmIOHandle *outh)
{
  g_string_truncate(gstr,0);
  gfsm_alphabet_labels_to_gstring(abet,
				  path->hi,
				  gstr,
				  TRUE, //-- warn_on_undefined
				  TRUE  //-- att_mode
				  );
  gfsmio_printf(outh, "\t%s <%g>", gstr->str, path->w);
  return FALSE;
}

void analyze_token(char *text, gfsmIOHandle *outh)
{
  //-- get labels
  if (labvec) g_ptr_array_set_size(labvec,0);
  labvec = gfsm_alphabet_generic_string_to_labels(abet, text, labvec, TRUE, att_mode);

  //-- lookup & connect
  if (args.debug_flag) {
    //-- lookup & connect: use temporary fst
    result = gfsmxl_cascade_lookup_nbest(cl, labvec, result);

    //-- get stats
    nchars_total  += labvec->len;
    nops_total    += cl->n_ops;
    nstates_total += gfsm_automaton_n_states(result);

    //-- connect
    //gfsm_automaton_connect(result); /*-- not needed for backtrace-construction --*/

    //-- serialize
    if (paths) gfsm_set_clear(paths);
    paths = gfsm_automaton_paths_full(result,paths,gfsmLSUpper);

    //-- coverage
    if (gfsm_set_size(paths) > 0) { ++ncovered; }

    //-- stringify
    gfsmio_puts(outh,text);
    gfsm_set_foreach(paths, (GTraverseFunc)analyze_token_foreach_, outh);
    gfsmio_putc(outh,'\n');
  }
  else {
    //-- lookup & connect: direct serialization
    int i;
    pathsa = gfsmxl_cascade_lookup_nbest_paths(cl, labvec, pathsa);

    //-- get stats
    nchars_total  += labvec->len;
    nops_total    += cl->n_ops;
    //nstates_total += gfsm_automaton_n_states(result);

    //-- coverage
    if (pathsa->len > 0) { ++ncovered; }

    //-- stringify
    gfsmio_puts(outh,text);
    for (i=0; i<pathsa->len; i++) {
      analyze_token_foreach_((gfsmPath*)g_ptr_array_index(pathsa,i), NULL, outh);
    }
    gfsmio_putc(outh,'\n');
  }
}

void show_config_macros(const char *prog)
{
  fprintf(stderr,"%s: %-32s ? %s\n", prog, "CASCADE_USE_BLOCKS",
#if defined(CASCADE_USE_BLOCKS)
	 "yes"
#else
	 "no"
#endif
	 );

  fprintf(stderr,"%s: %-32s ? %s\n", prog, "CASCADE_USE_BLOCK_INDEX",
#if defined(CASCADE_USE_BLOCK_INDEX)
	 "yes"
#else
	 "no"
#endif
	 );

  fprintf(stderr,"%s: %-32s ? %s\n", prog, "CASCADE_EXPAND_BLOCK_BSEARCH",
#if defined(CASCADE_EXPAND_BLOCK_BSEARCH)
	 "yes"
#else
	 "no"
#endif
	 );

  fprintf(stderr,"%s: %-32s ? %s\n", prog, "CASCADE_USE_BLOCK_HASH",
#if defined(CASCADE_USE_BLOCK_HASH)
	 "yes"
#else
	 "no"
#endif
	 );

  fprintf(stderr,"%s: %-32s ? %s\n", prog, "CASCADE_USE_RAW_BSEARCH",
#if defined(CASCADE_USE_RAW_BSEARCH)
	 "yes"
#else
	 "no"
#endif
	 );

  fprintf(stderr,"%s: %-32s ? %s\n", prog, "CASCADE_SORT_ARCITER",
#if defined(CASCADE_SORT_ARCITER)
	 "yes"
#else
	 "no"
#endif
	 );

  fprintf(stderr,"%s: %-32s ? %s\n", prog, "CASCADE_EXPAND_RECURSIVE",
#if defined(CASCADE_EXPAND_RECURSIVE)
	 "yes"
#else
	 "no"
#endif
	 );

  fprintf(stderr,"%s: %-32s ? %s\n", prog, "GFSMXL_DEBUG_ENABLED",
#if defined(GFSMXL_DEBUG_ENABLED)
	 "yes"
#else
	 "no"
#endif
	 );

  fprintf(stderr,"%s: %-32s ? %s\n", prog, "GFSMXL_CLC_FH_STATS",
#if defined(GFSMXL_CLC_FH_STATS)
	 "yes"
#else
	 "no"
#endif
	 );
}

/*--------------------------------------------------------------------------
 * Option Processing
 *--------------------------------------------------------------------------*/
void get_my_options(int argc, char **argv)
{
  if (cmdline_parser(argc, argv, &args) != 0)
    exit(1);

  //-- sanity check: cascade file
  if (!args.cascade_given) {
    fprintf(stderr, "%s: no cascade file specified!", progname);
    cmdline_parser_print_help();
    exit(1);
  } else { cscfilename = args.cascade_arg; }
  //-- sanity check: alphabet file
  if (!args.labels_given) {
    fprintf(stderr, "%s: no alphabet file specified!", progname);
    cmdline_parser_print_help();
    exit(1);
  } else { abetfilename = args.labels_arg; }

  //-- input file
  if (args.inputs_num) {
    infilename = args.inputs[0];
  }

  //-- output file
  if (args.output_given) outfilename = args.output_arg;

  //-- load: cascade
  csc = gfsmxl_cascade_new();
  if ( !gfsmxl_cascade_load_bin_filename(csc,cscfilename,&err) ) {
    fprintf(stderr, "%s: load failed for cascade file '%s': %s",
	    progname, cscfilename, (err ? err->message : "?"));
    exit(2);
  }

  //-- load: alphabet
  abet = gfsm_string_alphabet_new();
  if ( !gfsm_alphabet_load_filename(abet,abetfilename,&err) ) {
    fprintf(stderr, "%s: load failed for labels file '%s': %s",
	    progname, abetfilename, (err ? err->message : "?"));
    exit(2);
  }
  abet->utf8 = args.utf8_flag;

  //-- create cascade-lookup
  cl = gfsmxl_cascade_lookup_new();
  gfsmxl_cascade_lookup_set_cascade(cl,csc);

  //-- search parameters
  if (args.max_weight_given) {
    cl->max_w = args.max_weight_arg;
  } else {
    cl->max_w = csc->sr->zero;
  }
  cl->max_paths  = args.max_paths_arg;
  cl->max_ops    = (guint)args.max_ops_arg;

  //-- flags
  att_mode = args.att_mode_flag;
}

/*======================================================================
 * MAIN
 */
int main (int argc, char **argv)
{
  gfsmIOHandle  *ih=NULL, *outh=NULL;
  char          *linebuf=NULL;
  size_t         buflen=0;
  ssize_t        nread;
  GTimer         *timer=NULL;
  double         elapsed, ntoks_d, toks_per_sec, coverage, nchars_d;
  const char *prog = progname;

  get_my_options(argc,argv);

  //-- show preprocessor configuration
  //show_config_macros(progname);

  //-- report
  fprintf(stderr, "%s: cascade file: %s\n", progname, cscfilename);
  fprintf(stderr, "%s: alphabet    : %s\n", progname, abetfilename);
  fprintf(stderr, "%s: max_paths   : %u\n", progname, cl->max_paths);
  fprintf(stderr, "%s: max_w       : %g\n", progname, cl->max_w);
  fprintf(stderr, "%s: max_ops     : %u\n", progname, cl->max_ops);
  fprintf(stderr, "%s: debug       : %u\n", progname, args.debug_flag);
  fprintf(stderr, "%s: input file  : %s\n", progname, infilename);
  fprintf(stderr, "%s: output file : %s\n", progname, outfilename);
  fprintf(stderr, "%s: processing  : ", progname);

  //-- allocate globals
  labvec = g_ptr_array_new();
  gstr   = g_string_new("");

  //-- open: input file
  if ( !(ih = gfsmio_new_filename(infilename, "r", -1, &err)) ) {
    fprintf(stderr, "%s: open failed for input file '%s': %s\n", progname, infilename, (err ? err->message : "?"));
    exit(3);
  }
  //-- open: output file
  if ( !(outh=gfsmio_new_filename(outfilename, "w", 0, &err)) ) {
    fprintf(stderr, "%s: open failed for output file '%s': %s\n", progname, outfilename, (err ? err->message : "?"));
    exit(3);
  }

  //-- ye olde loope
  timer = g_timer_new();
  while (!gfsmio_eof(ih) && (nread=gfsmio_getline(ih,&linebuf,&buflen)) ) {
    g_strstrip(linebuf);
    if (linebuf[0]) {
      analyze_token(linebuf,outh);
      //
      if ( ntoks%TOKS_PER_DOT == 0 ) { fputc('.',stderr); fflush(stderr); }
      ++ntoks;
    } else {
      gfsmio_putc(outh,'\n');
    }
  }
  fprintf(stderr, " done.\n");

  elapsed = g_timer_elapsed(timer,NULL);
  ntoks_d = ntoks;
  nchars_d = nchars_total;
  toks_per_sec = ntoks_d / elapsed;
  coverage     = 100.0 * ((double)ncovered) / ntoks_d;

#ifdef GFSMXL_CLC_FH_STATS
  fprintf(stderr, "%s: fh:maxn     : %8d\n", prog, gfsmxl_clc_fh_maxn(cl->heap));
  fprintf(stderr, "%s: fh:#/insert : %8d\n", prog, gfsmxl_clc_fh_ninserts(cl->heap));
  fprintf(stderr, "%s: fh:#/extrct : %8d\n", prog, gfsmxl_clc_fh_nextracts(cl->heap));
#endif
  fprintf(stderr, "%s: #/tokens    : %8lu tok\n", prog, ntoks);
  fprintf(stderr, "%s: #/chars     : %8lu chr  (%8.2f chr/tok)\n", prog, nchars_total, ((double)nchars_total)/ntoks_d);
  fprintf(stderr, "%s: #/ops       : %8lu ops  (%8.2f ops/tok == %8.2f ops/chr)\n",
	  prog, nops_total, ((double)nops_total/ntoks_d), ((double)nops_total/nchars_d));
  fprintf(stderr, "%s: #/res_sts   : %8lu sts  (%8.2f sts/tok == %8.2f sts/chr)\n",
	  prog, nstates_total, ((double)nstates_total/ntoks_d), ((double)nstates_total/nchars_d));
  fprintf(stderr, "%s: covered     : %8lu tok  (%8.2f %%      )\n", prog, ncovered, coverage);
  fprintf(stderr, "%s: elapsed     : %8.2f sec\n", prog, elapsed);
  fprintf(stderr, "%s: throughput  : %8.2f tok/sec\n", prog, toks_per_sec);

  //-- cleanup & finish
  gfsmio_close(ih);
  gfsmio_handle_free(ih);

  gfsmio_close(outh);
  gfsmio_handle_free(outh);

  if (linebuf)  { g_free(linebuf); }
  if (labvec)   { g_ptr_array_free(labvec,TRUE); }
  if (gstr)     { g_string_free(gstr,TRUE); }
  if (timer)    { g_timer_destroy(timer); }
  if (cl)       { gfsmxl_cascade_lookup_free(cl); csc=NULL; }
  if (csc)      { gfsmxl_cascade_free(csc,TRUE); }

  return 0;
}

