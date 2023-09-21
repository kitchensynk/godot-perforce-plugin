<img src="/godot-p4.png" width="25%" />

# Godot Perforce Plugin
Adds support for Helix Core P4 in Godot 4.1

## Installation Instructions

>BINARIES NOT AVAILABLE YET

## Build Instructions

### Required Tools
>ONLY WINDOWS SUPPORTED AT THE MOMENT
- Full copy of the source code. Remember to use `git clone --recursive`.
- [SCons](https://scons.org/pages/download.html) (v4.5+)
- [P4API C++](https://www.perforce.com/downloads/helix-core-c/c-api) (v2023.1)
- [OpenSSL](https://www.openssl.org/source/old/1.0.2/) (v1.0.2t)
- C++17 and C90 compilers detectable by SCons and present in `PATH`.



### Build OpenSSL
>OPENSSL LIBRARIES NOT INCLUDED, WILL ADD LATER

OpenSSL includes platform specific build instructions in `INSTALL.md`.

The following Static Libraries need to be included in `src/thirdparty/openssl`
- Windows
  - `ssleay32.lib`
  - `libeay32.lib`

The following Dynamic Libraries 
need to be included in your godot addons directory `res://addons/win64`
- Windows
  - `ssleay32.dll`
  - `libeay32.dll`
    



### Building P4API

P4API files need to be added to `src/thirdparty/p4`. You should have a `p4/include` and `p4/lib` folder after copying.

>CURRENTLY godot-cpp and godot-perforce-plugin ARE COMPILED SEPARATELY, FIX

#### Determining OpenSSL version

P4API depends on a specific version of OpenSSL. To determine the version, run the following windows command on the P4API library directory:
```
strings librpc.lib | findstr /B OpenSSL
```

>Note: If you have `libcrypto.lib` or `libssl.lib`, these libraries indicate you are using `OpenSLL 1.1.1` or higher




### Build Godot CPP

[Guide to compile godot-cpp](https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html)

### Build Godot P4 Plugin

```
scons platform=<platform>
```

### Additional Reference
[Compiling And Linking Helix Core Applications](https://www.perforce.com/manuals/p4api/Content/P4API/client.programming.compiling.html#Compiling_and_linking_Helix_Server_applications)

## License
This plugin is under the MIT License. Third-party notices are present in THIRDPARTY.md.

OpenSSL License Attributions - This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit. (http://www.openssl.org/). This product includes cryptographic software written by Eric Young (eay@cryptsoft.com)




 
