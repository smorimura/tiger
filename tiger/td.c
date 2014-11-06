/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tiger.h>
#include <tiger/topt.h>
#include <stdarg.h> /* va_start, va_end */
#include <stdlib.h> /* exit */

/*******************************************************************************
 * Error Codes
 ******************************************************************************/
enum tdErr_ {
  CERR_NONE,
  CERR_NO_INPUT,
  CERR_FOPEN_READ,
  CERR_FOPEN_WRITE
};
typedef enum tdErr_ tdErr;

#define CERROR(e,...) tcerr(e, __VA_ARGS__)
#define CASSERT(b,e,...) if (!(b)) tcerr(e, __VA_ARGS__)

/*******************************************************************************
 * Global Properties
 ******************************************************************************/
static tgOpt const *options_ = NULL;
static char const* arg0 = NULL;
static char const* in   = NULL;
static char const* out  = NULL;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
static void print_usage() {
  printf("Usage: %s [OPTION]... [FILE]...\n", arg0);
}

#define SC(t) case t: fprintf(out, "%s\n", #t); break
#define JMP(t) case t: fprintf(out, "%s %d\n", #t, fgetc(in)); break
static void disassembleFile(FILE* in, FILE* out) {
  int i = 0;
  fprintf(out, ";; Begin Constants Table\n");
  for (;;) {
    int ct = fgetc(in);
    switch (ct) {
      case TGK_INTEGER:
      {
        tgInteger val;
        fread(&val, sizeof(tgInteger), 1, in);
        fprintf(out, "K(%d) : Integer  = %d\n", i, val);
      }
        break;
      case TGK_REAL:
      {
        tgReal val;
        fread(&val, sizeof(tgReal), 1, in);
        fprintf(out, "K(%d) : Real    = %f\n", i, val);
      }
        break;
      case TGK_STRING:
      {
        char val[4096];
        char *curr = val;
        do {
          *curr = (char)fgetc(in);
        } while (*(curr++) != 0);
        fprintf(out, "K(%d) : String   = \"%s\"\n", i, val);
      }
        break;
      case 0xFF:
        i = -1;
        break;
    }
    if (i == -1) break;
    ++i;
  }
  fprintf(out, ";; End Constants Table\n\n");
  fprintf(out, ";; Begin Bytecode\n");
  while (!feof(in)) {
    switch (fgetc(in)) {
      JMP(TB_PUSH);
      JMP(TB_PUSHK);
      JMP(TB_PUSHR);
      JMP(TB_JIF);
      JMP(TB_JIT);
      JMP(TB_JMP);
      JMP(TB_RJMP);
      JMP(TB_CALL);
      SC(TB_ADD);
      SC(TB_SUB);
      SC(TB_MUL);
      SC(TB_DIV);
      SC(TB_ASSIGN);
      SC(TB_EQ);
      SC(TB_LT);
      SC(TB_LE);
      SC(TB_GT);
      SC(TB_GE);
      case EOF:
        break;
      default:
        fprintf(out, "<< ERROR >>\n");
        break;
    }
  }
  fprintf(out, ";; End Bytecode\n");
}

static void disassemble(char const* path) {
  FILE* f = fopen(path, "rb");
  FILE* fout = fopen(out, "ww");
  if (f) {
    disassembleFile(f, fout);
    fclose(f);
  }
}

/*******************************************************************************
 * Options Callbacks
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

static void err_invalid(int *argc, char const* argv[]) {
  printf("%s: Unrecognized Option: '%s'\n", arg0, argv[*argc]);
}

/*******************************************************************************
 * Options Definitions
 ******************************************************************************/
static tgOpt const options[] = {
  { 'h', "help",        &print_help,  "Displays this help information." },
  { 'd', "disassemble", &set_compile, "Specifies one particular binary file to disassemble." },
  { 'o', "output",      &set_output,  "Specifies on particular output file. (Only works with -d)" },
  { 0, NULL, &err_invalid, NULL }
};

/*******************************************************************************
 * Main
 ******************************************************************************/
int main(int argc, char const* argv[]) {
  options_ = options;
  tgOpt_parse(argc, argv, options);

  if (in) {
    disassemble(in);
  }
  return 0;
}
