cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 14)
project(Houdini_Vox LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH "$ENV{HT}/cmake")
find_package(Houdini)
message (STATUS "Houdini HDK: ${Houdini_VERSION}")
message("-- Building plugin for Houdini")

add_library(GEO_Vox SHARED
        GEO_Vox.h
        GEO_Vox.C
        )

target_link_libraries( GEO_Vox  Houdini )
houdini_configure_target( GEO_Vox )



