# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/Cplus/MiniSTL

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/Cplus/MiniSTL/build

# Include any dependencies generated for this target.
include CMakeFiles/MiniSTL.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MiniSTL.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MiniSTL.dir/flags.make

CMakeFiles/MiniSTL.dir/alloc.cpp.o: CMakeFiles/MiniSTL.dir/flags.make
CMakeFiles/MiniSTL.dir/alloc.cpp.o: ../alloc.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/Cplus/MiniSTL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/MiniSTL.dir/alloc.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MiniSTL.dir/alloc.cpp.o -c /home/Cplus/MiniSTL/alloc.cpp

CMakeFiles/MiniSTL.dir/alloc.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MiniSTL.dir/alloc.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/Cplus/MiniSTL/alloc.cpp > CMakeFiles/MiniSTL.dir/alloc.cpp.i

CMakeFiles/MiniSTL.dir/alloc.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MiniSTL.dir/alloc.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/Cplus/MiniSTL/alloc.cpp -o CMakeFiles/MiniSTL.dir/alloc.cpp.s

CMakeFiles/MiniSTL.dir/test.cpp.o: CMakeFiles/MiniSTL.dir/flags.make
CMakeFiles/MiniSTL.dir/test.cpp.o: ../test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/Cplus/MiniSTL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/MiniSTL.dir/test.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MiniSTL.dir/test.cpp.o -c /home/Cplus/MiniSTL/test.cpp

CMakeFiles/MiniSTL.dir/test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MiniSTL.dir/test.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/Cplus/MiniSTL/test.cpp > CMakeFiles/MiniSTL.dir/test.cpp.i

CMakeFiles/MiniSTL.dir/test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MiniSTL.dir/test.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/Cplus/MiniSTL/test.cpp -o CMakeFiles/MiniSTL.dir/test.cpp.s

CMakeFiles/MiniSTL.dir/test_vector.cpp.o: CMakeFiles/MiniSTL.dir/flags.make
CMakeFiles/MiniSTL.dir/test_vector.cpp.o: ../test_vector.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/Cplus/MiniSTL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/MiniSTL.dir/test_vector.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MiniSTL.dir/test_vector.cpp.o -c /home/Cplus/MiniSTL/test_vector.cpp

CMakeFiles/MiniSTL.dir/test_vector.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MiniSTL.dir/test_vector.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/Cplus/MiniSTL/test_vector.cpp > CMakeFiles/MiniSTL.dir/test_vector.cpp.i

CMakeFiles/MiniSTL.dir/test_vector.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MiniSTL.dir/test_vector.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/Cplus/MiniSTL/test_vector.cpp -o CMakeFiles/MiniSTL.dir/test_vector.cpp.s

# Object files for target MiniSTL
MiniSTL_OBJECTS = \
"CMakeFiles/MiniSTL.dir/alloc.cpp.o" \
"CMakeFiles/MiniSTL.dir/test.cpp.o" \
"CMakeFiles/MiniSTL.dir/test_vector.cpp.o"

# External object files for target MiniSTL
MiniSTL_EXTERNAL_OBJECTS =

MiniSTL: CMakeFiles/MiniSTL.dir/alloc.cpp.o
MiniSTL: CMakeFiles/MiniSTL.dir/test.cpp.o
MiniSTL: CMakeFiles/MiniSTL.dir/test_vector.cpp.o
MiniSTL: CMakeFiles/MiniSTL.dir/build.make
MiniSTL: CMakeFiles/MiniSTL.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/Cplus/MiniSTL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable MiniSTL"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MiniSTL.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MiniSTL.dir/build: MiniSTL

.PHONY : CMakeFiles/MiniSTL.dir/build

CMakeFiles/MiniSTL.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MiniSTL.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MiniSTL.dir/clean

CMakeFiles/MiniSTL.dir/depend:
	cd /home/Cplus/MiniSTL/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/Cplus/MiniSTL /home/Cplus/MiniSTL /home/Cplus/MiniSTL/build /home/Cplus/MiniSTL/build /home/Cplus/MiniSTL/build/CMakeFiles/MiniSTL.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MiniSTL.dir/depend

