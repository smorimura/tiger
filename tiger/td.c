/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tiger.h>
#include <topt/topt.h>
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
static char const* in   = NULL;
static char const* out  = NULL;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
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
    if (i == EOF) break;
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
    fclose(fout);
  }
}

/*******************************************************************************
 * Options Callbacks
 ******************************************************************************/
static int print_help(tState *T) {
  tOpt_printHelp(T);
  printf(
    "For bug reporting instructions, please see:\n"
    "<http://bugs.libtiger.org/>\n"
  );
  exit(0);
}

static int set_compile(tState *T) {
  in = T->argv[++T->argi];
  return 0;
}

static int set_output(tState *T) {
  out = T->argv[++T->argi];
  return 0;
}

/*******************************************************************************
 * Options Definitions
 ******************************************************************************/
static tOption const options[] = {
  { 'h', "help",        &print_help,  "Displays this help information." },
  { 'i', "in",         &set_compile, "Specifies one particular binary file to disassemble." },
  { 'o', "out",         &set_output,  "Specifies on particular output file. (Only works with -d)" },
  { 0, NULL, NULL, NULL }
};

/*******************************************************************************
 * Main
 ******************************************************************************/
int main(int argc, char const* argv[]) {
  
  // Process input files
  int argi = 0;
  TOPT_PARSE(argi, argc, argv, options) {
  default:
    printf("td encountered a fatal error!\n");
    exit(1);
  }

  // Print output files
  if (in) {
    disassemble(in);
  }
  
  return 0;
}
