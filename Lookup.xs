#/*-*- Mode: C -*- */

MODULE = Gfsm::XL	PACKAGE = Gfsm::XL::Cascade::Lookup       PREFIX = gfsmxl_cascade_lookup_

##-- disable perl prototypes
PROTOTYPES: DISABLE

##=====================================================================
## Constructors etc.
##=====================================================================

##--------------------------------------------------------------
## Constructor: new()
gfsmxlCascadeLookupPerl*
new(char *CLASS, SV *csc_sv, gfsmWeight max_w=0, guint max_paths=1, guint max_ops=-1)
CODE:
 RETVAL = gfsmxl_perl_cascade_lookup_new(csc_sv, max_w, max_paths, max_ops);
 g_printerr("Gfsm::XL::Cascade::Lookup::new(): returning clp=%p, clp->cl=%p, clp->csc_sv=%p; clp->cl->csc=%p\n", RETVAL, RETVAL->cl, RETVAL->csc_sv, RETVAL->cl->csc);
OUTPUT:
 RETVAL

##--------------------------------------------------------------
## Destructor: DESTROY()
void
DESTROY(gfsmxlCascadeLookupPerl* clp)
CODE:
 g_printerr("Gfsm::XL::Cascade::Lookup::DESTROY(clp=%p : clp->cl=%p / csc_sv=%p, csc=%p)\n", clp, clp->cl, clp->csc_sv, clp->cl->csc);
 if (clp) gfsmxl_perl_cascade_lookup_free(clp);
 g_blow_chunks();


##=====================================================================
## High-level Access
##=====================================================================

SV*
cascade(gfsmxlCascadeLookupPerl *clp, ...)
CODE:
 g_printerr("cascade(): items=%d\n", items);
 if (items > 1) {
   gfsmxl_perl_cascade_lookup_set_cascade_sv(clp,ST(1));
 }
 if (clp->csc_sv) {
   RETVAL = sv_mortalcopy(clp->csc_sv);
   SvREFCNT_inc(RETVAL);
 }
 else { XSRETURN_UNDEF; }
OUTPUT:
 RETVAL

SV*
_cascade_get(gfsmxlCascadeLookupPerl *clp)
CODE:
 RETVAL = sv_mortalcopy(clp->csc_sv);
 SvREFCNT_inc(RETVAL);
OUTPUT:
 RETVAL

void
_cascade_set(gfsmxlCascadeLookupPerl *clp, SV *cascade_sv)
CODE:
 gfsmxl_perl_cascade_lookup_set_cascade_sv(clp,cascade_sv);

##=====================================================================
## Component Access
##=====================================================================
