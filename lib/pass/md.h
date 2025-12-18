/** 
 * @file	md.h 
 * @brief	metadata for llvm passes
 * */


#ifndef aclspv_md_h
#define aclspv_md_h

/** 
 * @def		ACLSPV_MD_ARGKND 
 * @brief	argument kind
 * */
#define ACLSPV_MD_ARGKND		"aclspv.kernel.argknd"

/**
 * @def		ACLSPV_MD_ARGS
 * @brief	kernel arguments
 * */
#define ACLSPV_MD_ARGS			"aclspv.kernel.args"

/**
 * @def		ACLSPV_MD_PIPELINE_LAYOUT
 * @brief	pipeline layout
 * */
#define ACLSPV_MD_PIPELINE_LAYOUT	"aclspv.pipelayout"

/**
 * @def		ACLSPV_MD_POD_CLUSTERS
 * @brief	pod clusters
 * @see		lib/pass/cluster_pod_args.c
 * @see		ACLSPV_PASSES_CLUSTER_POD_ARGS
 * */
#define ACLSPV_MD_POD_CLUSTERS		"aclspv.pod_clusters"

/**
 * @def		ACLSPV_MD_OCL_KERNELS
 * @brief	opencl kernels, which needs to be removed.
 * */
#define ACLSPV_MD_OCL_KERNELS		"opencl.kernels"



#endif
