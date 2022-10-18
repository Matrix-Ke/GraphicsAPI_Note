#===============================================寻找第三方库==========================================
set(EngineName  "Editor")
set(SAMPLE_DIRECTOPRY "Application")

# 开启异常处理编译选项
IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/EHsc") 
ENDIF(MSVC)

file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES})
add_executable(${EngineName}  ${HEADER_FILES} ${SOURCE_FILES} )
set_target_properties(${EngineName} PROPERTIES FOLDER ${SAMPLE_DIRECTOPRY})


target_link_libraries(${EngineName} PRIVATE glm)
target_link_libraries(${EngineName} PRIVATE glfw)
target_link_libraries(${EngineName} PRIVATE imgui)
target_link_libraries(${EngineName} PRIVATE spdlog)
target_link_libraries(${EngineName} PRIVATE tinyobjloader)
target_link_libraries(${EngineName} PRIVATE stb)
target_link_libraries(${EngineName} PRIVATE  BasicTool::Platform)
target_link_libraries(${EngineName} PRIVATE  BasicTool::Container)
target_link_libraries(${EngineName} PRIVATE ${Vulkan_LIBRARIES})#添加vulkan
add_dependencies(${EngineName} glm)
add_dependencies(${EngineName} glfw)
add_dependencies(${EngineName} imgui)
add_dependencies(${EngineName} spdlog)
add_dependencies(${EngineName} tinyobjloader)
add_dependencies(${EngineName}  BasicTool::Platform)
add_dependencies(${EngineName}  BasicTool::Container)

# 添加外部依赖目录
target_include_directories(${EngineName}  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${EngineName}  PRIVATE $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/Glfw/include> )
target_include_directories(${EngineName}  PRIVATE imgui)
target_include_directories(${EngineName}  PRIVATE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/BasicTool> )
target_include_directories(${EngineName}  PRIVATE ${Vulkan_INCLUDE_DIRS} )
target_include_directories(${EngineName}  PRIVATE ${THIRD_PARTY_DIR}/rapidjson/include)
target_include_directories(${EngineName}  PRIVATE ${THIRD_PARTY_DIR}/tinyobjloader)


if(${USE_UNICODE})
    target_compile_definitions(${EngineName} PUBLIC "UNICODE; _UNICODE")
endif()

#构建编译shader程序
# set(Shader_Program  ${EngineName}_ShaderCompile) 
# message(STATUS SHADER_SOURCES = ${SHADER_SOURCES})
# add_shaders_target(${Shader_Program}  OUTPUT_DIR  ${CMAKE_CURRENT_SOURCE_DIR}  SOURCES ${SHADER_SOURCES} )
# set_target_properties(${Shader_Program} PROPERTIES FOLDER  ${SAMPLE_DIRECTOPRY})

# =======================================添加子目录====================================
Add_all_subdirectory()