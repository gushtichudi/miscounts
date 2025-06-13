import build
import sys

b = build.Build("miscount")

b.add_compiler_arguments(
	[
		"-Wall", "-Wextra", "-Wpedantic", "-Wfatal-errors", "-std=gnu++23", "-march=native", "-O2", "-ggdb3"
	]
)

if sys.platform == "win32": b.append_compiler_argument("-lstdc++exp")

	
b.override_default_compiler("c++")


b.add_file(["src/Main.cpp"], ["Miscount.o"], build.BinaryType.Program)
b.add_file(["src/Miscount.cpp"], None, build.BinaryType.Object)

b.start_build()