#===============================================寻找第三方库==========================================
set(TutorialName  "")
GetCurrentDirectoryName(TutorialName)

# 开启异常处理编译选项
IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/EHsc") 
ENDIF(MSVC)

file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file (GLOB_RECURSE SHADER_SOURCES ./*.vert ./*.frag)
source_group("Shaders" FILES ${SHADER_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES})
add_executable(${TutorialName}  ${HEADER_FILES} ${SOURCE_FILES} ${SHADER_SOURCES})
set_target_properties(${TutorialName} PROPERTIES FOLDER ${ROOT_FOLDER_NAME}/${SAMPLE_DIRECTOPRY})



target_link_libraries(${TutorialName} PRIVATE glm)
target_link_libraries(${TutorialName} PRIVATE glfw)
target_link_libraries(${TutorialName} PRIVATE spdlog)
target_link_libraries(${TutorialName} PRIVATE stb)
target_link_libraries(${TutorialName} PRIVATE  BasicTool::Platform)
target_link_libraries(${TutorialName} PRIVATE  BasicTool::Container)
target_link_libraries(${TutorialName} PRIVATE ${Vulkan_LIBRARIES})#添加vulkan
add_dependencies(${TutorialName} glfw)
add_dependencies(${TutorialName} glm)
add_dependencies(${TutorialName}  BasicTool::Platform)
add_dependencies(${TutorialName}  BasicTool::Container)
add_dependencies(${TutorialName} spdlog)

target_include_directories( ${TutorialName} PRIVATE $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/Glfw/include> )
target_include_directories(${TutorialName} PRIVATE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/BasicTool> )
target_include_directories(${TutorialName}  PRIVATE ${Vulkan_INCLUDE_DIRS} )
target_include_directories(${TutorialName} PRIVATE ${THIRD_PARTY_DIR}/rapidjson/include)


if(USE_UNICODE)
    target_compile_definitions(${TutorialName} PUBLIC "UNICODE; _UNICODE")
endif()


#构建编译shader程序
set(Shader_Program  ${TutorialName}_ShaderProgram ) 
# message(STATUS TARGET_OUTPUT_NAME = ${TutorialName})
add_shaders_target(${Shader_Program}  OUTPUT_DIR  ${CMAKE_CURRENT_SOURCE_DIR}   SOURCES ${SHADER_SOURCES} )
set_target_properties(${Shader_Program} PROPERTIES FOLDER  ${ROOT_FOLDER_NAME}/${SAMPLE_DIRECTOPRY})