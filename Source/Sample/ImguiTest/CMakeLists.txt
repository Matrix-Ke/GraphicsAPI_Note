#===============================================寻找第三方库==========================================
set(SampleName  "")
GetCurrentDirectoryName(SampleName)



file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES})
add_executable(${SampleName}  ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(${SampleName} PROPERTIES FOLDER ${ROOT_FOLDER_NAME}/${SampleName})


target_link_libraries(${SampleName} PRIVATE imgui)
# target_link_libraries(${SampleName} PRIVATE glfw)
target_link_libraries(${SampleName} PRIVATE spdlog)
add_dependencies(${SampleName} imgui)
# add_dependencies(${SampleName} glfw)
add_dependencies(${SampleName} spdlog)

target_include_directories(${SampleName}  PRIVATE imgui)
target_include_directories(${SampleName}  PRIVATE ${THIRD_PARTY_DIR}/imgui)

# target_include_directories(${SampleName} PUBLIC $<BUILD_INTERFACE:${imgui_SOURCE_DIR_}>)
# target_include_directories(${SampleName} PUBLIC $<BUILD_INTERFACE:${Vulkan_INCLUDE_DIRS}>)


if(${USE_UNICODE})
    target_compile_definitions(${SampleName} PUBLIC "UNICODE; _UNICODE")
endif()