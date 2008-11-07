/*
   gfsmxl-utils : finite state automaton utilities
   Copyright (C) 2008 by Bryan Jurish <moocow@ling.uni-potsdam.de>

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

#include "gfsmxl_cascade_nbest_lookup_cmdparser.h"

/*======================================================================
 * Globals
 */
const char *progname = "gfsmxl_cascade_nbest_lookup";

//-- options
struct gengetopt_args_info args;

//-- files
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

/*======================================================================
 * Utils
 */
void analyze_cmdline_words(int argc, char **argv)
{
  int i;

  //-- get labels
  if (labvec) g_ptr_array_set_size(labvec,0);
  for (i=0; i < argc; i++) {
    labvec = gfsm_alphabet_generic_string_to_labels(abet, argv[i], labvec, TRUE, att_mode);
  }

  //-- lookup & connect
  result = gfsmxl_cascade_lookup_nbest(cl, labvec, result);
  gfsm_automaton_connect(result);
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
  get_my_options(argc,argv);

  //-- allocate globals
  labvec = g_ptr_array_new();

  //-- guts
  analyze_cmdline_words(args.inputs_num, args.inputs);

  //-- output
  if ( !gfsm_automaton_save_bin_filename(result,outfilename,-1,&err) ) {
    fprintf(stderr, "%s: save failed for output file '%s': %s",
	    progname, outfilename, (err ? err->message : "?"));
    exit(3);
  }

  if (labvec)   { g_ptr_array_free(labvec,TRUE); }
  if (cl)       { gfsmxl_cascade_lookup_free(cl); csc=NULL; }
  if (csc)      { gfsmxl_cascade_free(csc,TRUE); }

  return 0;
}

