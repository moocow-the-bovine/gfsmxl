
#include <gfsmxl.h>
#define STR(x) #x
#define VAL(x) STR(x)
int main(void) {

//-- CASCADE_USE_RAW_BSEARCH
#ifdef CASCADE_USE_RAW_BSEARCH
printf("#define CASCADE_USE_RAW_BSEARCH " VAL(CASCADE_USE_RAW_BSEARCH) "\n");
#else
printf("#undef CASCADE_USE_RAW_BSEARCH\n");
#endif

//-- CASCADE_USE_BLOCKS
#ifdef CASCADE_USE_BLOCKS
printf("#define CASCADE_USE_BLOCKS " VAL(CASCADE_USE_BLOCKS) "\n");
#else
printf("#undef CASCADE_USE_BLOCKS\n");
#endif

//-- CASCADE_USE_BLOCK_HASH
#ifdef CASCADE_USE_BLOCK_HASH
printf("#define CASCADE_USE_BLOCK_HASH " VAL(CASCADE_USE_BLOCK_HASH) "\n");
#else
printf("#undef CASCADE_USE_BLOCK_HASH\n");
#endif

//-- CASCADE_USE_SUFFIX_INDEX
#ifdef CASCADE_USE_SUFFIX_INDEX
printf("#define CASCADE_USE_SUFFIX_INDEX " VAL(CASCADE_USE_SUFFIX_INDEX) "\n");
#else
printf("#undef CASCADE_USE_SUFFIX_INDEX\n");
#endif

//-- CASCADE_USE_BLOCK_INDEX
#ifdef CASCADE_USE_BLOCK_INDEX
printf("#define CASCADE_USE_BLOCK_INDEX " VAL(CASCADE_USE_BLOCK_INDEX) "\n");
#else
printf("#undef CASCADE_USE_BLOCK_INDEX\n");
#endif

//-- CASCADE_EXPAND_BLOCK_BSEARCH
#ifdef CASCADE_EXPAND_BLOCK_BSEARCH
printf("#define CASCADE_EXPAND_BLOCK_BSEARCH " VAL(CASCADE_EXPAND_BLOCK_BSEARCH) "\n");
#else
printf("#undef CASCADE_EXPAND_BLOCK_BSEARCH\n");
#endif

//-- CASCADE_SORT_ARCITER
#ifdef CASCADE_SORT_ARCITER
printf("#define CASCADE_SORT_ARCITER " VAL(CASCADE_SORT_ARCITER) "\n");
#else
printf("#undef CASCADE_SORT_ARCITER\n");
#endif

//-- GFSM_INLINE_ENABLED
#ifdef GFSM_INLINE_ENABLED
printf("#define GFSM_INLINE_ENABLED " VAL(GFSM_INLINE_ENABLED) "\n");
#else
printf("#undef GFSM_INLINE_ENABLED\n");
#endif

//-- GFSMXL_CLC_FH_STATS
#ifdef GFSMXL_CLC_FH_STATS
printf("#define GFSMXL_CLC_FH_STATS " VAL(GFSMXL_CLC_FH_STATS) "\n");
#else
printf("#undef GFSMXL_CLC_FH_STATS\n");
#endif

//-- GFSMXL_DEBUG_ENABLED
#ifdef GFSMXL_DEBUG_ENABLED
printf("#define GFSMXL_DEBUG_ENABLED " VAL(GFSMXL_DEBUG_ENABLED) "\n");
#else
printf("#undef GFSMXL_DEBUG_ENABLED\n");
#endif
  return 0;
}
