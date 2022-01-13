#!/bin/bash
# Install dependencies and build VocalTractLab
wget https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.3/wxWidgets-3.1.3.tar.bz2
tar -xf wxWidgets-3.1.3.tar.bz2
cd wxWidgets-3.1.3
mkdir buildosx
cd buildosx
../configure --with-opengl
make
sudo make install
mkdir ../../../../tmp
cd ../../../../tmp
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target VocalTractLab --config Release
