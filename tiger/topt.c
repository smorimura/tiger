/*******************************************************************************
 * TigerScript - All content 2014 Trent Reed, all rights reserved.
 ******************************************************************************/

#include <tiger/topt.h>
#include <string.h> /* strcmp */

typedef int (*tgOptChk)(tgOpt const *opt, char const *arg);

static int tgOptChk_short(tgOpt const *opt, char const *arg) {
  return (opt->shortopt == *arg);
}

static int tgOptChk_long(tgOpt const *opt, char const *arg) {
  return (strcmp(opt->longopt, arg) == 0);
}

static int tgOpt_nil(tgOpt const *opt) {
  return (opt->longopt == NULL);
}

static int tgOpt_checkOpt(char const **arg) {
  if ((*arg)[0] == '-') {
    ++(*arg);
    return 1;
  }
  return 0;
}

static int tgOpt_processOpt(int *pargc, char const* argv[], char const *arg, tgOpt const *opts, tgOptChk check) {
  // Check each existing option.
  while (!tgOpt_nil(opts)) {
    if (check(opts, arg)) {
      opts->callback(pargc, argv);
      return 1;
    }
    ++opts;
  }

  // See if there is a fallback function.
  if (opts->callback) {
    opts->callback(pargc, argv);
    return 1;
  }

  // Otherwise, unhandled
  return 0;
}

static int tgOpt_shortOpt(int *pargc, char const* argv[], char const* arg, tgOpt const *opts) {
  tgOpt_processOpt(pargc, argv, arg, opts, &tgOptChk_short);
}

static int tgOpt_longOpt(int *pargc, char const* argv[], char const* arg, tgOpt const *opts) {
  tgOpt_processOpt(pargc, argv, arg, opts, &tgOptChk_long);
}

int tgOpt_proccess(int argc, char const* argv[], int *idx, tgOpt const opts[]) {
  char const* currOpt = argv[*idx];

  // Check if long option
  if (tgOpt_checkOpt(&currOpt)) {
    // Check if short option
    if (tgOpt_checkOpt(&currOpt)) {
      return tgOpt_longOpt(idx, argv, currOpt, opts);
    } else {
      return tgOpt_shortOpt(idx, argv, currOpt, opts);
    }
  }

  // Argument was not an option.
  return 0;
}

static int tgOpt_getLongest(tgOpt const *opts) {
  int curr;
  int length = 0;
  while (!tgOpt_nil(opts)) {
    curr = strlen(opts->longopt);
    if (curr > length)
      length = curr;
    ++opts;
  }
  return length;
}

static void tgOpt_fprintOpt(FILE *f, tgOpt const *opt, int width) {
  if (opt->shortopt != 0)
    fprintf(f, "  -%c, ", opt->shortopt);
  else
    fprintf(f, "      ");
  fprintf(f, "--%-*s %s\n", width, opt->longopt, opt->description);
}

void tgOpt_fprint(FILE *f, tgOpt const opts[]) {
  int spacing = tgOpt_getLongest(opts);

  // Print Options list
  fprintf(f, "Options:\n");
  while (!tgOpt_nil(opts)) {
    tgOpt_fprintOpt(f, opts, spacing);
    ++opts;
  }
}
