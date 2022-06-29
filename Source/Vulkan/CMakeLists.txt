find_package(Vulkan REQUIRED FATAL_ERROR)
add_executable (glslang::validator IMPORTED)
find_program (GLSLANG_VALIDATOR "glslangValidator" HINTS $ENV{VULKAN_SDK}/bin REQUIRED)
set_property (TARGET glslang::validator PROPERTY IMPORTED_LOCATION "${GLSLANG_VALIDATOR}")
message(STATUS VULKAN_SDK_PATH = $ENV{VULKAN_SDK}/bin)
message(STATUS VULKAN_SDK_IMPORTED_LOCATION = ${GLSLANG_VALIDATOR} )
set(ROOT_FOLDER_NAME "Vulkan")

#===================引入自置库函数=======================
include(CompileVulkanShader)

# =======================================添加子目录====================================
Add_all_subdirectory()
