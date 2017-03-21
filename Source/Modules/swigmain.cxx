/* ----------------------------------------------------------------------------- 
 * This file is part of SWIG, which is licensed as a whole under version 3 
 * (or any later version) of the GNU General Public License. Some additional
 * terms also apply to certain portions of SWIG. The full details of the SWIG
 * license and copyrights can be found in the LICENSE and COPYRIGHT files
 * included with the SWIG source code as distributed by the SWIG developers
 * and at http://www.swig.org/legal.html.
 *
 * swigmain.cxx
 *
 * Simplified Wrapper and Interface Generator  (SWIG)
 *
 * This file is the main entry point to SWIG.  It collects the command
 * line options, registers built-in language modules, and instantiates
 * a module for code generation.   If adding new language modules
 * to SWIG, you would modify this file.
 * ----------------------------------------------------------------------------- */

#include "swigmod.h"
#include <ctype.h>

/* Module factories.  These functions are used to instantiate
   the built-in language modules.    If adding a new language
   module to SWIG, place a similar function here. Make sure
   the function has "C" linkage.  This is required so that modules
   can be dynamically loaded in future versions. */

extern "C" {
  Language *swig_python(void);
  Language *swig_xml(void);
}

struct swig_module {
  const char *name;
  ModuleFactory fac;
  const char *help;
};

/* Association of command line options to language modules.
   Place an entry for new language modules here, keeping the
   list sorted alphabetically. */

static swig_module modules[] = {
  {"-python", swig_python, "Python"},
  {"-xml", swig_xml, "XML"},
  {NULL, NULL, NULL}
};

#ifdef MACSWIG
#include <console.h>
#include <SIOUX.h>
#endif

#ifndef SWIG_LANG
#define SWIG_LANG "-python"
#endif

//-----------------------------------------------------------------
// main()
//
// Main program.    Initializes the files and starts the parser.
//-----------------------------------------------------------------

void SWIG_merge_envopt(const char *env, int oargc, char *oargv[], int *nargc, char ***nargv) {
  if (!env) {
    *nargc = oargc;
    *nargv = oargv;
    return;
  }

  int argc = 1;
  int arge = oargc + 1024;
  char **argv = (char **) malloc(sizeof(char *) * (arge));
  char *buffer = (char *) malloc(2048);
  char *b = buffer;
  char *be = b + 1023;
  const char *c = env;
  while ((b != be) && *c && (argc < arge)) {
    while (isspace(*c) && *c)
      ++c;
    if (*c) {
      argv[argc] = b;
      ++argc;
    }
    while ((b != be) && *c && !isspace(*c)) {
      *(b++) = *(c++);
    }
    *b++ = 0;
  }

  argv[0] = oargv[0];
  for (int i = 1; (i < oargc) && (argc < arge); ++i, ++argc) {
    argv[argc] = oargv[i];
  }

  *nargc = argc;
  *nargv = argv;
}

int main(int margc, char **margv) {
  int i;
  Language *dl = 0;
  ModuleFactory fac = 0;

  int argc;
  char **argv;

  SWIG_merge_envopt(getenv("SWIG_FEATURES"), margc, margv, &argc, &argv);

#ifdef MACSWIG
  SIOUXSettings.asktosaveonclose = false;
  argc = ccommand(&argv);
#endif

  /* Register built-in modules */
  for (i = 0; modules[i].name; i++) {
    Swig_register_module(modules[i].name, modules[i].fac);
  }

  Swig_init_args(argc, argv);

  /* Get options */
  for (i = 1; i < argc; i++) {
    if (argv[i]) {
      fac = Swig_find_module(argv[i]);
      if (fac) {
	dl = (fac) ();
	Swig_mark_arg(i);
      } else if (strcmp(argv[i], "-nolang") == 0) {
	dl = new Language;
	Swig_mark_arg(i);
      } else if ((strcmp(argv[i], "-help") == 0) || (strcmp(argv[i], "--help") == 0)) {
	if (strcmp(argv[i], "--help") == 0)
	  strcpy(argv[i], "-help");
	Printf(stdout, "Target Language Options\n");
	for (int j = 0; modules[j].name; j++) {
	  if (modules[j].help) {
	    Printf(stdout, "     %-15s - Generate %s wrappers\n", modules[j].name, modules[j].help);
	  }
	}
	// Swig_mark_arg not called as the general -help options also need to be displayed later on
      }
    }
  }
  if (!dl) {
    fac = Swig_find_module(SWIG_LANG);
    if (fac) {
      dl = (fac) ();
    }
  }

  int res = SWIG_main(argc, argv, dl);

  return res;
}
