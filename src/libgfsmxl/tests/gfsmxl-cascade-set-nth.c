#include <gfsm.h>
#include <gfsmxlCascade.h>
#include <stdlib.h>
#include <string.h>

const char *prog = "gfsmxl-cascade-set-nth";


/*======================================================================
 * MAIN
 */
int main (int argc, char **argv)
{
  gfsmError      *err=NULL;
  gfsmxlCascade  *csc=NULL;
  gfsmIndexedAutomaton *xfsm=NULL;
  char          *cscfile=NULL;
  char		*xfsmfile=NULL;
  guint n;

  prog = argv[0];

  //-- check usage
  if (argc < 3) {
    g_printerr("Usage: %s GFSM_CASCADE_FILE N GFSM_INDEXED_FILE...\n", prog);
    exit(1);
  }

  //-- load cascade
  cscfile = argv[1];
  csc = gfsmxl_cascade_new();
  cscfile = argv[1];
  if ( !gfsmxl_cascade_load_bin_filename(csc,cscfile,&err) ) {
    g_printerr("%s: error loading cascade file: '%s': %s\n", prog, cscfile, (err ? err->message : "?"));
    exit(2);
  }

  //-- parse "n"
  n = strtoul(argv[2],NULL,0);
  if (n > csc->depth) {
    g_printerr("%s: argument n=%u > csc->depth=%u\n", prog, n, csc->depth);
    exit(2);
  }

  //-- load replacement xfsm
  xfsm = gfsm_indexed_automaton_new();
  xfsmfile = argv[3];
  if ( !gfsm_indexed_automaton_load_bin_filename(xfsm,xfsmfile,&err) ) {
    g_printerr("%s: error loading indexed automaton file '%s': %s\n",
	       prog, xfsmfile, (err ? err->message : "?"));
    exit(2);
  }
  gfsmxl_cascade_set_nth_indexed(csc,n,xfsm,FALSE);

  //-- dump
  if (!gfsmxl_cascade_save_bin_filename(csc,"-",-1,&err)) {
    g_printerr("%s: error saving cascade to stdout: %s\n", prog, (err ? err->message : "?"));
    exit(3);
  }

  return 0;
}

