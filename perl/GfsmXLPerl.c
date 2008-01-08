#include "GfsmXLPerl.h"
#include <fcntl.h>

#undef VERSION
#include <gfsmxlConfig.h>

/*#define GFSMDEBUG 1*/

/*======================================================================
 * Memory Stuff
 */
gpointer gfsm_perl_malloc(gsize n_bytes)
{
  gpointer ptr=NULL;
  Newc(0, ptr, n_bytes, char, gpointer);
  return ptr;
}

gpointer gfsm_perl_realloc(gpointer mem, gsize n_bytes)
{
  Renewc(mem, n_bytes, char, gpointer);
  return mem;
}

void gfsm_perl_free(gpointer mem)
{
  Safefree(mem);
}

/*======================================================================
 * Gfsm::XL::Cascade Utilities
 */

gfsmxlCascadePerl *gfsmxl_perl_cascade_new(void)
{
  gfsmxlCascadePerl *cscp = g_new0(gfsmxlCascadePerl,1);
  cscp->av = newAV();
  return cscp;
}

void gfsmxl_perl_cascade_clear(gfsmxlCascadePerl *cscp)
{
  gfsmxl_cascade_clear(cscp->csc,FALSE);
  av_clear(cscp->av);
}

void gfsmxl_perl_cascade_free(gfsmxlCascadePerl *cscp)
{
  gfsmxl_perl_cascade_clear(cscp);
  av_undef(cscp->av);
  gfsmxl_cascade_free(cscp->csc,FALSE);
  g_free(cscp);
}

void  gfsmxl_perl_cascade_append_sv(gfsmxlCascadePerl *cscp, SV *xfsm_sv)
{
  gfsmIndexedAutomaton *xfsm = (gfsmIndexedAutomaton*)SvIV((SV*)SvRV(xfsm_sv));
  SV *xfsm_sv_copy;
  //g_printerr("cascade_append_sv(cscp=%p, cscp->csc=%p): xfsm_sv=%p, xfsm=%p\n", cscp, cscp->csc, xfsm_sv, xfsm);
  //
  xfsm_sv_copy = sv_mortalcopy(xfsm_sv);     //-- array-stored value (mortal)
  SvREFCNT_inc(xfsm_sv_copy);                //   : mortal needs incremented refcnt
  av_push(cscp->av, xfsm_sv_copy);           //   : store
  //
  gfsmxl_cascade_append_indexed(cscp->csc, xfsm);
}

SV *gfsmxl_perl_cascade_get_sv(gfsmxlCascadePerl *cscp, int i)
{
  SV **fetched = av_fetch(cscp->av, i, 0);
  if (fetched) {
    SV *rv = sv_mortalcopy(*fetched);
    SvREFCNT_inc(rv);
    return rv;
  }
  return &PL_sv_undef;
}

void gfsmxl_perl_cascade_refresh_av(gfsmxlCascadePerl *cscp)
{
  int i;
  av_clear(cscp->av);
  for (i=0; i < cscp->csc->depth; i++) {
    gfsmIndexedAutomaton *xfsm = gfsmxl_cascade_index(cscp->csc,i);
    SV                   *svrv = newSV(0);
    sv_setref_pv(svrv, "Gfsm::Automaton::Indexed", (void*)xfsm);
    av_push(cscp->av, svrv);
  }
}

/*======================================================================
 * Type conversions
 */
AV *gfsm_perl_paths_to_av(gfsmSet *paths_s)
{
  int i;
  AV *RETVAL = newAV();
  GPtrArray *paths_a=g_ptr_array_sized_new(gfsm_set_size(paths_s));
  gfsm_set_to_ptr_array(paths_s, paths_a);

  for (i=0; i < paths_a->len; i++) {
    gfsmPath *path = (gfsmPath*)g_ptr_array_index(paths_a,i);
    HV       *hv   = gfsm_perl_path_to_hv(path);
    av_push(RETVAL, newRV((SV*)hv));
  }
  g_ptr_array_free(paths_a,TRUE);

  sv_2mortal((SV*)RETVAL);  
  return RETVAL;
}

HV *gfsm_perl_path_to_hv(gfsmPath *path)
{
  HV *hv = newHV();
  AV *lo = gfsm_perl_ptr_array_to_av_uv(path->lo);
  AV *hi = gfsm_perl_ptr_array_to_av_uv(path->hi);

  hv_store(hv, "lo", 2, newRV((SV*)lo), 0);
  hv_store(hv, "hi", 2, newRV((SV*)hi), 0);
  hv_store(hv, "w",  1, newSVnv(gfsm_perl_weight_getfloat(path->w)), 0);

  sv_2mortal((SV*)hv);
  return hv;
}

AV *gfsm_perl_ptr_array_to_av_uv(GPtrArray *ary)
{
  AV *av = newAV();
  guint i;
  for (i=0; i < ary->len; i++) {
    av_push(av, newSVuv((UV)g_ptr_array_index(ary,i)));
  }
  sv_2mortal((SV*)av);
  return av;
}


/*======================================================================
 * I/O: Constructors: SV*
 */
gfsmIOHandle *gfsmperl_io_new_sv(SV *sv, size_t pos)
{
  gfsmPerlSVHandle *svh = g_new(gfsmPerlSVHandle,1);
  gfsmIOHandle *ioh = gfsmio_handle_new(gfsmIOTUser,svh);

  SvUTF8_off(sv); //-- unset UTF8 flag for this SV*

  svh->sv = sv;
  svh->pos = pos;

  ioh->read_func = (gfsmIOReadFunc)gfsmperl_read_sv;
  ioh->write_func = (gfsmIOWriteFunc)gfsmperl_write_sv;
  ioh->eof_func = (gfsmIOEofFunc)gfsmperl_eof_sv;

  return ioh;
}

void gfsmperl_io_free_sv(gfsmIOHandle *ioh)
{
  gfsmPerlSVHandle *svh = (gfsmPerlSVHandle*)ioh->handle;
  g_free(svh);
  gfsmio_handle_free(ioh);
}

/*======================================================================
 * I/O: Methods: SV*
 */
gboolean gfsmperl_eof_sv(gfsmPerlSVHandle *svh)
{ return svh && svh->sv ? (STRLEN)svh->pos >= sv_len(svh->sv) : TRUE; }

gboolean gfsmperl_read_sv(gfsmPerlSVHandle *svh, void *buf, size_t nbytes)
{
  char *svbytes;
  STRLEN len;
  if (!svh || !svh->sv) return FALSE;

  svbytes = sv_2pvbyte(svh->sv, &len);
  if ((STRLEN)(svh->pos+nbytes) <= len) {
    //-- normal case: just copy
    memcpy(buf, svbytes+svh->pos, nbytes);
    svh->pos += nbytes;
    return TRUE;
  }
  //-- overflow: grab what we can
  memcpy(buf, svbytes+svh->pos, len-svh->pos);
  svh->pos = len;
  return FALSE;
}

gboolean gfsmperl_write_sv(gfsmPerlSVHandle *svh, const void *buf, size_t nbytes)
{
  if (!svh || !svh->sv) return FALSE;
  sv_catpvn(svh->sv, buf, (STRLEN)nbytes);
  return TRUE;
}
