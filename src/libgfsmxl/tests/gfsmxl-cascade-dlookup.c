#include <gfsm.h>
#include <gfsmxl.h>
#include <stdlib.h>
#include <string.h>

const char *prog = "gfsmxl-cascade-dlookup";

/*======================================================================
 * Globals
 */
gfsmxlCascadeLookup *cl=NULL;
guint max_paths  = 1;
guint max_ops    = (guint)-1;
gfsmWeight max_w = 1e38;

gfsmLabelVector *labvec=NULL;
gfsmAutomaton  *result=NULL;
gfsmSet        *paths =NULL;
gfsmAlphabet   *abet  =NULL;
GString        *gstr  =NULL;

/*======================================================================
 * MAIN
 */
int main (int argc, char **argv)
{
  gfsmError      *err=NULL;
  gfsmxlCascade  *csc=NULL;
  char	        *abetfile=NULL;
  char          *cscfile=NULL;
  char		*iword=NULL;

  prog = argv[0];

  //-- check usage
  if (argc < 3) {
    g_printerr("Usage: %s LABELS CASCADE WORD\n", prog);
    exit(1);
  }

  //-- load alphabet
  abetfile = argv[1];
  abet = gfsm_string_alphabet_new();
  if ( !gfsm_alphabet_load_filename(abet,abetfile,&err) ) {
    g_printerr("%s: error loading alphabet file: '%s': %s\n", prog, abetfile, (err ? err->message : "?"));
    exit(2);
  }

  //-- load cascade
  cscfile = argv[2];
  csc = gfsmxl_cascade_new();
  if ( !gfsmxl_cascade_load_bin_filename(csc,cscfile,&err) ) {
    g_printerr("%s: error loading cascade file: '%s': %s\n", prog, cscfile, (err ? err->message : "?"));
    exit(2);
  }

  //-- parse input word
  iword = argv[3];
  labvec = gfsm_alphabet_string_to_labels(abet, iword, labvec, TRUE);

  //-- lookup
  cl     = gfsmxl_cascade_lookup_new_full(csc,max_w,max_paths,max_ops);
#if 0
  result = gfsmxl_cascade_lookup_nbest_debug(cl, labvec, result);
#else
  result = gfsmxl_cascade_lookup_nbest(cl, labvec, result);
#endif

  //-- dump
  if (!gfsm_automaton_save_bin_filename(result,"-",-1,&err)) {
    g_printerr("%s: error saving result to stdout: %s\n", prog, (err ? err->message : "?"));
    exit(3);
  }

  return 0;
}

