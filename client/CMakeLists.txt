add_executable(${PROJECT_NAME}-Client)

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/*.hpp"
)

target_sources(${PROJECT_NAME}-Client PRIVATE ${CLIENT_SOURCES})
target_compile_features(${PROJECT_NAME}-Client PRIVATE cxx_std_23)
target_compile_options(${PROJECT_NAME}-Client PRIVATE ${FLAGS})
target_compile_definitions(${PROJECT_NAME}-Client PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_link_libraries(${PROJECT_NAME}-Client PRIVATE Vulkan::Vulkan volk glfw)

target_include_directories(${PROJECT_NAME}-Client
  SYSTEM PRIVATE
  ${vulkan_headers_SOURCE_DIR}/include
)

set(CLIENT_INCLUDE_DIRS "")
foreach(header ${CLIENT_SOURCES})
  get_filename_component(dir ${header} DIRECTORY)
  list(APPEND CLIENT_INCLUDE_DIRS ${dir})
endforeach()

list(REMOVE_DUPLICATES CLIENT_INCLUDE_DIRS)
target_include_directories(${PROJECT_NAME}-Client PRIVATE ${CLIENT_INCLUDE_DIRS})

# ===========================================================================================================

find_program(GLSLANG_VALIDATOR glslangValidator)
if(NOT GLSLANG_VALIDATOR)
  message(FATAL_ERROR "glslangValidator not found!")
endif()

set(SHADER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SPIRV_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SPIRV_OUTPUT_DIR})

file(GLOB_RECURSE SHADERS "${SHADER_SRC_DIR}/*.vert" "${SHADER_SRC_DIR}/*.frag")

set(SPIRV_SHADERS "")

foreach(SHADER ${SHADERS})
  get_filename_component(FILENAME ${SHADER} NAME)
  set(SPIRV ${SPIRV_OUTPUT_DIR}/${FILENAME}.spv)

  get_filename_component(EXT ${SHADER} EXT)
  if(EXT STREQUAL ".vert")
    set(STAGE vert)
  elseif(EXT STREQUAL ".frag")
    set(STAGE frag)
  else()
    message(FATAL_ERROR "shader type not foutnd ${FILENAME}")
  endif()

  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSLANG_VALIDATOR} -V -S ${STAGE} ${SHADER} -o ${SPIRV}
    DEPENDS ${SHADER}
    COMMENT "copping shader: ${FILENAME}"
    VERBATIM
  )

  list(APPEND SPIRV_SHADERS ${SPIRV})
endforeach()

add_custom_target(shaders ALL DEPENDS ${SPIRV_SHADERS})

# ===========================================================================================================

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND ASSAN)
  message(STATUS "Enabling AddressSanitizer")
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(${PROJECT_NAME}-Client PRIVATE -fsanitize=address)
    target_link_options(${PROJECT_NAME}-Client PRIVATE -fsanitize=address)
  endif()
endif()

