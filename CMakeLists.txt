cmake_minimum_required(VERSION 3.10)

enable_testing()

project(aclspv)

# Set this source directory as the tree root for project.
set(ae2f_ProjRoot ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Tree root for project")
set(ae2f_submod  .submod CACHE STRING "Relative path to submodule (subdirectory)")

option(aclspv_N_RELOAD_ALWAYS	"reloads aclspv always on configuration when set `OFF`" ON)

if(NOT aclspv_RELOADED)
	set(_aclspv_RELOADED OFF)
else()
	set(_aclspv_RELOADED ON)
endif()

option(aclspv_RELOADED "aclspv has been reloaded" ${aclspv_N_RELOAD_ALWAYS})

# Execute git to fetch CoreScript when repository does not exist
set(ae2f_CoreScript_Wh ${ae2f_ProjRoot}/${ae2f_submod}/ae2f/CoreScript)
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

ae2f_CoreLibFetchX(KhronosGroup SPIRV-Headers main)

find_package(Clang)

message(STATUS ${Clang_FOUND})

if(Clang_FOUND)
	message(STATUS "clang is found")
else()
	if(NOT EXISTS ${ae2f_CoreScript_Wh}/llvm)
		execute_process(
			COMMAND git clone https://github.com/llvm/llvm-project
			${ae2f_CoreScript_Wh}/llvm --depth 1
			)
	endif()

	add_subdirectory(${ae2f_CoreScript_Wh}/llvm/clang ${ae2f_BinRoot}/${ae2f_submod}/clang)
endif()

file(GLOB aclspv-inc ${PROJECT_SOURCE_DIR}/inc/*)

ae2f_Macro_init(1 30 30)

ae2f_Macro_Lib_V2(
	ae3f aclspvCore INTERFACE
	${PROJECT_SOURCE_DIR}/src/ "__NOTHING"
	${PROJECT_SOURCE_DIR}/src/ "*.c*"
	${PROJECT_SOURCE_DIR}/lib/ ".auto.h"
	${PROJECT_SOURCE_DIR}/cfg/aclspvCore.cmake.in
	inc ${_aclspv_RELOADED} 
	)


if(NOT ${_aclspv_RELOADED})
	ae2f_FileRaw_init(1 100)
	ae2f_FileRaw_Run_One(
		${PROJECT_SOURCE_DIR}/src/source.cl 
		${PROJECT_SOURCE_DIR}/test/lifespan/source.auto.h
		)

	ae2f_FileRaw_Run_One2(
		${PROJECT_SOURCE_DIR}/src/ocl.h
		${PROJECT_SOURCE_DIR}/test/lifespan/ocl.auto.h
		"B"
		)
endif()

target_link_libraries(${ae3f__aclspvCore__TENT} INTERFACE
	ae2f::Core ae2f::Preproc
	)

target_link_libraries(${ae3f__aclspvCore__TENT} INTERFACE 
	SPIRV-Headers::SPIRV-Headers
	)

file(GLOB aclspv-src ${PROJECT_SOURCE_DIR}/lib/*)
file(GLOB aclspv-src-build ${PROJECT_SOURCE_DIR}/lib/build/*.tar.c)

ae2f_CoreLibTentConfigCustom(
	aclspvABI ${ae2f_LIBPREFIX}
	inc ae3f
	${PROJECT_SOURCE_DIR}/cfg/aclspvABI.cmake.in
	${aclspv-src} ${aclspv-inc} 
	${aclspv-src-pass}
	${aclspv-src-build}
	)

target_link_libraries(${ae3f__aclspvABI__TENT} PUBLIC ae3f::aclspvCore)
target_include_directories(${ae3f__aclspvABI__TENT} PRIVATE lib)

target_link_libraries(${ae3f__aclspvABI__TENT} PUBLIC libclang)

ae2f_CoreTestTentVerbose(${ae3f__aclspvABI__TENT} ./test/lifespan)
ae2f_CoreTestTentVerbose(${ae3f__aclspvABI__TENT} ./test/util)

ae2f_UltraErrWERROR(${ae3f__aclspvABI__TENT} PRIVATE)

