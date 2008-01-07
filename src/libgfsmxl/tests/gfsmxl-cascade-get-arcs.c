#include <gfsm.h>
#include <gfsmxlCascade.h>
#include <stdlib.h>
#include <string.h>

const char *prog = "gfsmxl-cascade-get-arcs";

void fprintf_qids(FILE *f, gfsmxlCascadeStateId qids, guint depth)
{
  int i;
  fprintf(f, "(");
  for (i=0; i < depth; i++) {
    fprintf(f, "%s%d", (i==0 ? "" : ","), qids[i]);
  }
  fprintf(f,")");
}


/*======================================================================
 * show_config_macros
 */
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
  char          *labstr =NULL;
  gfsmLabelId         lab=gfsmEpsilon;
  gfsmxlCascadeStateId qids=NULL;
  int i, argi;
  gfsmxlCascadeArcIter cai;

  prog = argv[0];

  //-- check usage
  if (argc < 3) {
    g_printerr("Usage: %s GFSM_CASCADE_FILE LABEL STATEIDs...\n", prog);
    exit(1);
  }

  //-- show configuration
  show_config_macros(prog);

  //-- load cascade
  cscfile = argv[1];
  csc = gfsmxl_cascade_new();
  cscfile = argv[1];
  if ( !gfsmxl_cascade_load_bin_filename(csc,cscfile,&err) ) {
    g_printerr("%s: error loading cascade file: '%s': %s\n", prog, cscfile, (err ? err->message : "?"));
    exit(2);
  }

  //-- parse label
  labstr  = argv[2];
  lab = strtoul(labstr,NULL,0);

  //-- parse source state
  qids = gfsmxl_cascade_stateid_sized_new(csc->depth);
  gfsmxl_cascade_stateid_sized_copy(qids,gfsmxl_cascade_get_root(csc),csc->depth);
  for (argi=3,i=0; argi < argc && i < csc->depth; argi++,i++) {
    qids[i] = strtoul(argv[argi],NULL,0);
  }

  //-- show confgiguration
  fprintf(stderr, "%s: cascade='%s'\n", prog, cscfile);
  fprintf(stderr, "%s: label=%d\n", prog, lab);
  fprintf(stderr, "%s: state=", prog);
  fprintf_qids(stderr,qids,csc->depth);
  fprintf(stderr, "\n");

  //-- get requested arcs
  gfsmxl_cascade_arciter_open(&cai, csc, qids, lab);
  fprintf(stderr, "%s: got %d arcs\n", prog, g_slist_length(cai.arclist));
  for (; gfsmxl_cascade_arciter_ok(&cai); gfsmxl_cascade_arciter_next(&cai)) {
    gfsmxlCascadeArc *carc = gfsmxl_cascade_arciter_arc(&cai);
    fprintf_qids(stdout, qids, csc->depth);
    fprintf(stdout, " --[%d:%d]<%g>--> ", carc->lower, carc->upper, carc->weight);
    fprintf_qids(stdout,carc->targets,csc->depth);
    fprintf(stdout, "\n");
  }
  gfsmxl_cascade_arciter_close(&cai);

  return 0;
}

