/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/tiger.h> /* tgState */
#include <tiger/topt.h> /* tgOpt */
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
 * Forward Declarations
 ******************************************************************************/
static tgOpt options[];

/*******************************************************************************
 * Global Properties
 ******************************************************************************/
static char const* arg0 = NULL;
static char const* file = NULL;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
static void tlerr(tcErr e, char const* fmt, ...) {
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


int help(tgState* T) {
  printf("Under Construction\n");
  return 0;
}

int quit(tgState* T) {
  fclose(stdin);
  return 0;
}

int say(tgState* T) {
  char const* msg = tgState_pop(T)->data.string;
  printf("%s\n", msg);
  return 0;
}

int printInt(tgState* T) {
  tgInteger i = tgState_pop(T)->data.integer;
  printf("%d\n", i);
  return 0;
}

/*******************************************************************************
 * Option Callbacks
 ******************************************************************************/
static void print_help(int *argc, char const* argv[]) {
  print_usage();
  tgOpt_fprint(stdout, options);
  printf(
    "For bug reporting instructions, please see:\n"
    "<http://bugs.libtiger.org/>\n"
  );
  exit(0);
}

static void set_file(int *argc, char const* argv[]) {
  file = argv[++*argc];
}

static void err_invalid(int *argc, char const* argv[]) {
  printf("%s: Unrecognized Option: '%s'\n", arg0, argv[*argc]);
}

/*******************************************************************************
 * Options Definitions
 ******************************************************************************/
static tgOpt options[] = {
  { 'h', "help", &print_help, "Displays this help information." },
  { 'f', "file", &set_file, "Runs a TigerScript file."},
  { 0, NULL, &err_invalid, NULL }
};

/*******************************************************************************
 * Main
 ******************************************************************************/
int main(int argc, char const* argv[]) {

  // Create and initialize TigerScript
  tgState *T = tgState_create(tgMalloc, NULL);

  // Handle command-line options
  tgOpt_parse(argc, argv, options);

  // Add helper functions for the end-user
  tgState_addCFunc(T, "help", &help);
  tgState_addCFunc(T, "quit", &quit);
  tgState_addCFunc(T, "say", &say);
  tgState_addCFunc(T, "printInt", &printInt);

  if (file) {
    tgState_execFile(T, file);
  }
  /*
  while (!feof(file)) {
    printf("> ");
    tgState_exec(T, io);
  }
  */

  // Clean up TigerScript
  tgState_free(T);

  return 0;
}
