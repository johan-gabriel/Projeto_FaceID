# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

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

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Users\tecnica\Downloads\FID_Ethernet 25EQR\FID_Ethernet 25EQR"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\tecnica\Downloads\FID_Ethernet 25EQR\FID_Ethernet 25EQR\build"

# Utility rule file for bs2_default_padded_checksummed_asm.

# Include any custom commands dependencies for this target.
include pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_padded_checksummed_asm.dir\compiler_depend.make

# Include the progress variables for this target.
include pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_padded_checksummed_asm.dir\progress.make

pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_padded_checksummed_asm: pico-sdk\src\rp2_common\boot_stage2\bs2_default_padded_checksummed.S
	cd C:\Users\tecnica\DOWNLO~1\FID_ET~2\FID_ET~1\build\pico-sdk\src\RP2_CO~1\BOOT_S~1
	cd C:\Users\tecnica\DOWNLO~1\FID_ET~2\FID_ET~1\build

pico-sdk\src\rp2_common\boot_stage2\bs2_default_padded_checksummed.S: pico-sdk\src\rp2_common\boot_stage2\bs2_default.bin
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir="C:\Users\tecnica\Downloads\FID_Ethernet 25EQR\FID_Ethernet 25EQR\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Generating bs2_default_padded_checksummed.S"
	cd C:\Users\tecnica\DOWNLO~1\FID_ET~2\FID_ET~1\build\pico-sdk\src\RP2_CO~1\BOOT_S~1
	C:\Users\tecnica\AppData\Local\Programs\Python\Python311\python.exe C:/Users/tecnica/Documents/Pico/pico-sdk/src/rp2_common/boot_stage2/pad_checksum -s 0xffffffff "C:/Users/tecnica/Downloads/FID_Ethernet 25EQR/FID_Ethernet 25EQR/build/pico-sdk/src/rp2_common/boot_stage2/bs2_default.bin" "C:/Users/tecnica/Downloads/FID_Ethernet 25EQR/FID_Ethernet 25EQR/build/pico-sdk/src/rp2_common/boot_stage2/bs2_default_padded_checksummed.S"
	cd C:\Users\tecnica\DOWNLO~1\FID_ET~2\FID_ET~1\build

pico-sdk\src\rp2_common\boot_stage2\bs2_default.bin: pico-sdk\src\rp2_common\boot_stage2\bs2_default.elf
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir="C:\Users\tecnica\Downloads\FID_Ethernet 25EQR\FID_Ethernet 25EQR\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Generating bs2_default.bin"
	cd C:\Users\tecnica\DOWNLO~1\FID_ET~2\FID_ET~1\build\pico-sdk\src\RP2_CO~1\BOOT_S~1
	echo >nul && "C:\Program Files (x86)\GNU Arm Embedded Toolchain\10 2021.10\bin\arm-none-eabi-objcopy.exe" -Obinary "C:/Users/tecnica/Downloads/FID_Ethernet 25EQR/FID_Ethernet 25EQR/build/pico-sdk/src/rp2_common/boot_stage2/bs2_default.elf" "C:/Users/tecnica/Downloads/FID_Ethernet 25EQR/FID_Ethernet 25EQR/build/pico-sdk/src/rp2_common/boot_stage2/bs2_default.bin"
	cd C:\Users\tecnica\DOWNLO~1\FID_ET~2\FID_ET~1\build

bs2_default_padded_checksummed_asm: pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_padded_checksummed_asm
bs2_default_padded_checksummed_asm: pico-sdk\src\rp2_common\boot_stage2\bs2_default.bin
bs2_default_padded_checksummed_asm: pico-sdk\src\rp2_common\boot_stage2\bs2_default_padded_checksummed.S
bs2_default_padded_checksummed_asm: pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_padded_checksummed_asm.dir\build.make
.PHONY : bs2_default_padded_checksummed_asm

# Rule to build all files generated by this target.
pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_padded_checksummed_asm.dir\build: bs2_default_padded_checksummed_asm
.PHONY : pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_padded_checksummed_asm.dir\build

pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_padded_checksummed_asm.dir\clean:
	cd C:\Users\tecnica\DOWNLO~1\FID_ET~2\FID_ET~1\build\pico-sdk\src\RP2_CO~1\BOOT_S~1
	$(CMAKE_COMMAND) -P CMakeFiles\bs2_default_padded_checksummed_asm.dir\cmake_clean.cmake
	cd C:\Users\tecnica\DOWNLO~1\FID_ET~2\FID_ET~1\build
.PHONY : pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_padded_checksummed_asm.dir\clean

pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_padded_checksummed_asm.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" "C:\Users\tecnica\Downloads\FID_Ethernet 25EQR\FID_Ethernet 25EQR" C:\Users\tecnica\Documents\Pico\pico-sdk\src\rp2_common\boot_stage2 "C:\Users\tecnica\Downloads\FID_Ethernet 25EQR\FID_Ethernet 25EQR\build" "C:\Users\tecnica\Downloads\FID_Ethernet 25EQR\FID_Ethernet 25EQR\build\pico-sdk\src\rp2_common\boot_stage2" "C:\Users\tecnica\Downloads\FID_Ethernet 25EQR\FID_Ethernet 25EQR\build\pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_padded_checksummed_asm.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : pico-sdk\src\rp2_common\boot_stage2\CMakeFiles\bs2_default_padded_checksummed_asm.dir\depend

