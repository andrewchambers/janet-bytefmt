#include <janet.h>
#include <stdlib.h>
#include <string.h>
#if !defined(JANET_BSD) && !defined(JANET_WINDOWS)
#include <alloca.h>
#endif
#ifdef JANET_WINDOWS
#include <malloc.h>
#define alloca _alloca
#endif

static Janet format_byte_size(char **sfx, double div, double size) {
  if (size < 0)
    janet_panicf("expected a positive number");
  int i = 0;
  if (size >= div)
    for (; (size / div) >= 1 && sfx[i+1]; i++)
      size = size / div;
  char *fmt;
  if (i > 0)
    fmt = "%.02lf%s";
  else 
    fmt = "%.0lf%s";
  int sz = snprintf(NULL, 0, fmt, size, sfx[i]);
  if (sz < 0)
    janet_panicf("unable to format byte size");
  uint8_t *buf = alloca(sz+1);
  sprintf((char*)buf, fmt, size, sfx[i]);
  return janet_stringv(buf, sz);
}

static Janet si_format_byte_size(int argc, Janet *argv) {
  janet_fixarity(argc, 1);
  double size = janet_getnumber(argv, 0);
  char *sfx[] = {"B", "kB", "MB", "GB", "TB", NULL};
  return format_byte_size(sfx, 1000, size);
}

static Janet binary_format_byte_size(int argc, Janet *argv) {
  janet_fixarity(argc, 1);
  double size = janet_getnumber(argv, 0);
  char *sfx[] = {"B", "KiB", "MiB", "GiB", "TiB", NULL};
  return format_byte_size(sfx, 1024, size);
}

static const JanetReg cfuns[] = {
    {"si-format-byte-size", si_format_byte_size,
     "(bytefmt/si-format-byte-size sz)\n\nFormat byte size in SI bytes."},
    {"binary-format-byte-size", binary_format_byte_size, 
     "(bytefmt/binary-format-byte-size sz)\n\nFormat byte size in binary units."},
    {NULL, NULL, NULL}};

JANET_MODULE_ENTRY(JanetTable *env) { janet_cfuns(env, "bytefmt", cfuns); }
