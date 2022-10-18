set(SHADER_TARGET "${EngineName}_ShaderCompile")

file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.vert"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.frag"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.comp"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.geom"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.mesh")

SOURCE_GROUP("shaders" FILES ${SHADER_FILES})

set(SHADER_INCLUDE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(GENERATED_SHADER_FOLDER "generated")
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_SHADER_FOLDER})
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_SHADER_FOLDER})
endif()

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_SHADER_FOLDER}/spv)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_SHADER_FOLDER}/spv)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/CompileVulkanShader.cmake)

compile_shader(
  "${SHADER_FILES}"
  "${SHADER_TARGET}"
  "${SHADER_INCLUDE_FOLDER}"
  "${GENERATED_SHADER_FOLDER}"
  "glslang::validator")

set_target_properties("${SHADER_TARGET}" PROPERTIES FOLDER ${SAMPLE_DIRECTOPRY} )



