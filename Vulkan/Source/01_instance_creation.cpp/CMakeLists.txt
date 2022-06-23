#===============================================寻找第三方库==========================================
set(TutorialName  "")
getCurrentDirName(TutorialName)

file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
# source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/HeaderFiles" FILES ${HEADER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES})
# add_executable(TutorialName ${HEADER_FILES} ${SOURCE_FILES})
add_executable(${TutorialName}  ${SOURCE_FILES})
set_target_properties(${TutorialName} PROPERTIES FOLDER ${API_NAME})



target_link_libraries(${TutorialName} PUBLIC glm)
target_link_libraries(${TutorialName} PUBLIC glfw)
add_dependencies(${TutorialName} glfw)
add_dependencies(${TutorialName} glm)
target_include_directories(  ${TutorialName} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/Glfw/include> )

#添加vulkan
find_package(Vulkan REQUIRED FATAL_ERROR)
target_link_libraries(${TutorialName} PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(${TutorialName}  PUBLIC ${Vulkan_INCLUDE_DIRS}  )