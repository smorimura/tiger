/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h> /* printf */
#include <stdlib.h> /* exit */
#include <stdarg.h> /* va_start, va_end */
#include <tiger/tiger.h> /* tgState */
#include <topt/topt.h> /* tgOpt */

/*******************************************************************************
 * Error Codes
 ******************************************************************************/
enum tcErr_ {
  CERR_NONE,
  CERR_NO_INPUT,
  CERR_FOPEN_READ,
  CERR_FOPEN_WRITE
};
typedef enum tcErr_ tcErr;

#define CERROR(e,...) tcerr(e, __VA_ARGS__)
#define CASSERT(b,e,...) if (!(b)) tcerr(e, __VA_ARGS__)

static void tcerr(tcErr e, char const* fmt, ...) {
  printf("Fatal Error: ");
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
  exit(e);
}

static char const* buildOPath(char const* path) {
  // We never expect more than one file to be processed at a time.
  static char buffer[4096];

  // "file.tg" -> "file" -> "file.to"
  char const * pend = strrchr(path, '.');
  size_t off = pend - path;
  strncpy(buffer, path, off);
  strcpy(buffer + off, ".to");

  // "file.to"
  return buffer;
}

static void writeCode(tgState* T, char const* path, char const* opath) {
  tgCode *code = tgState_compileFile(T, path);
  if (code) {
    if (!opath) {
      opath = buildOPath(path);
    }
    FILE *f = fopen(opath, "wb");FILE *f2 = fopen("BINARY.OUT", "ww");
    CASSERT(f, CERR_FOPEN_WRITE, "%s: Could not open file or directory.\n", opath);
    tgCode_fprintb(f, code);
    tgCode_fprint(f2, code);
    fclose(f);fclose(f2);
  }
  else {
    CERROR(CERR_FOPEN_READ, "%s: No such file or directory.\n", path);
    exit(-2);
  }
}

/*******************************************************************************
 * Option Callbacks
 ******************************************************************************/
static tErr print_help(tState *s) {
  tOpt_printUsage(s, 80);
  printf(
    "For bug reporting instructions, please see:\n"
    "<http://bugs.libtiger.org/>\n"
  );
  exit(0);
  return 0;
}

static tErr print_version(tState *s) {
  (void)s;
  printf("TigerScript Version %d.%d.%d\n", tgVersionMajor, tgVersionMinor, tgVersionPatch);
  return 0;
}

static tErr err_invalid(tState *s) {
  (void)s;
  printf("%s: Unrecognized Option: '%s'\n", s->argv[0], s->argv[s->argi]);
  return 0;
}

/*******************************************************************************
 * Options Definitions
 ******************************************************************************/
static tOption const options[] = {
  { 'h',    "help",    &print_help, "Displays this help information." },
  { 'v', "version", &print_version, "Displays TigerScript version information."},
  {  0 ,      NULL,   &err_invalid, NULL }
};

/*******************************************************************************
 * Main
 ******************************************************************************/
int main(int argc, char const* argv[]) {

  int argi = 0;
  tgState *T = tgState_create(&tgMalloc, NULL);
  while (tOpt_parse(&argi, argc, argv, options)) {
    // Not an option, assume it's a file.
    writeCode(T, argv[argi++], NULL);
  }
  tgState_free(T);

  return 0;
}
