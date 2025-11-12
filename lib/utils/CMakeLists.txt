# =============================
# 1. Create library
# =============================
set(PROJECT ${PROJECT_NAME}_UTILS)
add_library(${PROJECT} STATIC)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/*.hpp"
)

target_sources(${PROJECT} PRIVATE ${PROJECT_SOURCES})
target_compile_features(${PROJECT} PRIVATE cxx_std_23)
target_compile_options(${PROJECT} PRIVATE ${FLAGS})
target_compile_definitions(${PROJECT} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_link_libraries(${PROJECT} PUBLIC glfw Vulkan::Vulkan TBB::tbb)

# =============================
# 3. Include directories
# =============================

target_include_directories(${PROJECT}
  PUBLIC
  "${CMAKE_CURRENT_LIST_DIR}"
)
