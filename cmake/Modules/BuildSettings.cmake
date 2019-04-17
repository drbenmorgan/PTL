################################################################################
#
#        Handles the build settings
#
################################################################################

include(GNUInstallDirs)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

# ---------------------------------------------------------------------------- #
# check C flag
macro(ADD_C_FLAG_IF_AVAIL FLAG)
    if(NOT "${FLAG}" STREQUAL "")
        string(REGEX REPLACE "^-" "c_" FLAG_NAME "${FLAG}")
        string(REPLACE "-" "_" FLAG_NAME "${FLAG_NAME}")
        string(REPLACE " " "_" FLAG_NAME "${FLAG_NAME}")
        string(REPLACE "=" "_" FLAG_NAME "${FLAG_NAME}")
        check_c_compiler_flag("${FLAG}" ${FLAG_NAME})
        if(${FLAG_NAME})
            list(APPEND ${PROJECT_NAME}_C_FLAGS "${FLAG}")
        endif()
    endif()
endmacro()


# ---------------------------------------------------------------------------- #
# check CXX flag
macro(ADD_CXX_FLAG_IF_AVAIL FLAG)
    if(NOT "${FLAG}" STREQUAL "")
        string(REGEX REPLACE "^-" "cxx_" FLAG_NAME "${FLAG}")
        string(REPLACE "-" "_" FLAG_NAME "${FLAG_NAME}")
        string(REPLACE " " "_" FLAG_NAME "${FLAG_NAME}")
        string(REPLACE "=" "_" FLAG_NAME "${FLAG_NAME}")
        check_cxx_compiler_flag("${FLAG}" ${FLAG_NAME})
        if(${FLAG_NAME})
            list(APPEND ${PROJECT_NAME}_CXX_FLAGS "${FLAG}")
        endif()
    endif()
endmacro()


# ---------------------------------------------------------------------------- #
#
set(SANITIZE_TYPE leak CACHE STRING "-fsantitize=<TYPE>")
set(CMAKE_INSTALL_MESSAGE LAZY)
if(WIN32)
    set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ STL standard")
else(WIN32)
    set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ STL standard")
endif(WIN32)

# ---------------------------------------------------------------------------- #
# set the output directory (critical on Windows

foreach(_TYPE ARCHIVE LIBRARY RUNTIME)
    # if ${PROJECT_NAME}_OUTPUT_DIR is not defined, set to CMAKE_BINARY_DIR
    if(NOT DEFINED ${PROJECT_NAME}_OUTPUT_DIR OR "${${PROJECT_NAME}_OUTPUT_DIR}" STREQUAL "")
        set(${PROJECT_NAME}_OUTPUT_DIR ${CMAKE_BINARY_DIR})
    endif(NOT DEFINED ${PROJECT_NAME}_OUTPUT_DIR OR "${${PROJECT_NAME}_OUTPUT_DIR}" STREQUAL "")
    # set the CMAKE_{ARCHIVE,LIBRARY,RUNTIME}_OUTPUT_DIRECTORY variables
    if(WIN32)
        # on Windows, separate types into different directories
        string(TOLOWER "${_TYPE}" _LTYPE)
        set(CMAKE_${_TYPE}_OUTPUT_DIRECTORY ${${PROJECT_NAME}_OUTPUT_DIR}/outputs/${_LTYPE})
    else(WIN32)
        # on UNIX, just set to same directory
        set(CMAKE_${_TYPE}_OUTPUT_DIRECTORY ${${PROJECT_NAME}_OUTPUT_DIR})
    endif(WIN32)
endforeach(_TYPE ARCHIVE LIBRARY RUNTIME)


# ---------------------------------------------------------------------------- #
# used by configure_package_*
#
set(LIBNAME ptl)

