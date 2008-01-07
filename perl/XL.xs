/*-*- Mode: C -*- */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include <gfsm.h>
#include "GfsmPerl.h"

MODULE = Gfsm::XL		PACKAGE = Gfsm::XL

##=====================================================================
## Gfsm::XL (bootstrap)
##=====================================================================
BOOT:
 {
   gfsmPerlAlphabet *a=NULL;
   //
   //g_mem_set_vtable(&gfsm_perl_vtable); //-- shuld be done by Gfsm
   //gfsm_allocators_enable();
   //
   g_printerr("Gfsm::XL::BOOT() called.\n");
   a = (gfsmPerlAlphabet*)gfsm_perl_alphabet_new();
   g_printerr("new alphabet a=%p\n", a);
   gfsm_perl_alphabet_free(a);
 } 

##=====================================================================
## Whatever
##=====================================================================
#INCLUDE: Whatever.xs
