# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.29.6/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.29.6/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/anirud/Downloads/CSProjects/burdock

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/anirud/Downloads/CSProjects/burdock/build

# Include any dependencies generated for this target.
include lib/CMakeFiles/lib.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include lib/CMakeFiles/lib.dir/compiler_depend.make

# Include the progress variables for this target.
include lib/CMakeFiles/lib.dir/progress.make

# Include the compile flags for this target's objects.
include lib/CMakeFiles/lib.dir/flags.make

lib/CMakeFiles/lib.dir/kdbfile.cpp.o: lib/CMakeFiles/lib.dir/flags.make
lib/CMakeFiles/lib.dir/kdbfile.cpp.o: /Users/anirud/Downloads/CSProjects/burdock/lib/kdbfile.cpp
lib/CMakeFiles/lib.dir/kdbfile.cpp.o: lib/CMakeFiles/lib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/anirud/Downloads/CSProjects/burdock/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/CMakeFiles/lib.dir/kdbfile.cpp.o"
	cd /Users/anirud/Downloads/CSProjects/burdock/build/lib && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT lib/CMakeFiles/lib.dir/kdbfile.cpp.o -MF CMakeFiles/lib.dir/kdbfile.cpp.o.d -o CMakeFiles/lib.dir/kdbfile.cpp.o -c /Users/anirud/Downloads/CSProjects/burdock/lib/kdbfile.cpp

lib/CMakeFiles/lib.dir/kdbfile.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/lib.dir/kdbfile.cpp.i"
	cd /Users/anirud/Downloads/CSProjects/burdock/build/lib && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/anirud/Downloads/CSProjects/burdock/lib/kdbfile.cpp > CMakeFiles/lib.dir/kdbfile.cpp.i

lib/CMakeFiles/lib.dir/kdbfile.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/lib.dir/kdbfile.cpp.s"
	cd /Users/anirud/Downloads/CSProjects/burdock/build/lib && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/anirud/Downloads/CSProjects/burdock/lib/kdbfile.cpp -o CMakeFiles/lib.dir/kdbfile.cpp.s

# Object files for target lib
lib_OBJECTS = \
"CMakeFiles/lib.dir/kdbfile.cpp.o"

# External object files for target lib
lib_EXTERNAL_OBJECTS =

lib/liblib.a: lib/CMakeFiles/lib.dir/kdbfile.cpp.o
lib/liblib.a: lib/CMakeFiles/lib.dir/build.make
lib/liblib.a: lib/CMakeFiles/lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/anirud/Downloads/CSProjects/burdock/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library liblib.a"
	cd /Users/anirud/Downloads/CSProjects/burdock/build/lib && $(CMAKE_COMMAND) -P CMakeFiles/lib.dir/cmake_clean_target.cmake
	cd /Users/anirud/Downloads/CSProjects/burdock/build/lib && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/CMakeFiles/lib.dir/build: lib/liblib.a
.PHONY : lib/CMakeFiles/lib.dir/build

lib/CMakeFiles/lib.dir/clean:
	cd /Users/anirud/Downloads/CSProjects/burdock/build/lib && $(CMAKE_COMMAND) -P CMakeFiles/lib.dir/cmake_clean.cmake
.PHONY : lib/CMakeFiles/lib.dir/clean

lib/CMakeFiles/lib.dir/depend:
	cd /Users/anirud/Downloads/CSProjects/burdock/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/anirud/Downloads/CSProjects/burdock /Users/anirud/Downloads/CSProjects/burdock/lib /Users/anirud/Downloads/CSProjects/burdock/build /Users/anirud/Downloads/CSProjects/burdock/build/lib /Users/anirud/Downloads/CSProjects/burdock/build/lib/CMakeFiles/lib.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : lib/CMakeFiles/lib.dir/depend
