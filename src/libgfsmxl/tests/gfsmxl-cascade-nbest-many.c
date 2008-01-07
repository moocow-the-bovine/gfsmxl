#include <gfsm.h>
#include <gfsmxlCascade.h>
#include <gfsmxlCascadeLookup.h>
#include <stdlib.h>
#include <string.h>

const char *prog = "gfsmxl-cascade-nbest";

/*======================================================================
 * Globals
 */
gfsmxlCascadeLookup *cl=NULL;
guint max_paths  = 1;
guint max_ops    = (guint)-1;
gfsmWeight max_w = 0;

gfsmLabelVector *labvec=NULL;
gfsmAutomaton  *result=NULL;
gfsmSet        *paths =NULL;
gfsmAlphabet   *abet  =NULL;
GString        *gstr  =NULL;

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
gboolean analyze_token_foreach_(gfsmPath *path, gpointer val_unused, FILE *out)
{
  g_string_truncate(gstr,0);
  gfsm_alphabet_labels_to_gstring(abet,
				  path->hi,
				  gstr,
				  TRUE, //-- warn_on_undefined
				  TRUE  //-- att_mode
				  );
  fprintf(out, "\t%s <%g>", gstr->str, path->w);
  return FALSE;
}
void analyze_token(char *text, FILE *out)
{
  //-- get labels
  if (labvec) g_ptr_array_set_size(labvec,0);
  labvec = gfsm_alphabet_string_to_labels(abet, text, labvec, TRUE);

  //-- lookup & connect
  result = gfsmxl_cascade_lookup_nbest(cl, labvec, result);

  //-- get stats
  nchars_total  += labvec->len;
  nops_total    += cl->n_ops;
  nstates_total += gfsm_automaton_n_states(result);
  
  //-- connect
  gfsm_automaton_connect(result);

  //-- serialize
  if (paths) gfsm_set_clear(paths);
  paths = gfsm_automaton_paths_full(result,paths,gfsmLSUpper);
  if (gfsm_set_size(paths) > 0) { ++ncovered; }

  //-- stringify
  fputs(text,out);
  gfsm_set_foreach(paths, (GTraverseFunc)analyze_token_foreach_, out);
  fputc('\n',out);
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
}

/*======================================================================
 * MAIN
 */
int main (int argc, char **argv)
{
  gfsmError     *err=NULL;
  gfsmxlCascade  *csc=NULL;
  char          *cscfile=NULL;
  char          *abetfile=NULL;
  char          *max_paths_str=NULL;
  char          *max_w_str=NULL;
  char          *ifilename="-";
  gfsmIOHandle  *ih=NULL;
  char          *linebuf=NULL;
  size_t         buflen=0;
  ssize_t        nread;
  GTimer         *timer=NULL;
  double         elapsed, ntoks_d, toks_per_sec, coverage;

  prog = argv[0];

  //-- check usage
  if (argc < 4) {
    g_printerr("Usage: %s GFSM_CASCADE_FILE ALPHABET_FILE MAX_PATHS MAX_W INPUT_FILE\n", prog);
    exit(1);
  }

  //-- show preprocessor configuration
  show_config_macros(prog);

  //-- load cascade
  csc = gfsmxl_cascade_new();
  cscfile = argv[1];
  if ( !gfsmxl_cascade_load_bin_filename(csc,cscfile,&err) ) {
    g_printerr("%s: error loading cascade file: '%s': %s\n", prog, cscfile, (err ? err->message : "?"));
    exit(2);
  }

  //-- load alphabet
  abetfile = argv[2];
  abet = gfsm_string_alphabet_new();
  if ( !gfsm_alphabet_load_filename(abet,abetfile,&err) ) {
    g_printerr("%s: error loading alphabet file: '%s': %s\n", prog, abetfile, (err ? err->message : "?"));
    exit(2);
  }

  //-- parse numeric args
  max_paths_str = argv[3];
  max_paths     = strtoul(max_paths_str,NULL,0);
  max_w_str     = argv[4];
  max_w         = strtod(max_w_str,NULL);

  //-- create cascade-lookup
  cl = gfsmxl_cascade_lookup_new_full(csc,max_w,max_paths,max_ops);

  //-- input file
  if (argc > 5) ifilename = argv[5];
  if ( !(ih = gfsmio_new_filename(ifilename, "r", -1, &err)) ) {
    fprintf(stderr, "%s: open failed for input file: %s\n", prog, (err ? err->message : "?"));
    exit(4);
  }

  //-- allocate globals
  labvec = g_ptr_array_new();
  gstr   = g_string_new("");

  //-- report
  fprintf(stderr, "%s: cascade file: %s\n", prog, cscfile);
  fprintf(stderr, "%s: max_paths   : %u\n", prog, cl->max_paths);
  fprintf(stderr, "%s: max_w       : %g\n", prog, cl->max_w);
  fprintf(stderr, "%s: max_ops     : %u\n", prog, cl->max_ops);
  fprintf(stderr, "%s: input file  : %s\n", prog, ifilename);
  fprintf(stderr, "%s: processing  : ", prog);

  //-- ye olde loope
  timer = g_timer_new();
  while (!gfsmio_eof(ih) && (nread=gfsmio_getline(ih,&linebuf,&buflen)) ) {
    g_strstrip(linebuf);
    if (linebuf[0]) {
      analyze_token(linebuf,stdout);
      //
#define TOKS_PER_DOT 128
      if ( ntoks%TOKS_PER_DOT == 0 ) { fputc('.',stderr); fflush(stderr); }
      ++ntoks;
    } else {
      fputc('\n',stdout);
    }
  }
  fprintf(stderr, " done.\n");

  elapsed = g_timer_elapsed(timer,NULL);
  ntoks_d = ntoks;
  toks_per_sec = ntoks_d / elapsed;
  coverage     = 100.0 * ((double)ncovered) / ntoks_d;

  fprintf(stderr, "%s: #/tokens    : %8lu tok\n", prog, ntoks);
  fprintf(stderr, "%s: #/chars     : %8lu chr  (%8.2f chr/tok)\n", prog, nchars_total, ((double)nchars_total)/ntoks_d);
  fprintf(stderr, "%s: #/ops       : %8lu ops  (%8.2f ops/tok)\n", prog, nops_total, ((double)nops_total/ntoks_d));
  fprintf(stderr, "%s: #/res_sts   : %8lu sts  (%8.2f sts/tok)\n", prog, nstates_total, ((double)nstates_total/ntoks_d));
  fprintf(stderr, "%s: covered     : %8lu tok  (%8.2f %%      )\n", prog, ncovered, coverage);
  fprintf(stderr, "%s: elapsed     : %8.2f sec\n", prog, elapsed);
  fprintf(stderr, "%s: throughput  : %8.2f tok/sec\n", prog, toks_per_sec);

  return 0;
}

