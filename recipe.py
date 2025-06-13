import build
import sys

b = build.Build("miscount")

if sys.platform == "win32":
	b.add_compiler_arguments(
		[
			"-Wall", "-Wextra", "-Wpedantic", "-Wfatal-errors", "-std=gnu++23", "-march=native", "-O2", "-lstdc++exp", "-ggdb3"
		]
	)
else:
	b.add_compiler_arguments(
		[
			"-Wall", "-Wextra", "-Wpedantic", "-Wfatal-errors", "-std=gnu++23", "-march=native", "-O2", "-ggdb3"
		]
	)

b.override_default_compiler("c++")


b.add_file(["src/Main.cpp"], ["Miscount.o"], build.BinaryType.Program)
b.add_file(["src/Miscount.cpp"], None, build.BinaryType.Object)

b.start_build()