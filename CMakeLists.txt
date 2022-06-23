# =====================================全局设置=======================================
cmake_minimum_required(VERSION 3.11)
# 相关变量设置
# set(BUILD_USE_64BITS on)
set(PROJECT_NAME "GraphicsAPI_Tutorial")
set(CMAKE_DEBUG_POSTFIX "_d")
set(PROJECT_VERSION 1.0.0)
Project(${PROJECT_NAME} LANGUAGES CXX VERSION ${PROJECT_VERSION})
message(STATUS "Version: ${PROJECT_VERSION}")

# 设置c++ 版本 目前选择17版
set(CMAKE_CXX_STANDARD 17) # 方法一，全局设置
set(CMAKE_CXX_FLAGS "/GR-") # 设置关闭RTTI
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#设置模块查找路径
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Tinyobjloader")
set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty")


# ===================================定义函数========================================
# 添加子文件夹
function(fAdd_all_subdirectory)
    file(GLOB _children LIST_DIRECTORIES true RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
    set(_dirlist "")

    foreach(_child ${_children})
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_child}/CMakeLists.txt)
            list(APPEND _dirlist ${_child})
            add_subdirectory(${_child})
        endif()
    endforeach()
endfunction()


#======================================全局函数================================
function(getCurrentDirName _current_dir_name)
    get_filename_component(temp_dir_name ${CMAKE_CURRENT_SOURCE_DIR} NAME )
    string(REPLACE " " "_" temp_dir_name ${temp_dir_name})
    set(${_current_dir_name}  ${temp_dir_name}  PARENT_SCOPE)
endfunction(getCurrentDirName current_dir_name)

# =======================================添加子目录====================================
fAdd_all_subdirectory()
