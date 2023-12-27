#line 11 "gperf/section.gperf"

#include "tables/section_tbl.h"

#define uint8_t unsigned char

#define TOTAL_KEYWORDS 6
#define MIN_INTKEY 3
#define MAX_INTKEY 8
/* maximum key range = 655374, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
/*ARGSUSED*/
static unsigned int
hash (const uint8_t key)
{
  return (unsigned int)(key - 3);
}

static const struct section_entry wordlist[] =
  {
#line 16 "gperf/section.gperf"
    {3, "Windows"},
#line 17 "gperf/section.gperf"
    {4, "VERSION"},
#line 18 "gperf/section.gperf"
    {5, "OPTIONS"},
#line 19 "gperf/section.gperf"
    {6, "LOCALIZE"},
#line 20 "gperf/section.gperf"
    {7, "REGISTRATION"},
#line 21 "gperf/section.gperf"
    {8, "SCURK"}
  };

const struct section_entry *
section_lookup (const uint8_t num)
{
  if (num >= MIN_INTKEY && num <= MAX_INTKEY)
    {
      return &wordlist[hash (num)];
    }
  return 0;
}
