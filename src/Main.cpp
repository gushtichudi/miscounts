#include <cstring>
#include <print>

#include <getopt.h>

#include "../include/Miscount.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr,
            "Usage: miscount -m <Miscount> -n <Name> -d <Description>\n");
    return -1;
  }

  Args *args = new Args;
  if (!args) {
    std::print("miscount: cannot allocate memory for arguments\n");
    return 1;
  }

  FurtherOptions *furtheroptions = new FurtherOptions;
  if (!furtheroptions) {
    std::print("miscount: cannot allocate memory for arguments\n");
    return 1;
  }

  int c;
  while ((c = getopt(argc, argv, "m:n:d:v")) != -1) {
    switch (c) {
    case 'm':
      args->nameOfMiscount = strdup(optarg);
      break;
    case 'n':
      args->nameOfOffender = strdup(optarg);
      break;
    case 'd':
      if (strcmp(optarg, "!EDITOR") == 0) {
        furtheroptions->writeDescriptionInEditor = true;
        break;
      }

      args->descriptionOfMiscount = strdup(optarg);
      furtheroptions->writeDescriptionInEditor = false;

      break;

    case 'v':
      std::print("{}\n", MISCOUNT_VERSION);

      delete args;
      delete furtheroptions;

      return 0;
    }
  }

  MiscountParams *miscountparams = new MiscountParams;
  if (!miscountparams) {
    std::print("miscount: cannot allocate memory for miscount\n");
    return 1;
  }

  miscountparams->a = args;
  miscountparams->b = furtheroptions;

  Miscount miscount(miscountparams);

  miscount.Init();
  if (miscount.AppendMiscount(miscountparams) != 0)
    return 1;

  delete args;
  delete furtheroptions;
  delete miscountparams;

  return 0;
}
