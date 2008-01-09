/*-*- Mode: C -*- */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include <gfsm.h>
#include "GfsmXLPerl.h"

#define GFSMXL_DEBUG 1

MODULE = Gfsm::XL		PACKAGE = Gfsm::XL

##=====================================================================
## Gfsm::XL (bootstrap)
##=====================================================================
BOOT:
 {
   //g_mem_set_vtable(&gfsm_perl_vtable); //-- shuld be done by Gfsm
   //gfsm_allocators_enable();
   //
#ifdef GFSMXL_DEBUG
   g_printerr("Gfsm::XL::BOOT() called.\n");
#endif
 } 

##=====================================================================
## Gfsm::XL (debug)
##=====================================================================

#ifdef GFSMXL_DEBUG

int __refcnt(SV *sv)
CODE:
 if (sv && SvOK(sv)) {
   RETVAL = SvREFCNT(sv);
 } else {
   XSRETURN_UNDEF;
 }
OUTPUT:
 RETVAL

#endif

##=====================================================================
## Gfsm::XL (global)
##=====================================================================

const char *
library_version()
CODE:
 RETVAL = PACKAGE_VERSION;
OUTPUT:
 RETVAL

##=====================================================================
## Gfsm::XL::Cascade
##=====================================================================
INCLUDE: Cascade.xs

##=====================================================================
## Gfsm::XL::Cascade::Lookup
##=====================================================================
INCLUDE: Lookup.xs
