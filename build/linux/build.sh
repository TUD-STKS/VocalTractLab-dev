#!/bin/bash
# Install dependencies and build VocalTractLab
sudo apt-get update
sudo apt-get install libgtk-3-dev libopenal-dev mesa-utils freeglut3-dev
wget https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.3/wxWidgets-3.1.3.tar.bz2
tar -xf wxWidgets-3.1.3.tar.bz2
cd wxWidgets-3.1.3
mkdir buildgtk
cd buildgtk
../configure --with-gtk --with-opengl
make
sudo make install
sudo ldconfig
mkdir ../../../../tmp
cd ../../../../tmp
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target VocalTractLab --config Release
