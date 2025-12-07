cmake_minimum_required(VERSION 3.10)

enable_testing()

project(aclspv)

# Set this source directory as the tree root for project.
set(ae2f_ProjRoot ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Tree root for project")
set(ae2f_submod  .submod CACHE STRING "Relative path to submodule (subdirectory)")
set(ae2f_CoreScript_Wh ${ae2f_ProjRoot}/${ae2f_submod}/ae2f/CoreScript)

option(aclspv_N_RELOAD_ALWAYS	"reloads aclspv always on configuration when set `OFF`" ON)

if(NOT aclspv_RELOADED)
	set(aclspv_RELOADED OFF)
endif()

# Execute git to fetch CoreScript when repository does not exist
if(NOT EXISTS ${ae2f_CoreScript_Wh})
	execute_process(
		COMMAND git clone https://codeberg.org/ae2f/CoreScript
		${ae2f_CoreScript_Wh}
		)
endif()

# add subdirectory
if(NOT TARGET ae2f::CoreScript)
	add_subdirectory(${ae2f_CoreScript_Wh} ${ae2f_submod}/ae2f/CoreScript)
endif()

ae2f_CoreLibFetchX(ae2f Core	main)
ae2f_CoreLibFetchX(ae2f Preproc	main)

find_package(LLVM)
find_package(Clang)

message(STATUS ${LLVM_LIBRARY})
message(STATUS ${Clang_FOUND})

if(LLVM_FOUND)
	message(STATUS "LLVM is found")
else()
	if(NOT EXISTS ${ae2f_CoreScript_Wh})
		execute_process(
			COMMAND git clone https://github.com/llvm/llvm-project
			${ae2f_ProjRoot}/${ae2f_submod}/llvm --depth 1
			)
	endif()
endif()

if(Clang_FOUND)
	message(STATUS "clang is found")
else()
	if(NOT EXISTS ${ae2f_CoreScript_Wh})
		execute_process(
			COMMAND git clone https://github.com/llvm/llvm-project
			${ae2f_ProjRoot}/${ae2f_submod}/llvm --depth 1
			)
	endif()
endif()

file(GLOB aclspv-inc ${PROJECT_SOURCE_DIR}/inc/*)

ae2f_Macro_init(1 30 30)
ae2f_Macro_Lib_V2(
	ae3f aclspvCore INTERFACE
	${PROJECT_SOURCE_DIR}/src/ "__NOTHING" 
	${PROJECT_SOURCE_DIR}/src/ "*.def.c*" 
	${PROJECT_SOURCE_DIR}/lib/ ".auto.h"
	${PROJECT_SOURCE_DIR}/cfg/aclspvCore.cmake.in
	inc ${aclspv_RELOADED}
	)

target_link_libraries(${ae3f__aclspvCore__TENT} INTERFACE
	ae2f::Core ae2f::Preproc
	)


file(GLOB aclspv-src ${PROJECT_SOURCE_DIR}/lib/*)
file(GLOB aclspv-src-pass ${PROJECT_SOURCE_DIR}/lib/pass/*)

ae2f_CoreLibTentConfigCustom(
	aclspvABI ${ae2f_LIBPREFIX}
	inc ae3f 
	${PROJECT_SOURCE_DIR}/cfg/aclspvABI.cmake.in
	${aclspv-src} ${aclspv-inc} ${aclspv-src-pass}
	)

target_link_libraries(${ae3f__aclspvABI__TENT} PUBLIC ae3f::aclspvCore)

llvm_map_components_to_libnames(LLVM_LIBS LLVMCore)
target_link_libraries(${ae3f__aclspvABI__TENT} PUBLIC
	clang	LLVM clang-cpp
	)

ae2f_CoreTestTentVerbose(${ae3f__aclspvABI__TENT} ./test/lifespan)
ae2f_CoreTestTentVerbose(${ae3f__aclspvABI__TENT} ./test)

ae2f_UltraErrWERROR(${ae3f__aclspvABI__TENT} PRIVATE)

add_executable(dummy main.c)
target_link_libraries(dummy ae3f::aclspvABI)

option(aclspv_RELOADED "aclspv has been reloaded" ${aclspv_N_RELOAD_ALWAYS})
