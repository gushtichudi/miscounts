#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <format>
#include <fstream>
#include <iomanip>
#include <print>
#include <sstream>
#include <string>
#include <vector>

#include "Main.h"

#ifdef __WIN32
#include <windows.h>
#endif

typedef struct MiscountParams {
	Args *a;
	FurtherOptions *b;
} MiscountParams;

class Miscount {
	public:
		Args *a;
		FurtherOptions *b;

		Miscount(MiscountParams *miscountparams) {
			a = miscountparams->a;
			b = miscountparams->b;
		}

		void Init();
		int AppendMiscount(MiscountParams *m);
};