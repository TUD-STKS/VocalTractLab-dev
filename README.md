![CMake Linux](https://github.com/TUD-STKS/VocalTractLab-dev/actions/workflows/cmake_linux.yml/badge.svg) ![CMake Windows](https://github.com/TUD-STKS/VocalTractLab-dev/actions/workflows/cmake_windows.yml/badge.svg) ![CMake OSX](https://github.com/TUD-STKS/VocalTractLab-dev/actions/workflows/cmake_osx.yml/badge.svg) ![MSBuild](https://github.com/TUD-STKS/VocalTractLab-dev/actions/workflows/msbuild.yml/badge.svg)

# VocalTractLab-dev

This is the VocalTractLab repository for on-going development work. *This is not the place for official stable releases!* 

You can find the bleeding edge builds here including new and experimental features, some of which may break your old projects. For official stable releases, please check [the VocalTractLab website](https://www.vocaltractlab.de).

Please feel free to fork this repo and make your own contributions, though! 

The main branch of this repo is reviewed on a semi-regular basis for inclusion into the official release.

## Clone the repository
This repository includes the VocalTractLabBackend library as a submodule. You therefore need to clone it slightly differently than what you may be used to:

```
git clone --recurse-submodules https://github.com/TUD-STKS/VocalTractLab-dev.git
```

## Build instructions
VocalTractLab depends on the following additional libraries:

- VocalTractLabBackend 
- wxWidgets 3.1.3
- OpenGL
- OpenAL (on Windows or Linux) or WinMM (on Windows)

The VocalTractLabBackend library is included as a submodule in this repo and built as part of the frontend build workflow.

### Install dependencies on Linux (tested on Ubuntu)
You can install the dependencies and build VocalTractLab by simply running the script `build/linux/build.sh`. If for whatever reason you want to walk through the steps manually, here is what you need to do.

First we need to install OpenGL and OpenAL:

```bash
sudo apt-get update 
sudo apt-get install libgtk-3-dev libopenal-dev mesa-utils freeglut3-dev
```

Now you should be ready to install wxWidgets. Unfortunately, the current wxWidgets binaries in the APT repositories are still version 3.0. So we need to download and build wxWidgets from source like so:

```bash
wget https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.3/wxWidgets-3.1.3.tar.bz2
tar -xf wxWidgets-3.1.3.tar.bz2
cd wxWidgets-3.1.3
mkdir buildgtk
cd buildgtk
../configure --with-gtk --with-opengl
make
sudo make install
sudo ldconfig
```

That should be it.

### Install dependencies on Mac OS X 
You can install the dependencies and build VocalTractLab by simply running the script `build/osx/build.sh`. If for whatever reason you want to walk through the steps manually, here is what you need to do.

Mac OS X already ships with OpenGL and OpenAL built-in. You just need to build and install wxWidgets:

```bash
wget https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.3/wxWidgets-3.1.3.tar.bz2
tar -xf wxWidgets-3.1.3.tar.bz2
cd wxWidgets-3.1.3
mkdir buildosx
cd buildosx
../configure --with-opengl
make
sudo make install
```

### Install dependencies on Windows
You can install the dependencies and build VocalTractLab by simply running the script `build/msw/build.ps1`. If for whatever reason you want to walk through the steps manually, here is what you need to do.
On Windows, OpenGL and WinMM are part of the Windows SDK and should already be available on your system. All that is left to do is build wxWidgets. Bring up a Powershell and run the following commands:

```pwsh
Invoke-WebRequest https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.3/wxWidgets-3.1.3.zip -outfile wxWidgets-3.1.3.zip
Expand-Archive wxWidgets-3.1.3.zip
```

Now open the solution `wx_vc16.sln` in the folder `wxWidgets-3.1.3\build\msw` using Visual Studio 2019 or above and build all projects.
Once all libraries are built, copy them to the location where the VocalTractLab build files expect them. Bring up a Powershell again and run (from the parent directory of wxWidgets-3.1.3):

```pwsh
xcopy wxWidgets-3.1.3\* C:\wxwidgets-3.1.3\ /s /h /e
```

### Build VocalTractLab using CMake (Linux, Mac OS X, Windows)
You can build VocalTractLab on Linux and Windows using CMake like so (starting from the repository root folder):

```bash
mkdir tmp
cd tmp
cmake ..
cmake --build . --target VocalTractLab --config Release
```

On Mac OS X, you need the specify the build type (a.k.a. configuration) when the build files are generated:

```bash
mkdir tmp
cd tmp
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target VocalTractLab
```

### Build VocalTractLab using Visual Studio (Windows)
Alternatively to CMake (see above), you can build VocalTractLab on Windows using the provided Visual Studio solution `VocalTractLab2.sln` in the directory `build/msw`. Simply open it with Visual Studio 2019 or above and build all projects.

## Running VocalTractLab
After building the program, the executable file can be found in `bin/Release/`. Before running it, you may want to copy the speaker file `JD3.speaker` from the folder `resources/` to the same folder as the executable. Otherwise you will have to load it at runtime.

## Getting started
Check out the official [manual](https://www.vocaltractlab.de/download-vocaltractlab/VTL2.3-manual.pdf) for detailed instructions and examples. Note that the manual is only updated with official releases, so not everything may still apply to the most recent dev release from this repository.

You can also find some example gestural scores and segment sequence files in the folder `examples/`.

## Troubleshooting
If you run into any problems in the build process, make sure all the dependencies are correctly installed. If you still run into any trouble, feel free to open an issue.


