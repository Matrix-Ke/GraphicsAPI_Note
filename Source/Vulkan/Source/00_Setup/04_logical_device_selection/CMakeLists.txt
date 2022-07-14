#===============================================寻找第三方库==========================================
set(TutorialName  "")
GetCurrentDirectoryName(TutorialName)

file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES})
add_executable(${TutorialName}  ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(${TutorialName} PROPERTIES FOLDER ${ROOT_FOLDER_NAME}/${SAMPLE_DIRECTOPRY})



target_link_libraries(${TutorialName} PUBLIC glm)
target_link_libraries(${TutorialName} PUBLIC glfw)
add_dependencies(${TutorialName} glfw)
add_dependencies(${TutorialName} glm)
target_include_directories(  ${TutorialName} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/Glfw/include> )
if(${USE_UNICODE})
    target_compile_definitions(${TutorialName} PUBLIC "UNICODE; _UNICODE")
endif()
# target_compile_definitions(${TutorialName} PUBLIC "/EHsc")
IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/EHsc")
ENDIF(MSVC)

#添加vulkan

target_link_libraries(${TutorialName} PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(${TutorialName}  PUBLIC ${Vulkan_INCLUDE_DIRS}  )