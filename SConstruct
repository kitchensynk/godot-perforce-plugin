#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

env.Replace(CCFLAGS = ['/MT'])

env.Append(CXXFLAGS = ['/DOS_NT /DCASE_INSENSITIVE'])


libs1 = "Shlwapi.lib"
libs2 = "ws2_32.lib"
libs3 = "crypt32.lib"

libs4 = "src/thirdparty/openssl/ssleay32.lib"
libs5 = "src/thirdparty/openssl/libeay32.lib"
libs6 = "src/thirdparty/p4/libclient.lib"        
libs7 = "src/thirdparty/p4/librpc.lib"      
libs8 = "src/thirdparty/p4/libsupp.lib"         
libs9 = "src/thirdparty/p4/libp4api.lib"

libs10 = "kernel32.lib"
libs11 = "user32.lib"
libs12 = "gdi32.lib"
libs13 = "winspool.lib"
libs14 = "shell32.lib"
libs15 = "ole32.lib"
libs16 = "oleaut32.lib"
libs17 = "uuid.lib"
libs18 = "comdlg32.lib"
libs19 = "advapi32.lib"

env.Append(LIBS = [libs1,libs2,libs3,libs4,libs5,libs6,libs7,libs8,libs9,libs10,libs11,libs12,libs13,libs14,libs15,libs16,libs17,libs18,libs19])

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/bin/p4_plugin.{}.{}.framework/p4_plugin.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "demo/addons/win64/p4_plugin{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources
    )


Default(library)
