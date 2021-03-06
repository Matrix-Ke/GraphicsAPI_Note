
#===========================配置第三方库==============================
set(third_party_folder "ThirdParty")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# if(NOT TARGET imgui)
#     include(imgui.cmake)
#     set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder}/imgui)
# endif()

if(NOT TARGET glm)
    option(BUILD_STATIC_LIBS "" ON)
    option(BUILD_TESTING "" OFF)
    option(GLM_TEST_ENABLE "" OFF)
    add_subdirectory(glm)
    set_target_properties(glm_static PROPERTIES FOLDER ${third_party_folder}/glm)
endif()

if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
    set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw)
    set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw)
endif()

if(NOT TARGET tinyobjloader)
    add_subdirectory(tinyobjloader)
    set_target_properties(tinyobjloader PROPERTIES FOLDER ${third_party_folder}/tinyobjloader)
    set_target_properties(uninstall PROPERTIES FOLDER ${third_party_folder}/tinyobjloader)
endif()

if(NOT TARGET rapidjson)
    option(RAPIDJSON_BUILD_DOC "Build rapidjson documentation." OFF)
    option(RAPIDJSON_BUILD_EXAMPLES "Build rapidjson examples." OFF)
    option(RAPIDJSON_BUILD_TESTS "Build rapidjson perftests and unittests." OFF)
    option(RAPIDJSON_BUILD_THIRDPARTY_GTEST "Use gtest installation in `thirdparty/gtest` by default if available" OFF)
    CMAKE_MINIMUM_REQUIRED(VERSION 2.8...3.14)
    add_subdirectory(rapidjson)
    # rapidjson 只有头文件
    # set_target_properties(rapidjson PROPERTIES FOLDER ${third_party_folder}/json11)
endif()

if(NOT TARGET spdlog)
    option(SPDLOG_BUILD_EXAMPLE "" OFF)
    option(SPDLOG_INSTALL "" OFF)
    add_subdirectory(spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER ${third_party_folder}/spdlog)
    target_compile_options(spdlog PRIVATE /EHsc)
endif()

if(NOT TARGET stb)
    file(GLOB stb_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/stb/*.h")
    add_library(stb INTERFACE ${stb_sources})
    add_library(stb::stb  ALIAS stb)
    target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)
    set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)
endif()

if(NOT TARGET imgui)
find_package(Vulkan REQUIRED FATAL_ERROR)
    set(imgui_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
    file(GLOB imgui_sources CONFIGURE_DEPENDS  "${imgui_SOURCE_DIR_}/*.cpp")
    file(GLOB imgui_impl CONFIGURE_DEPENDS  
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.cpp" 
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.h"
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_vulkan.cpp" 
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_vulkan.h")
    add_library(imgui STATIC ${imgui_sources} ${imgui_impl})
    target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${imgui_SOURCE_DIR_}>)
    target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${Vulkan_INCLUDE_DIRS}>)
    target_link_libraries(imgui PUBLIC glfw ${Vulkan_LIBRARIES})
    set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder}/imgui)
endif()
