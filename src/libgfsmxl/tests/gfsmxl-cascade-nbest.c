#include <gfsm.h>
#include <gfsmxlCascade.h>
#include <gfsmxlCascadeLookup.h>
#include <stdlib.h>
#include <string.h>

const char *prog = "gfsmxl-cascade-nbest";

#define MAX_OPS gfsmNoState

/*======================================================================
 * Utils
 */
void fprintf_labvec(FILE *f, gfsmLabelVector *labvec)
{
  int i;
  fprintf(f, "(");
  for (i=0; i < labvec->len; i++) {
    guint lab = GPOINTER_TO_UINT(g_ptr_array_index(labvec,i));
    fprintf(f, "%s%u", (i==0 ? "" : " "), lab);
  }
  fprintf(f,")");
}


/*======================================================================
 * MAIN
 */
int main (int argc, char **argv)
{
  gfsmError     *err=NULL;
  gfsmxlCascade  *csc=NULL;
  gfsmxlCascadeLookup *cl=NULL;
  char          *cscfile=NULL;
  char          *n_best_str=NULL;
  guint          n_best;
  char          *max_w_str=NULL;
  gfsmWeight     max_w;
  guint          max_ops = MAX_OPS;
  gfsmLabelVector *labvec=NULL;
  gfsmAutomaton   *result=NULL;
  int argi;

  prog = argv[0];

  //-- check usage
  if (argc < 4) {
    g_printerr("Usage: %s GFSM_CASCADE_FILE NBEST MAXW LABEL(s)... > GFSM_FILE\n", prog);
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

  //-- parse numeric args
  n_best_str = argv[2];
  n_best     = strtoul(n_best_str,NULL,0);
  max_w_str  = argv[3];
  max_w      = strtod(max_w_str,NULL);
  max_ops    = MAX_OPS;

  //-- create cascade-lookup
  cl = gfsmxl_cascade_lookup_new_full(csc,max_w,n_best,max_ops);

  //-- parse input labels
  labvec = g_ptr_array_sized_new(argc-4);
  for (argi=4; argi < argc; argi++) {
    char  *labstr = argv[argi];
    guint     lab = strtoul(labstr,NULL,0);
    g_ptr_array_add(labvec, GUINT_TO_POINTER(lab));
  }

  //-- report
  fprintf(stderr, "%s: cascade file: %s\n", prog, cscfile);
  fprintf(stderr, "%s: max_w       : %g\n", prog, cl->max_w);
  fprintf(stderr, "%s: max_paths   : %u\n", prog, cl->max_paths);
  fprintf(stderr, "%s: max_ops     : %u\n", prog, cl->max_ops);
  fprintf(stderr, "%s: input       : ", prog);
  fprintf_labvec(stderr, labvec);
  fprintf(stderr, "\n");

  //-- lookup
  result = gfsmxl_cascade_lookup_nbest(cl, labvec, result);

  //-- dump result
  if (!gfsm_automaton_save_bin_filename(result,"-",-1,&err)) {
    g_printerr("%s: error saving result automaton to stdout: %s\n", prog, (err ? err->message : "?"));
    exit(3);
  }

  //-- debug: store output trie
  gfsm_automaton_save_bin_filename(cl->otrie,"otrie.gfst",-1,&err);

  //-- cleanup
  gfsmxl_cascade_lookup_free(cl);

  return 0;
}

