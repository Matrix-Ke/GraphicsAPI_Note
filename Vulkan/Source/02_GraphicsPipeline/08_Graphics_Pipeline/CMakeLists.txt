#===============================================寻找第三方库==========================================
set(TutorialName  "")
GetCurrentDirectoryName(TutorialName)

file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file (GLOB_RECURSE SHADER_SOURCES ./*.vert ./*.frag)
source_group("Shaders" FILES ${SHADER_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES})
add_executable(${TutorialName}  ${HEADER_FILES} ${SOURCE_FILES} ${SHADER_SOURCES})
set_target_properties(${TutorialName} PROPERTIES FOLDER ${ROOT_FOLDER_NAME}/${SAMPLE_DIRECTOPRY})



target_link_libraries(${TutorialName} PUBLIC glm)
target_link_libraries(${TutorialName} PUBLIC glfw)
add_dependencies(${TutorialName} glfw)
add_dependencies(${TutorialName} glm)
target_include_directories(  ${TutorialName} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/Glfw/include> )
# target_compile_definitions(${TutorialName} PUBLIC "UNICODE; _UNICODE")
# target_compile_definitions(${TutorialName} PUBLIC "/EHsc")
IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/EHsc")
ENDIF(MSVC)

#添加vulkan
target_link_libraries(${TutorialName} PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(${TutorialName}  PUBLIC ${Vulkan_INCLUDE_DIRS}  )

#构建编译shader程序
set(Shader_Program  ${TutorialName}_ShaderProgram )
add_shaders_target(${Shader_Program}  SHADER_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}"  SOURCES "${SHADER_SOURCES}" )
set_target_properties(${Shader_Program} PROPERTIES FOLDER  ${ROOT_FOLDER_NAME}/${SAMPLE_DIRECTOPRY})