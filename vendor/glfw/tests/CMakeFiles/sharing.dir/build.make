# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.13.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.13.2/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/johanventer/Development/ray-tracer/vendor/glfw

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/johanventer/Development/ray-tracer/vendor/glfw

# Include any dependencies generated for this target.
include tests/CMakeFiles/sharing.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/sharing.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/sharing.dir/flags.make

tests/CMakeFiles/sharing.dir/sharing.c.o: tests/CMakeFiles/sharing.dir/flags.make
tests/CMakeFiles/sharing.dir/sharing.c.o: tests/sharing.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/johanventer/Development/ray-tracer/vendor/glfw/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/CMakeFiles/sharing.dir/sharing.c.o"
	cd /Users/johanventer/Development/ray-tracer/vendor/glfw/tests && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/sharing.dir/sharing.c.o   -c /Users/johanventer/Development/ray-tracer/vendor/glfw/tests/sharing.c

tests/CMakeFiles/sharing.dir/sharing.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sharing.dir/sharing.c.i"
	cd /Users/johanventer/Development/ray-tracer/vendor/glfw/tests && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/johanventer/Development/ray-tracer/vendor/glfw/tests/sharing.c > CMakeFiles/sharing.dir/sharing.c.i

tests/CMakeFiles/sharing.dir/sharing.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sharing.dir/sharing.c.s"
	cd /Users/johanventer/Development/ray-tracer/vendor/glfw/tests && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/johanventer/Development/ray-tracer/vendor/glfw/tests/sharing.c -o CMakeFiles/sharing.dir/sharing.c.s

tests/CMakeFiles/sharing.dir/__/deps/glad.c.o: tests/CMakeFiles/sharing.dir/flags.make
tests/CMakeFiles/sharing.dir/__/deps/glad.c.o: deps/glad.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/johanventer/Development/ray-tracer/vendor/glfw/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object tests/CMakeFiles/sharing.dir/__/deps/glad.c.o"
	cd /Users/johanventer/Development/ray-tracer/vendor/glfw/tests && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/sharing.dir/__/deps/glad.c.o   -c /Users/johanventer/Development/ray-tracer/vendor/glfw/deps/glad.c

tests/CMakeFiles/sharing.dir/__/deps/glad.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sharing.dir/__/deps/glad.c.i"
	cd /Users/johanventer/Development/ray-tracer/vendor/glfw/tests && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/johanventer/Development/ray-tracer/vendor/glfw/deps/glad.c > CMakeFiles/sharing.dir/__/deps/glad.c.i

tests/CMakeFiles/sharing.dir/__/deps/glad.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sharing.dir/__/deps/glad.c.s"
	cd /Users/johanventer/Development/ray-tracer/vendor/glfw/tests && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/johanventer/Development/ray-tracer/vendor/glfw/deps/glad.c -o CMakeFiles/sharing.dir/__/deps/glad.c.s

# Object files for target sharing
sharing_OBJECTS = \
"CMakeFiles/sharing.dir/sharing.c.o" \
"CMakeFiles/sharing.dir/__/deps/glad.c.o"

# External object files for target sharing
sharing_EXTERNAL_OBJECTS =

tests/sharing.app/Contents/MacOS/sharing: tests/CMakeFiles/sharing.dir/sharing.c.o
tests/sharing.app/Contents/MacOS/sharing: tests/CMakeFiles/sharing.dir/__/deps/glad.c.o
tests/sharing.app/Contents/MacOS/sharing: tests/CMakeFiles/sharing.dir/build.make
tests/sharing.app/Contents/MacOS/sharing: src/libglfw3.a
tests/sharing.app/Contents/MacOS/sharing: tests/CMakeFiles/sharing.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/johanventer/Development/ray-tracer/vendor/glfw/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable sharing.app/Contents/MacOS/sharing"
	cd /Users/johanventer/Development/ray-tracer/vendor/glfw/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sharing.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/sharing.dir/build: tests/sharing.app/Contents/MacOS/sharing

.PHONY : tests/CMakeFiles/sharing.dir/build

tests/CMakeFiles/sharing.dir/clean:
	cd /Users/johanventer/Development/ray-tracer/vendor/glfw/tests && $(CMAKE_COMMAND) -P CMakeFiles/sharing.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/sharing.dir/clean

tests/CMakeFiles/sharing.dir/depend:
	cd /Users/johanventer/Development/ray-tracer/vendor/glfw && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/johanventer/Development/ray-tracer/vendor/glfw /Users/johanventer/Development/ray-tracer/vendor/glfw/tests /Users/johanventer/Development/ray-tracer/vendor/glfw /Users/johanventer/Development/ray-tracer/vendor/glfw/tests /Users/johanventer/Development/ray-tracer/vendor/glfw/tests/CMakeFiles/sharing.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/sharing.dir/depend

