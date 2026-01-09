# OpenCL C
> `aclspv` uses dialect of OpenCL C.  
> This page describes the specification of `aclspv`'s target language.

# Possible required capabilitties
- Shader
    > by default

- Address
    > when `m_is_logical` is off

- VulkanMemoryModel
    > when `m_is_for_gl` is off

# Possible required extensions
- SPV_KHR_storage_buffer_storage_class
    > by default



# Entry Points
> with `__kernel`, void returning function shall be entry point.  
- Execution model will be `GLCompute` by default.
- Bindable object's bind numbers will be in order of its position, left to right, incremented.
- Arguments which is not pointer or an array will be merged into one push constant with 32-bit aligned.

# Address Space Qualifiers
__global  
- Has to be pointer and maps to `StorageBuffer` in Vulkan.
- Bindable
- Only for entry points
- Scope will be modular
- Must be used as an argument of `Entry Points`.
- 32-bit alignment is adviced.
- always parsed as `u32*` (you will not care)

__local  
- Has to be constant array and maps to `Workgroup` in Vulkan.
- Not binded by host.
- Its size could be `specified`. See `aclspv_set(set)`.
- Scope will be modular
- 32-bit alignment is adviced.
- always parsed as `u32[(COUNT + 3) / 4]` (you will not care)

__constant  
- Maps to `Uniform` in Vulkan when used as an argument of `Entry Points`.
    - Bindable
    - Scope will be modular
    - Only for entry points
    - 128-bit alignment is adviced.
    - always parsed as `vec4*` (you will not care)

- Maps to constant literals in other use cases.

# Attributes
> Some compiler attributes are available for vulkan-specific behaviours.  
> Many will be annotation attribute so seem as `__attribute__((annotate("<here>")))`

aclspv_specid(specid)  
> Make constant value modifiable on vulkan pipeline.  
> See [shader stage create info](https://docs.vulkan.org/refpages/latest/refpages/source/VkPipelineShaderStageCreateInfo.html)  
- if `specid` is `0xFFFFFFFF`, it will be seen undefined.
- Maps to `OpSpecConstant` with id of `specid`
- Result type must be scalar Integer or Floating point type.
- Follows the first specified value & data type.
- Default value must be specified on first use.
- Ignores the initialising value when specid is found.
- Must be specified on use. E.g:
    - Setting a size of an constant array
    - Initialising a non-constant variable
- Specifying on following condition will be ignored
    - Enumerators
    - __constant variable
    - Constant literals

aclspv_set(set)  
> Specifies the descriptor sets of bindable object.  
- Could be used on bindable objects.

aclspv_wrkgroup_size(x, y, z)  
> Specifies the entry point's workgroup size.  
- has `reqd_work_group_size(x, y, z)` as alternatives.

# Restriction
> Some statements / extensions may not be planned to be supported for now.
- `VK_KHR_relaxed_block_layout` behaviour (free alignment)
- Non plain-old-data types
- `OpenCL` memory model
- `Simple` memory model
