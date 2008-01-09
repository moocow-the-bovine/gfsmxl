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

#include "gfsmxl_cascade_compile_cmdparser.h"

/*--------------------------------------------------------------------------
 * Globals
 *--------------------------------------------------------------------------*/
char *progname = "gfsmxl_cascade_compile";

//-- options
struct gengetopt_args_info args;

//-- files
const char *outfilename = "-";

//-- global structs
gfsmxlCascade        *csc  = NULL;
gfsmSRType          srtype = gfsmSRTUnknown; //-- default semiring type
gfsmArcCompMask   sortmask = gfsmASMLower;   //-- default sort mask

/*--------------------------------------------------------------------------
 * Option Processing
 *--------------------------------------------------------------------------*/
void get_my_options(int argc, char **argv)
{
  if (cmdline_parser(argc, argv, &args) != 0)
    exit(1);

  //-- require at least one file argument
  if (args.inputs_num < 1) {
    cmdline_parser_print_help();
    exit(2);
  }

  //-- output
  if (args.output_given) outfilename = args.output_arg;

  //-- semiring type
  srtype = gfsm_sr_name_to_type(args.semiring_arg);

  //-- sort mask
  if (args.mode_given) { sortmask = gfsm_acmask_from_chars(args.mode_arg); }

  //-- initialize cascade
  csc = gfsmxl_cascade_new_full(args.inputs_num, srtype);
  gfsmxl_cascade_clear(csc,FALSE);
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
  int i;

  //GFSM_INIT

  get_my_options(argc,argv);

  //-- load indexed automaton files & create cascade
  for (i=0; i < args.inputs_num; i++) {
    const char *xfsmfile       = args.inputs[i];
    gfsmIndexedAutomaton *xfsm = gfsm_indexed_automaton_new();
    if ( !gfsm_indexed_automaton_load_bin_filename(xfsm,xfsmfile,&err) ) {
      g_printerr("%s: error loading indexed automaton file '%s': %s\n",
		 progname, xfsmfile, (err ? err->message : "?"));
      exit(3);
    }
    gfsmxl_cascade_append_indexed(csc,xfsm);
  }

  //-- sort cascade
  if (sortmask) gfsmxl_cascade_sort_all(csc, sortmask);

  //-- spew cascade
  if ( !gfsmxl_cascade_save_bin_filename(csc, outfilename, args.compress_arg, &err) ) {
    g_printerr("%s: store failed to '%s': %s\n", progname, outfilename, err ? err->message : "?");
    exit(4);
  }

  //-- cleanup
  if (csc) gfsmxl_cascade_free(csc,TRUE);

  //GFSM_FINISH

  return 0;
}
