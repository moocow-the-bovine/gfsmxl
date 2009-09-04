#!/usr/bin/perl -w

my @MACROS =
  qw(
     CASCADE_USE_RAW_BSEARCH
     CASCADE_USE_BLOCKS
     CASCADE_USE_BLOCK_HASH
     CASCADE_USE_BLOCK_INDEX
     CASCADE_EXPAND_BLOCK_BSEARCH
     CASCADE_EXPAND_RECURSIVE
     CASCADE_SORT_ARCITER
     GFSM_INLINE_ENABLED
     GFSMXL_CLC_FH_STATS
     GFSMXL_DEBUG_ENABLED
   );


print "
#include <gfsmxl.h>
#define STR(x) #x
#define VAL(x) STR(x)
int main(void) {
";

foreach (@MACROS) {
  print qq{
//-- $_
#ifdef $_
printf("#define $_ " VAL($_) "\\n");
#else
printf("#undef $_\\n");
#endif
};
}

print "  return 0;
}
";
