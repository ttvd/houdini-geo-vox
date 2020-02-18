workspace "GEO_Vox"

    -- Add --houdini-version option.
    newoption {
        trigger = "houdini-version",
        value = "version",
        description = "Version of Houdini to build against"
    }

    -- Make sure the version has been specified.
    if not _OPTIONS["houdini-version"] then
        premake.error("Please specify Houdini version via --houdini-version or run help.")
    end

    -- Extract Houdini version.
    local version_houdini = _OPTIONS["houdini-version"]

    -- Construct path to Houdini installation.
    local path_houdini = ""
    local path_houdini_target = ""

    if os.host() == "windows" then
        path_houdini = "C:/Program Files/Side Effects Software/Houdini " .. version_houdini
        path_houdini_target = path_houdini
    elseif os.host() == "linux" then
        if os.target() == "windows" then
            path_houdini = "/mnt/c/Program Files/Side Effects Software/Houdini " .. version_houdini
            path_houdini_target = "C:/Program Files/Side Effects Software/Houdini " .. version_houdini
        else
            premake.error("Unsupported platform.")
        end
    else
        premake.error("Unsupported platform.")
    end

    -- Check if path is valid.
    if not os.isdir(path_houdini) then
        premake.error(string.format("Invalid Houdini installation path provided: %s", path_houdini))
    end

    -- Make version file path.
    local version_file_houdini = path_houdini .. "/toolkit/include/sys/SYS_Version.h"

    -- Make sure version file exists.
    if not os.isfile(version_file_houdini) then
        print(version_file_houdini)
        premake.error(string.format("Version file not found for Houdini installation: %s", path_houdini))
    end

    -- Set the sdk.
    systemversion(last_sdk)

    configurations { "Debug", "Release" }
    platforms { "Win64" }
    architecture "x64"
    location "_build"

    buildoptions {
        "/TP",
        "/bigobj",
        "/GR",
        "/Zc:forScope",
        "/wd4355",
        "/w14996",
        "/MD",
        "/EHsc",
        "/nologo"
    }

    defines {
        "VERSION=" .. version_houdini,
        "WIN32",
        "I386",
        "MAKING_DSO",
        "AMD64",
        "SIZEOF_VOID_P=8",
        "SESI_LITTLE_ENDIAN",
        "FBX_ENABLED=1",
        "OPENCL_ENABLED=1",
        "OPENVDB_ENABLED=1",
        "_REENTRANT",
        "_FILE_OFFSET_BITS=64",
        "_WIN32_WINNT=0x0502",
        "NOMINMAX",
        "_USE_MATH_DEFINES",            
        "SWAP_BITFIELDS",
        "STRICT",
        "WIN32_LEAN_AND_MEAN",
        "_CRT_SECURE_NO_DEPRECATE",
        "_CRT_NONSTDC_NO_DEPRECATE",
        "_SCL_SECURE_NO_WARNINGS",
        "NDEBUG",
        "HBOOST_ALL_NO_LIB"
    }

    includedirs {
        path_houdini_target .. "/toolkit/include"
    }

    libdirs {
        path_houdini_target .. "/custom/houdini/dsolib",
        path_houdini_target .. "/bin"
    }

    startproject "GEO_Vox"

    project "GEO_Vox"

    kind "SharedLib"
    language "C++"

    files {
        "GEO_Vox.C",
        "GEO_Vox.h"
    }

    links {
        "libUT",
        "libGEO",
        "libGA",
        "libGU",
        "libSYS"
    }
