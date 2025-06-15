#include "../include/Miscount.h"

static inline const char *getUserHomeDir() {
#ifdef __WIN32
  char homeDir[MAX_PATH];
  if (GetEnvironmentVariable("USERPROFILE", homeDir, MAX_PATH)) {
    return homeDir;
  } else {
    fprintf(stderr, "miscount: cannot get home directory\n");
    exit(-1);
  }
#else
  return getenv("HOME");
#endif
}

static std::string buildMiscountPath() {
  const char *home = getUserHomeDir();
  auto ret = std::filesystem::path(home) / std::filesystem::path("Documents") /
             std::filesystem::path("miscount.csv");

  // make sure `home` isn't optimized out, otherwise keep trying for 10 tries
  // a notable problem is that `auto ret` turns out `home` into `miscount.csv`
  // this just checks for that.
  int tries = 10;
  while (strcmp(home, "miscount.csv") == 0) {
    fprintf(
        stderr,
        "miscount: warning: compiler deliquency detected, trying again...\n");
    tries--;

    buildMiscountPath();
  }

  return ret.string();
}

static inline bool miscountPathExists() {
  return std::filesystem::exists(buildMiscountPath());
}

static int mkMiscountPath() {
  std::filesystem::path miscountPath = buildMiscountPath();

  std::ofstream mout(miscountPath);
  if (!mout) {
    std::print("miscount: cannot make {}: {}\n", miscountPath.string(),
               strerror(errno));

    exit(-1);
  }

  mout << "DateTime,Miscount,Name,Description" << std::endl;
  mout.close();

  return 0;
}

static std::string inferGoodEditor() {
  if (getenv("EDITOR") == NULL) {
#ifdef __WIN32
    fprintf(stderr, "It appears that you're on a Windows-like environment "
                    "without an $EDITOR variable set.\n");
    fprintf(stderr, "We'll set the default editor to Notepad for you.\n");

    if (_putenv("EDITOR=notepad") != 0) {
      fprintf(stderr, "miscount: cannot add environment variable: %s\n",
              strerror(errno));
      exit(-1);
    }

    return "notepad";
#else
    return "nano";
#endif
  }

  return std::string(getenv("EDITOR"));
}

// thanks, Ingmar!
static inline void replaceAll(std::string &source, const std::string &from,
                              const std::string &to) {
  std::string newString;
  newString.reserve(source.length()); // avoids a few memory allocations

  std::string::size_type lastPos = 0;
  std::string::size_type findPos;

  while (std::string::npos != (findPos = source.find(from, lastPos))) {
    newString.append(source, lastPos, findPos - lastPos);
    newString += to;
    lastPos = findPos + from.length();
  }

  // Care for the rest after last occurrence
  newString += source.substr(lastPos);

  source.swap(newString);
}

void Miscount::Init() {
  if (!miscountPathExists()) {
    std::print("miscount: {} doesn't exist; making\n", buildMiscountPath());

    mkMiscountPath();
  }
}

int Miscount::AppendMiscount(MiscountParams *m) {
  std::ofstream miscountFile(buildMiscountPath(), std::ios::app);
  if (!miscountFile) {
    std::print("miscount: cannot open {}: {}\n", buildMiscountPath(),
               strerror(errno));

    return 1;
  }

  // date time
  auto time =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  miscountFile << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
               << ",";

  // miscount
  auto nom = std::string(m->a->nameOfMiscount);
  replaceAll(nom, ",", " AND/OR ");

  miscountFile << nom << ",";

  // name of offender
  auto noo = std::string(m->a->nameOfOffender);
  replaceAll(noo, ",", " AND/OR ");
  miscountFile << noo << ",";

  // description of miscount

  auto miscountDescription = std::string(m->a->descriptionOfMiscount);

  // first, sanitize description
  //	includes stuff in normal grammar that might intefere
  //	with the way how programs parse CSV

  replaceAll(miscountDescription, ",", " AND/OR ");

  if (m->b->writeDescriptionInEditor == false) {
    miscountFile << miscountDescription << std::endl;
  } else {
    // fuck you
    if (system(
            std::format("{} {}", inferGoodEditor(), ".miscount_tmp").c_str()) !=
        0)
      return -1;

    std::ifstream tmp(".miscount_tmp");
    if (!tmp) {
      fprintf(stderr, "miscount: cannot open temp file\n");
      return 1;
    }

    std::ostringstream buffer;
    buffer << tmp.rdbuf();
    std::string description = buffer.str();

    miscountFile << description << std::endl;

    // delete any temp files
    if (std::remove(".miscount_tmp") != 0)
      fprintf(stderr, "miscount: warning: tried to delete temp file but it "
                      "might not exist anymore\nn");
  }

  return 0;
}
