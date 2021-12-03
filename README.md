![CMake](https://github.com/TUD-STKS/VocalTractLab-dev/actions/workflows/cmake.yml/badge.svg) ![MSBuild](https://github.com/TUD-STKS/VocalTractLab-dev/actions/workflows/msbuild.yml/badge.svg)

# VocalTractLab-dev

This is the VocalTractLab repository for on-going development work. *This is not the place for official stable releases!* 

You can find the bleeding edge builds here including new and experimental features, some of which may break your old projects. For official stable releases, please check [the VocalTractLab website](https://www.vocaltractlab.de).

Please feel free to fork this repo and make your own contributions, though! 

The main branch of this repo is reviewed on a semi-regular basis for inclusion into the official release.


## Build instructions
- Use [vcpkg to install wxWidgets](https://www.wxwidgets.org/blog/2019/01/wxwidgets-and-vcpkg/)
- Build using CMake:

```
mkdir tmp
cd tmp
cmake ..
cmake --build . --target VocalTractLab --config Release
```

Currently only Debug and Release configurations for Windows x64 are provided. If you are willing to contribute a Linux configuration, please do!
