#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 10 "gperf/reg.gperf"

#include "tables/reg_tbl.h"
#include "reg_keys.h"

#define TOTAL_KEYWORDS 12
#define MIN_WORD_LENGTH 5
#define MAX_WORD_LENGTH 12
#define MIN_HASH_VALUE 5
#define MAX_HASH_VALUE 27
/* maximum key range = 23, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (const char *str, size_t len)
{
  static const unsigned char asso_values[] =
    {
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28,  5, 28, 28,
      28, 15, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28,  4, 28,  5, 15, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28,  0,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 10,
      10, 28, 28, 28,  0, 28, 10, 28, 28, 28,
       0, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28
    };
  return len + asso_values[(unsigned char)str[2]];
}

static const struct reg_entry wordlist[] =
  {
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 16 "gperf/reg.gperf"
    {"Maxis", MAXIS_KEY},
    {(char*)0},
#line 22 "gperf/reg.gperf"
    {"Version", VERSION_KEY},
#line 25 "gperf/reg.gperf"
    {"Localize", LOCALIZE_KEY},
    {(char*)0},
#line 18 "gperf/reg.gperf"
    {"PATHS", PATHS_KEY},
#line 21 "gperf/reg.gperf"
    {"VERSION", VERSION_KEY},
#line 23 "gperf/reg.gperf"
    {"OPTIONS", OPTIONS_KEY},
#line 24 "gperf/reg.gperf"
    {"LOCALIZE", LOCALIZE_KEY},
    {(char*)0},
#line 19 "gperf/reg.gperf"
    {"Paths", PATHS_KEY},
    {(char*)0},
#line 20 "gperf/reg.gperf"
    {"Windows", WINDOWS_KEY},
    {(char*)0}, {(char*)0},
#line 27 "gperf/reg.gperf"
    {"SCURK", SCURK_KEY},
    {(char*)0},
#line 17 "gperf/reg.gperf"
    {"SimCity 2000", SC2K_KEY},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 26 "gperf/reg.gperf"
    {"REGISTRATION", REGISTRATION_KEY}
  };

const struct reg_entry *
reg_lookup (const char *str, size_t len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          const char *s = wordlist[key].key;

          if (s && *str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
