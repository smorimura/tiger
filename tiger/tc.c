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
#include <tiger/topt.h> /* tgOpt */

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
tgOpt const *options_;

#define CERROR(e,...) tcerr(e, __VA_ARGS__)
#define CASSERT(b,e,...) if (!(b)) tcerr(e, __VA_ARGS__)

/*******************************************************************************
 * Global Properties
 ******************************************************************************/
static char const* arg0 = NULL;
static char const* in   = NULL;
static char const* out  = NULL;
static int pnonterm     = 0;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
static void tcerr(tcErr e, char const* fmt, ...) {
  printf("%s: Fatal Error: ", arg0);
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
  exit(e);
}

static void print_usage() {
  printf("Usage: %s [OPTION]... [FILE]...\n", arg0);
}

static void print_usageTry() {
  print_usage();
  printf("Try '%s --help' for more information.\n", arg0);
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
static void print_help(int *argc, char const* argv[]) {
  (void)argc; (void)argv;
  print_usage();
  tgOpt_fprint(stdout, options_);
  printf(
    "For bug reporting instructions, please see:\n"
    "<http://bugs.libtiger.org/>\n"
  );
  exit(0);
}

static void set_compile(int *argc, char const* argv[]) {
  in = argv[++*argc];
}

static void set_output(int *argc, char const* argv[]) {
  out = argv[++*argc];
}

static void print_nonterm(int *argc, char const* argv[]) {
  (void)argc; (void)argv;
  pnonterm = 1;
}

static void err_invalid(int *argc, char const* argv[]) {
  printf("%s: Unrecognized Option: '%s'\n", arg0, argv[*argc]);
}

/*******************************************************************************
 * Options Definitions
 ******************************************************************************/
static tgOpt options[] = {
  { 'h',          "help",    &print_help, "Displays this help information." },
  { 'c',       "compile",   &set_compile, "Specifies one particular compile file." },
  { 'o',        "output",    &set_output, "Specifies on particular output file. (Only works with -c)" },
  { 't', "print-nonterm", &print_nonterm, "Prints the nonterminal symbols as they are read in from a file." },
  {  0 ,            NULL,   &err_invalid, NULL }
};

/*******************************************************************************
 * Main
 ******************************************************************************/
int main(int argc, char const* argv[]) {
  options_ = options;
  arg0 = argv[0];

  int filesIdx = tgOpt_parse(argc, argv, options);
  tgState *T = tgState_create(&tgMalloc, NULL);

  // Check for specific file handling.
  if (in) {
    writeCode(T, in, out);
  }
  else if (filesIdx >= argc) {
    print_usageTry(argv);
    CERROR(CERR_NO_INPUT, "No input files.\n");
  }

  // Compile files
  while (filesIdx < argc) {
    writeCode(T, argv[filesIdx], NULL);
    ++filesIdx;
  };

  tgState_free(T);

  return 0;
}
