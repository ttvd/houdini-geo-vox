cmake_minimum_required(VERSION 3.1)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
cmake_policy(SET CMP0003 NEW)
include(CheckCXXCompilerFlag)

project(HoudiniGeoVox)

# Detect C++11 or C++0x.
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Function to extract Houdini version from a Version file.
function(extract_houdini_version parm_hfs)
    set(_version_file ${parm_hfs}/toolkit/include/SYS/SYS_Version.h)
    if(EXISTS ${_version_file})
        file(READ ${_version_file} _version_file_contents)
        string(REGEX MATCH "#define SYS_VERSION_FULL \"([0-9]+\.[0-9]+\.[0-9]+)\"" _version_line ${_version_file_contents})
        string(REGEX REPLACE "#define SYS_VERSION_FULL \"([0-9]+\.[0-9]+\.[0-9]+)\"" "\\1" HOUDINI_GEOVOX_VERSION_FULL ${_version_line})
        set(HOUDINI_GEOVOX_VERSION_FULL ${HOUDINI_GEOVOX_VERSION_FULL} PARENT_SCOPE)
    else()
        message(FATAL " ${parm_hfs} does not point to a valid Houdini baseline.")
    endif()
endfunction()

if(APPLE)

    find_library(GEO_SLAPI_FRAMEWORK SketchUpAPI)

    if(DEFINED ENV{HOUDINI_VERSION})
        set(HOUDINI_GEOVOX_VERSION_FULL $ENV{HOUDINI_VERSION})
        set(HOUDINI_GEOVOX_HFS /Library/Frameworks/Houdini.Framework/Versions/${HOUDINI_GEOVOX_VERSION_FULL})
        set(_version_file /Library/Frameworks/Houdini.Framework/Versions/${HOUDINI_GEOVOX_VERSION_FULL}/Resources/toolkit/include/SYS/SYS_Version.h)
        if(NOT EXISTS ${_version_file})
            message(FATAL " Houdini ${HOUDINI_GEOVOX_VERSION_FULL} is not installed.")
        endif()
    elseif(DEFINED ENV{HFS})
        set(HOUDINI_GEOVOX_HFS $ENV{HFS})
        if(EXISTS ${HOUDINI_GEOVOX_HFS}/Frameworks/Houdini.Framework/Versions/Current/Resources)
            extract_houdini_version(${HOUDINI_GEOVOX_HFS}/Frameworks/Houdini.Framework/Versions/Current/Resources)
        else()
            extract_houdini_version(${HOUDINI_GEOVOX_HFS}/Resources)
        endif()
    else()
        message(FATAL " Please define either HOUDINI_VERSION or HFS variable.")
    endif()

    string(REGEX MATCH "([0-9]+\.[0-9]+)" HOUDINI_GEOVOX_VERSION ${HOUDINI_GEOVOX_VERSION_FULL})
    set(HOUDINI_GEOVOX_VERSION ${HOUDINI_GEOVOX_VERSION})

    # Path to where DSOs are installed for this version.
    set(HOUDINI_GEOVOX_DSO_PATH $ENV{HOME}/Library/Preferences/houdini/${HOUDINI_GEOVOX_VERSION}/dso)

    set(HOUDINI_GEOVOX_INCLUDE_DIRECTORIES
        ${HOUDINI_GEOVOX_HFS}/Resources/toolkit/include
    )

    set(HOUDINI_GEOVOX_LINK_DIRECTORIES
        ${HOUDINI_GEOVOX_HFS}/Libraries
    )

    # Remove lib prefix on OS X.
    set(CMAKE_SHARED_MODULE_PREFIX "")

    # Set dylib prefix for modules on OS X.
    set(CMAKE_SHARED_MODULE_SUFFIX ".dylib")

    # OS X specific definitions.
    add_definitions(
        -DVERSION=\"${HOUDINI_GEOVOX_VERSION_FULL}\"

        -DMBSD_INTEL
        -DMBSD_COCOA
        -DMBSD
        -DUSE_PTHREADS
        -D_GNU_SOURCE
        -DGCC4
        -DGCC3
        "-arch x86_64"
        "-mmacosx-version-min=10.9"
    )

    # Disable rpath.
    set(CMAKE_SKIP_RPATH TRUE)

    # Module creation flags.
    string(REPLACE "-Wl,-headerpad_max_install_names" "" CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS "${CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS}")
    set(CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS "${CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS} -flat_namespace -undefined suppress -arch x86_64 -fobjc-gc")

else()
    message(FATAL_ERROR "Unsupported platform.")
endif()

# Add common definitions.
add_definitions(
    -DMAKING_DSO

    -DAMD64
    -DSIZEOF_VOID_P=8
    -DSESI_LITTLE_ENDIAN
    -DFBX_ENABLED=1
    -DOPENCL_ENABLED=1
    -DOPENVDB_ENABLED=1
    -DENABLE_THREADS
    -DUSE_PTHREADS
    -D_REENTRANT
    -D_FILE_OFFSET_BITS=64
)

# Set include directories.
include_directories(${HOUDINI_GEOVOX_INCLUDE_DIRECTORIES})

# Set link directories.
link_directories(${HOUDINI_GEOVOX_LINK_DIRECTORIES})

# Set output directory.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

message(STATUS "HOUDINI_VERSION=${HOUDINI_GEOVOX_VERSION_FULL}")
message(STATUS "HFS=${HOUDINI_GEOVOX_HFS}")

set(HOUDINI_GEOVOX_INCLUDE
    GEO_Vox.h
)

set(HOUDINI_GEOVOX_SOURCE
    GEO_Vox.C
)

set(HOUDINI_GEOVOX_LIBS
    HoudiniUT
    HoudiniGEO
)

add_definitions(
    -DUT_DSO_TAGINFO=\"3262197cbf1f501f2caa0ca8572890fb3e4b9bfcc021d8a873a8c3063d69ee812b972985db32267a5946addc2f8f31780eadf501ffc2e85149062ee263fe81be4baf508aff924994a3268d0847f848e6ab4e98a984ed956599\")

add_library(GEO_Vox MODULE ${HOUDINI_GEOVOX_INCLUDE} ${HOUDINI_GEOVOX_SOURCE})
target_link_libraries(GEO_Vox ${HOUDINI_GEOVOX_LIBS})
