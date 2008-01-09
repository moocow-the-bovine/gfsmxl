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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <gfsmxl.h>

#include "gfsmxl_cascade_nth_cmdparser.h"

/*--------------------------------------------------------------------------
 * Globals
 *--------------------------------------------------------------------------*/
char *progname = "gfsmxl_cascade_nth";

//-- options
struct gengetopt_args_info args;

//-- files
const char *infilename  = "-";
const char *outfilename = "-";

//-- global structs
gfsmxlCascade        *csc  = NULL;
gfsmIndexedAutomaton *xfsm = NULL;

/*--------------------------------------------------------------------------
 * Option Processing
 *--------------------------------------------------------------------------*/
void get_my_options(int argc, char **argv)
{
  if (cmdline_parser(argc, argv, &args) != 0)
    exit(1);

  //-- I/O
  if (args.inputs_num > 0) infilename = args.inputs[0];
  if (args.output_given) outfilename = args.output_arg;

  //-- initialize cascade
  csc = gfsmxl_cascade_new();
}

/*--------------------------------------------------------------------------
 * Utilities
 *--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
 * MAIN
 *--------------------------------------------------------------------------*/
int main (int argc, char **argv)
{
  gfsmError *err = NULL;

  //GFSM_INIT

  get_my_options(argc,argv);

  //-- load cascade
  if (!gfsmxl_cascade_load_bin_filename(csc,infilename,&err)) {
    g_printerr("%s: error loading cascade file '%s': %s\n",
	       progname, infilename, (err ? err->message : "?"));
    exit(3);
  }

  //-- sanity check(s)
  if (args.index_arg >= csc->depth) {
    g_printerr("%s: requested automaton index (=%d) exceeds cascade maximum index (=%d) -- aborting!\n",
	       progname, args.index_arg, (csc->depth-1));
    exit(4);
  }

  //-- get & spew indexed automaton
  xfsm = gfsmxl_cascade_index(csc,args.index_arg);
  if ( !gfsm_indexed_automaton_save_bin_filename(xfsm, outfilename, args.compress_arg, &err) ) {
    g_printerr("%s: store failed to '%s': %s\n", progname, outfilename, (err ? err->message : "?"));
    exit(5);
  }

  //-- cleanup
  if (csc) gfsmxl_cascade_free(csc,TRUE);

  //GFSM_FINISH

  return 0;
}
