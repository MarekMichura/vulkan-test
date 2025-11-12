# =============================
# 1. Create executable
# =============================

set(PROJECT ${PROJECT_NAME}_CLIENT)
add_executable(${PROJECT})

if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 20)
endif()

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/*.hpp"
)

target_sources(${PROJECT} PRIVATE ${CLIENT_SOURCES})
target_compile_features(${PROJECT} PRIVATE cxx_std_23)
target_compile_options(${PROJECT} PRIVATE ${FLAGS})
target_compile_definitions(${PROJECT} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_link_libraries(${PROJECT} PRIVATE ${PROJECT_NAME}_UTILS ${PROJECT_NAME}_VULKAN)

# =============================
# 2. Include dir
# =============================

#set(CLIENT_INCLUDE_DIRS "")
#foreach(header ${CLIENT_SOURCES})
#  get_filename_component(dir ${header} DIRECTORY)
#  list(APPEND CLIENT_INCLUDE_DIRS ${dir})
#endforeach()
#list(REMOVE_DUPLICATES CLIENT_INCLUDE_DIRS)
#
#target_include_directories(${PROJECT}
#  SYSTEM PRIVATE
#  ${vulkan_headers_SOURCE_DIR}/include
#  ${glfw_SOURCE_DIR}/include
#  ${volk_SOURCE_DIR}/
#  PRIVATE
#  ${CLIENT_INCLUDE_DIRS}
#)
#
## =============================
## 3. Shader compile
## =============================
#
#find_program(GLSLANG_VALIDATOR glslangValidator)
#if(NOT GLSLANG_VALIDATOR)
#  message(FATAL_ERROR "glslangValidator not found!")
#endif()
#
#set(SHADER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#set(SPIRV_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
#file(MAKE_DIRECTORY ${SPIRV_OUTPUT_DIR})
#
#file(GLOB_RECURSE SHADERS "${SHADER_SRC_DIR}/*.vert" "${SHADER_SRC_DIR}/*.frag")
#
#set(SPIRV_SHADERS "")
#
#foreach(SHADER ${SHADERS})
#  get_filename_component(FILENAME ${SHADER} NAME)
#  set(SPIRV ${SPIRV_OUTPUT_DIR}/${FILENAME}.spv)
#
#  get_filename_component(EXT ${SHADER} EXT)
#  if(EXT STREQUAL ".vert")
#    set(STAGE vert)
#  elseif(EXT STREQUAL ".frag")
#    set(STAGE frag)
#  else()
#    message(FATAL_ERROR "shader type not foutnd ${FILENAME}")
#  endif()
#
#  add_custom_command(
#    OUTPUT ${SPIRV}
#    COMMAND ${GLSLANG_VALIDATOR} -V -S ${STAGE} ${SHADER} -o ${SPIRV}
#    DEPENDS ${SHADER}
#    COMMENT "copping shader: ${FILENAME}"
#    VERBATIM
#  )
#
#  list(APPEND SPIRV_SHADERS ${SPIRV})
#endforeach()
#
#add_custom_target(shaders ALL DEPENDS ${SPIRV_SHADERS})
#
## =============================
## 4. AddressSanitizer
## =============================
#
if(ASSAN AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  message(STATUS "Enabling AddressSanitizer")

  target_compile_options(${PROJECT} PRIVATE -fsanitize=address,undefined)
  target_link_options(${PROJECT} PRIVATE -fsanitize=address,undefined)
endif()
#
#