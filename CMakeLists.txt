cmake_minimum_required(VERSION 3.10)

project(aclspv)

# Set this source directory as the tree root for project.
set(ae2f_ProjRoot ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Tree root for project")
set(ae2f_submod  .submod CACHE STRING "Relative path to submodule (subdirectory)")
set(ae2f_CoreScript_Wh ${ae2f_ProjRoot}/${ae2f_submod}/ae2f/CoreScript)

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

ae2f_CoreLibFetchX(ae2f Core main)

find_package(LLVM CONFIG)
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

file(GLOB aclspv-src ${PROJECT_SOURCE_DIR}/lib/*)

ae2f_CoreLibTentConfigCustom(
	aclspv ${ae2f_LIBPREFIX}
	inc ae3f ${PROJECT_SOURCE_DIR}/cfg/aclspv.cmake.in
	${aclspv-src}
	)
