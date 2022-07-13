# =====================================全局设置=======================================
cmake_minimum_required(VERSION 3.14)
# 相关变量设置
set(PROJECT_NAME "GraphicsAPI_Tutorial")
set(PROJECT_VERSION 1.0.0)
Project(${PROJECT_NAME} LANGUAGES CXX VERSION ${PROJECT_VERSION})
message(STATUS "Version: ${PROJECT_VERSION}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake)# 设置Include路径
#设置第三方库路径
set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty")

set(USE_UNICODE FALSE)

if(USE_UNICODE)
    target_compile_definitions(${TutorialName} PUBLIC "UNICODE; _UNICODE")
 endif()

#=======================include CMake===============
if(NOT SetupProject_FOUND)
    include(SetupProject)
    message(STATUS ${PROJECT_NAME}.SetupProject_FOUND = ${SetupProject_FOUND})
endif()
if(NOT  CMakeFunction_FOUND)
    include(CMakeFunction)
    message(STATUS  ${PROJECT_NAME}.CMakeFunction_FOUND = ${CMakeFunction_FOUND})
endif()




# =======================================添加子目录====================================
add_subdirectory(BasicTool)
add_subdirectory(Source)
add_subdirectory(ThirdParty)