# ---------------------------------------------------------------------------- #
#  debug macro
#
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    list(APPEND ${PROJECT_NAME}_DEFINITIONS DEBUG)
else("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    list(APPEND ${PROJECT_NAME}_DEFINITIONS NDEBUG)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

# ---------------------------------------------------------------------------- #
# set the compiler flags
#
add_c_flag_if_avail("-W")
if(NOT WIN32)
    add_c_flag_if_avail("-Wall")
endif()
add_c_flag_if_avail("-Wextra")
add_c_flag_if_avail("-std=c11")
if(NOT c_std_c11)
    add_c_flag_if_avail("-std=c99")
endif()

# SIMD OpenMP
add_c_flag_if_avail("-fopenmp-simd")

# general warnings
add_cxx_flag_if_avail("-W")
if(NOT WIN32)
    add_cxx_flag_if_avail("-Wall")
endif()
add_cxx_flag_if_avail("-Wextra")
add_cxx_flag_if_avail("-Wshadow")
add_cxx_flag_if_avail("-faligned-new")

# SIMD OpenMP
add_cxx_flag_if_avail("-fopenmp-simd")

if(PTL_USE_ARCH)
    if(CMAKE_C_COMPILER_IS_INTEL)
        add_c_flag_if_avail("-xHOST")
    else()
        add_c_flag_if_avail("-march=native")
        add_c_flag_if_avail("-mtune=native")
        add_c_flag_if_avail("-msse2")
        add_c_flag_if_avail("-msse3")
        add_c_flag_if_avail("-mssse3")
        add_c_flag_if_avail("-msse4")
        add_c_flag_if_avail("-msse4.1")
        add_c_flag_if_avail("-msse4.2")
        add_c_flag_if_avail("-mavx")
        add_c_flag_if_avail("-mavx2")
    endif()

    if(CMAKE_CXX_COMPILER_IS_INTEL)
        add_cxx_flag_if_avail("-xHOST")
    else()
        add_cxx_flag_if_avail("-march=native")
        add_cxx_flag_if_avail("-mtune=native")
        add_cxx_flag_if_avail("-msse2")
        add_cxx_flag_if_avail("-msse3")
        add_cxx_flag_if_avail("-mssse3")
        add_cxx_flag_if_avail("-msse4")
        add_cxx_flag_if_avail("-msse4.1")
        add_cxx_flag_if_avail("-msse4.2")
        add_cxx_flag_if_avail("-mavx")
        add_cxx_flag_if_avail("-mavx2")
    endif()

    if(PTL_USE_AVX512)
        if(CMAKE_C_COMPILER_IS_INTEL)
            add_c_flag_if_avail("-axMIC-AVX512")
        else()
            add_c_flag_if_avail("-mavx512f")
            add_c_flag_if_avail("-mavx512pf")
            add_c_flag_if_avail("-mavx512er")
            add_c_flag_if_avail("-mavx512cd")
            add_c_flag_if_avail("-mavx512vl")
            add_c_flag_if_avail("-mavx512bw")
            add_c_flag_if_avail("-mavx512dq")
            add_c_flag_if_avail("-mavx512ifma")
            add_c_flag_if_avail("-mavx512vbmi")
        endif()

        if(CMAKE_CXX_COMPILER_IS_INTEL)
            add_cxx_flag_if_avail("-axMIC-AVX512")
        else()
            add_cxx_flag_if_avail("-mavx512f")
            add_cxx_flag_if_avail("-mavx512pf")
            add_cxx_flag_if_avail("-mavx512er")
            add_cxx_flag_if_avail("-mavx512cd")
            add_cxx_flag_if_avail("-mavx512vl")
            add_cxx_flag_if_avail("-mavx512bw")
            add_cxx_flag_if_avail("-mavx512dq")
            add_cxx_flag_if_avail("-mavx512ifma")
            add_cxx_flag_if_avail("-mavx512vbmi")
        endif()
    endif()
endif()

if(PTL_USE_SANITIZER)
    add_c_flag_if_avail("-fsanitize=${PTL_SANITIZER_TYPE}")
    add_cxx_flag_if_avail("-fsanitize=${PTL_SANITIZER_TYPE}")
    if(cxx_fsanitize_${PTL_SANITIZER_TYPE})
        if("${PTL_SANITIZER_TYPE}" STREQUAL "leak")
            list(APPEND EXTERNAL_LIBRARIES lsan)
        elseif("${PTL_SANITIZER_TYPE}" STREQUAL "address")
            list(APPEND EXTERNAL_LIBRARIES asan)
        elseif("${PTL_SANITIZER_TYPE}" STREQUAL "memory")
            list(APPEND EXTERNAL_LIBRARIES msan)
        elseif("${PTL_SANITIZER_TYPE}" STREQUAL "thread")
            list(APPEND EXTERNAL_LIBRARIES tsan)
        endif()
    endif()
endif()

if(PTL_USE_PROFILE)
    add_c_flag_if_avail("-p")
    add_c_flag_if_avail("-pg")
    add_c_flag_if_avail("-fbranch-probabilities")
    if(c_fbranch_probabilities)
        add(PROJECT_C_FLAGS "-fprofile-arcs")
        #add(PROJECT_C_FLAGS "-fprofile-dir=${CMAKE_BINARY_DIR}")
        #add(PROJECT_C_FLAGS "-fprofile-generate=${CMAKE_BINARY_DIR}/profile")
    endif()
    add_cxx_flag_if_avail("-p")
    add_cxx_flag_if_avail("-pg")
    add_cxx_flag_if_avail("-fbranch-probabilities")
    if(cxx_fbranch_probabilities)
        add(PROJECT_CXX_FLAGS "-fprofile-arcs")
        #add(PROJECT_CXX_FLAGS "-fprofile-dir=${CMAKE_BINARY_DIR}")
        #add(PROJECT_CXX_FLAGS "-fprofile-generate=${CMAKE_BINARY_DIR}/profile")
        add(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs")
        add_feature(CMAKE_EXE_LINKER_FLAGS "Linker flags")
    endif()
endif()

if(PTL_USE_COVERAGE)
    add_c_flag_if_avail("-ftest-coverage")
    if(c_ftest_coverage)
        list(APPEND ${PROJECT_NAME}_C_FLAGS "-fprofile-arcs")
    endif()
    add_cxx_flag_if_avail("-ftest-coverage")
    if(cxx_ftest_coverage)
        list(APPEND ${PROJECT_NAME}_CXX_FLAGS "-fprofile-arcs")
        add(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs")
        add_feature(CMAKE_EXE_LINKER_FLAGS "Linker flags")
    endif()
endif()

# ---------------------------------------------------------------------------- #
# user customization
to_list(_CFLAGS "${CFLAGS};$ENV{CFLAGS}")
foreach(_FLAG ${_CFLAGS})
    add_c_flag_if_avail("${_FLAG}")
endforeach()

to_list(_CXXFLAGS "${CXXFLAGS};$ENV{CXXFLAGS}")
foreach(_FLAG ${_CXXFLAGS})
    add_cxx_flag_if_avail("${_FLAG}")
endforeach()
