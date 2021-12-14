# Get and build wxWidgets
Invoke-WebRequest https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.3/wxWidgets-3.1.3.zip -outfile wxWidgets-3.1.3.zip
Expand-Archive wxWidgets-3.1.3.zip
cd wxWidgets-3.1.3
mkdir buildmsw
cd buildmsw
cmake .. -DwxBUILD_SHARED=OFF
cmake --build . --config Release
xcopy lib\* ..\lib\
xcopy ..\..\wxWidgets-3.1.3\* C:\wxwidgets-3.1.3\ /s /h /e

# Build VocalTractLab
mkdir ..\..\tmp
cd ..\..\tmp
cmake ..
cmake --build . --config Release --target VocalTractLab
