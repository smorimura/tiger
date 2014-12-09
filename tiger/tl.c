/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tiger.h> /* tgState */
#include <topt/topt.h> /* tgOpt */
#include <stdio.h> /* printf */
#include <stdlib.h> /* exit */
#include <stdarg.h> /* va_start, va_end */

/*******************************************************************************
 * Error Codes
 ******************************************************************************/
enum tcErr_ {
  CERR_NONE,
  CERR_FOPEN_READ
};
typedef enum tcErr_ tcErr;

#define CERROR(e,...) tcerr(e, __VA_ARGS__)
#define CASSERT(b,e,...) if (!(b)) tcerr(e, __VA_ARGS__)

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
int help(tgState* T) {
  (void)T;
  printf("Under Construction\n");
  return 0;
}

int quit(tgState* T) {
  (void)T;
  fclose(stdin);
  return 0;
}

int say(tgState* T) {
  char const* msg = tgState_pop(T)->data.string;
  printf("%s\n", msg);
  return 0;
}

int sayInt(tgState* T) {
  tgInteger i = tgState_pop(T)->data.integer;
  printf("%d", i);
  return 0;
}

int sayNewline(tgState* T) {
  (void)T;
  printf("\n");
  return 0;
}

/*******************************************************************************
 * Option Callbacks
 ******************************************************************************/
static int print_help(tState *s) {
    tOpt_printUsage(s);
    printf(
           "For bug reporting instructions, please see:\n"
           "<http://bugs.libtiger.org/>\n"
           );
    exit(0);
    return 0;
}

static int print_version(tState *s) {
    (void)s;
    printf("TigerScript Version %d.%d.%d\n", tgVersionMajor, tgVersionMinor, tgVersionPatch);
    return 0;
}

static int err_invalid(tState *s) {
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

  // Create and initialize TigerScript
  tgState *T = tgState_create(tgMalloc, NULL);
  
  // Add helper functions for the end-user
  tgState_addCFunc(T, "help", &help);
  tgState_addCFunc(T, "quit", &quit);
  tgState_addCFunc(T, "say", &say);
  tgState_addCFunc(T, "sayInt", &sayInt);
  tgState_addCFunc(T, "sayNewline", &sayNewline);
  
  // Handle command line arguments.
  int argi = 0;
  TOPT_PARSE(argi, argc, argv, options) {
  case TERR_ARGUMENT:
    tgState_execFile(T, argv[argi]);
    break;
  default:
    printf("tl has encountered a fatal error!\n");
    exit(1);
  }

  // Clean up TigerScript
  tgState_free(T);

  return 0;
}
