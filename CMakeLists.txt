cmake_minimum_required(VERSION 3.20)
project("Vulkan" "CXX")

if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# ----------------------------
# 1. Compiler flags
# ----------------------------

if(MSVC)
  set(COMMON_FLAGS
    /W4 # Sets warning level to 4, which is very strict.
    /Wall # Enables all warnings.
    /WX # Treats all warnings as errors, stopping compilation.
    /permissive- # Enforces stricter standard C++ conformance.
    /EHsc # Specifies the exception-handling model for C++ code.
  )
  set(DEBUG_FLAGS
    /Zi # Enables generation of a program database (PDB) for debugging.
    /Od # Disables optimization for faster builds and easier debugging.
    /MDd # Uses the debug multithreaded dynamic-link library (DLL) version of the C runtime library.
  )
  set(RELEASE_FLAGS
    /O2 # Enables full optimization for speed.
    /Oi # Enables intrinsic functions.
    /Ot # Favors code speed over size.
    /Ob2 # Expands some functions inline.
    /Gy # Enables function-level linking.
    /GL # Enables whole program optimization during linking.
    /fp:fast # Enables faster, less precise floating-point models.
    /MD # Uses the multithreaded dynamic-link library (DLL) version of the C runtime library.
  )
else()
  set(COMMON_FLAGS
    -Wall # Enables a set of common warnings.
    -Wextra # Enables extra, potentially useful warnings not covered by -Wall.
    -Wpedantic # Issues all warnings demanded by strict ISO C and ISO C++.
    -Wconversion # Warns for implicit type conversions that may change a value.
    -Wsign-conversion # Warns for conversions that change the sign of a value.
    -Wshadow=local # Warns when a local variable shadows a global variable.
    -Wnon-virtual-dtor # Warns if a class has a virtual function but non-virtual destructor.
    -Werror # Treats all warnings as errors.
    -Wold-style-cast # Warns about C-style casts in C++ code.
    -Wcast-align # Warns when a pointer cast increases the alignment requirement.
    -Woverloaded-virtual # Warns about a derived class virtual function hiding a base class virtual function.
    -Wdouble-promotion # Warns about implicit promotion of float to double.
    -Wformat=2 # Checks format string arguments more stringently.
    -Wundef # Warns if an undefined identifier is evaluated in a preprocessor directive.
    -Wpointer-arith # Warns about some questionable pointer arithmetic.
    -Wwrite-strings # Makes string literals const, catching attempts to write to them.
    -Wlogical-op # Warns if logical operators (!, &&, ||) are used incorrectly.
    -Wuseless-cast # Warns about casts that have no effect.
    -Wduplicated-cond # Warns about duplicated conditional branches.
    -Wduplicated-branches # Warns about duplicated branches in if/else.
    -Wnull-dereference # Warns about possible null pointer dereferences.
    -Wcast-qual # Warns about casts that cast away a const or volatile qualifier.
    -Wmissing-include-dirs # Warns if a specified include directory does not exist.
    -Wunreachable-code # Warns about code that can never be executed.
    -Wextra-semi # Warns about extra semicolons in class/struct definitions.
    -fno-common # Prevents allocation of common blocks, forcing proper definition of globals.
    -Winit-self # Warns when a variable is initialized with its own uninitialized value. This is a subtle error that can lead to undefined behavior.
    -Wmissing-declarations # Warns about global functions that are defined but have no prior declaration. This helps enforce good coding practices and modularity.
    -Wredundant-decls # Warns about redundant declarations of functions or variables within the same scope. This can indicate a code logic error or sloppy declaration management.
    -Wstrict-overflow=2 # Enables strict checking for potential signed integer overflows, using the most aggressive level of analysis. The compiler uses assumptions that overflows do not occur to perform optimizations, and this flag detects when those assumptions may be invalid.
    -Wfloat-equal # Warns about comparisons of floating-point numbers using == or !=. Due to precision issues, it is often a logical error to compare floating-point values for exact equality.
    -fdiagnostics-color=always
  )
  set(DEBUG_FLAGS
    -O0 # Disables all optimization.
    -g3 # Generates debug information.
    -fno-omit-frame-pointer # Ensures the frame pointer is not omitted, improving backtraces during debugging.
    -pipe # Uses pipes instead of temporary files for communication between stages of compilation.
    -fstack-protector-strong # Inserts stack-smashing protection by adding a "canary" value on the stack to detect buffer overflows. 
  )
  set(RELEASE_FLAGS
    -O3 # Enables a high level of aggressive optimization.
    -march=native # Optimizes code for the CPU where compilation is running.
    -flto # Enables link-time optimization.
    -funroll-loops # Performs loop unrolling optimization.
    -finline-functions # Inlines function calls for optimization.
    -fstrict-aliasing # Enables the compiler to assume certain memory access patterns.
    -fvisibility=hidden # Sets default visibility for symbols to 'hidden', potentially reducing binary size.
  )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(FLAGS ${COMMON_FLAGS} ${RELEASE_FLAGS})
else()
  set(FLAGS ${COMMON_FLAGS} ${DEBUG_FLAGS})
endif()

# ----------------------------
# 2. FetchContent and packages
# ----------------------------

if(WIN32)
  find_package(TBB CONFIG REQUIRED)
else()
  find_package(TBB REQUIRED)
endif()

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

FetchContent_Declare(
  glfw
  GIT_TAG 3.3.10
  GIT_REPOSITORY https://github.com/glfw/glfw)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  vulkan_headers
  GIT_TAG v1.4.331
  GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git)
FetchContent_MakeAvailable(vulkan_headers)

FetchContent_Declare(
  volk
  GIT_TAG 1.4.304
  GIT_REPOSITORY https://github.com/zeux/volk.git)
FetchContent_MakeAvailable(volk)
target_include_directories(volk PRIVATE ${vulkan_headers_SOURCE_DIR}/include)

# ----------------------------
# 3. Vulkan SDK – system or download
# ----------------------------

find_package(Vulkan QUIET)
if(Vulkan_FOUND)
  message(STATUS "Found system Vulkan SDK — using Vulkan::Vulkan")
else()
  message(STATUS "System Vulkan not found — creating Vulkan::Vulkan as interface -> vulkan_headers + volk")

  add_library(Vulkan::Vulkan INTERFACE IMPORTED)
  set_target_properties(Vulkan::Vulkan PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${vulkan_headers_SOURCE_DIR}/include")

  target_link_libraries(Vulkan::Vulkan INTERFACE volk)
endif()

# ----------------------------
# 4. Clang-tidy
# ----------------------------
if(TIDY)
  set(CLANG_TIDY_CHECKS_LIST
    "*"
    "-llvmlibc-*"
    "-llvm-header-guard"
    "-fuchsia-default-arguments-calls"
    "-modernize-use-trailing-return-type"
    "-altera-unroll-loops"
    "-misc-use-anonymous-namespace"
  )
  string(JOIN "," CLANG_TIDY_CHECKS ${CLANG_TIDY_CHECKS_LIST})
  set(CLANG_TIDY_OPTIONS "-config={\"CheckOptions\":[{\"key\":\"readability-identifier-length.IgnoredVariableNames\",\"value\":\"id|it\"}]}")
  set(CMAKE_CXX_CLANG_TIDY
    "clang-tidy"
    "-checks=${CLANG_TIDY_CHECKS}"
    "${CLANG_TIDY_OPTIONS}"
    "--extra-arg=-Wno-unknown-warning-option"
  )
endif()

# ----------------------------
# 5. Add subdir
# ----------------------------

add_subdirectory("lib/utils")
add_subdirectory("lib/vulkan")
add_subdirectory("client")
