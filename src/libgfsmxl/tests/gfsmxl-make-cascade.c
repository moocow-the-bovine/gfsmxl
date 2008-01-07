#include <gfsm.h>
#include <gfsmxlCascade.h>
#include <stdlib.h>

const char *prog = "gfsmxl-make-cascade";

int main (int argc, char **argv)
{
  gfsmError     *err=NULL;
  gfsmxlCascade  *csc=NULL;
  int i;

  prog = argv[0];

  //-- check usage
  if (argc < 2) {
    g_printerr("Usage: %s GFSM_INDEXED_FILE(s)...\n", prog);
    exit(1);
  }

  //-- load gfsm files & create cascade
  csc = gfsmxl_cascade_new();
  for (i=1; i < argc; i++) {
    const char *fsmfile = argv[i];
    gfsmIndexedAutomaton *xfsm = gfsm_indexed_automaton_new();
    if ( !gfsm_indexed_automaton_load_bin_filename(xfsm,fsmfile,&err) ) {
      g_printerr("%s: error loading indexed automaton file '%s': %s\n",
		 prog, fsmfile, (err ? err->message : "?"));
      exit(2);
    }
    gfsmxl_cascade_append_indexed(csc,xfsm);
  }

  //-- sort cascade
  gfsmxl_cascade_sort_all(csc, gfsm_acmask_from_args(gfsmACLower,gfsmACWeight,gfsmACUpper,gfsmACTarget));

  //-- dump cascade
  if (!gfsmxl_cascade_save_bin_filename(csc,"-",-1,&err)) {
    g_printerr("%s: error saving cascade to stdout: %s\n", prog, (err ? err->message : "?"));
    exit(3);
  }

  return 0;
}

