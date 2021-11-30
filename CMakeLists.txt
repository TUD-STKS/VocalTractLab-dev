# CMakeList.txt : CMake project for VocalTractLab, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.16)

project ("VocalTractLab")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_subdirectory("Backend")

if (MSVC)
set(wxWidgets_ROOT_DIR C:/wxwidgets-3.1.3)
endif()
# Request the required wxWidgets libs
find_package(wxWidgets 3.1 COMPONENTS core base gl adv REQUIRED)

# Include the wxWidgets use file to initialize various settings
include(${wxWidgets_USE_FILE})

if (MSVC)
add_definitions(-D
_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D UNICODE -D wxUSE_UNICODE -D _WINDOWS -D __WXMSW__ -D _CRT_SECURE_NO_DEPRECATE -D _CRT_NONSTDC_NO_DEPRECATE -D NDEBUG)
# Add source to this project's executable.
add_executable (VocalTractLab WIN32
src/AnalysisResultsDialog.cpp
src/AnalysisSettingsDialog.cpp
src/AnatomyParamsDialog.cpp
src/AnnotationDialog.cpp
src/Application.cpp
src/AreaFunctionPicture.cpp
src/BasicPicture.cpp
src/ColorScale.cpp
src/CrossSectionPicture.cpp
src/Data.cpp
src/EmaConfigDialog.cpp
src/FdsOptionsDialog.cpp
src/FormantOptimizationDialog.cpp
src/GesturalScorePage.cpp
src/GesturalScorePicture.cpp
src/GlottisDialog.cpp
src/GlottisPanel.cpp
src/GlottisPicture.cpp
src/Graph.cpp
src/LfPulseDialog.cpp
src/LfPulsePicture.cpp
src/MainWindow.cpp
src/PhoneticParamsDialog.cpp
src/PoleZeroDialog.cpp
src/PoleZeroPlot.cpp
src/SignalComparisonPicture.cpp
src/SignalPage.cpp
src/SignalPicture.cpp
src/SilentMessageBox.cpp
src/SimpleSpectrumPicture.cpp
src/SoundLib.cpp
src/SpectrogramPicture.cpp
src/SpectrogramPlot.cpp
src/SpectrumOptionsDialog.cpp
src/SpectrumPicture.cpp
src/SynthesisThread.cpp
src/TdsOptionsDialog.cpp
src/TdsPage.cpp
src/TdsSpatialSignalPicture.cpp
src/TdsTimeSignalPicture.cpp
src/TdsTubePicture.cpp
src/TimeAxisPicture.cpp
src/TransitionDialog.cpp
src/VocalTractDialog.cpp
src/VocalTractPage.cpp
src/VocalTractPicture.cpp
src/VocalTractShapesDialog.cpp
)
set_property(TARGET VocalTractLab PROPERTY VS_DPI_AWARE "PerMonitor")
target_include_directories(VocalTractLab PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Backend/include/VocalTractLabBackend)
target_link_libraries(VocalTractLab VocalTractLabBackend ${wxWidgets_LIBRARIES} )
endif()

if(UNIX)
add_definitions(-D
_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D WXUSINGDLL -D UNICODE -D wxUSE_UNICODE -D _CRT_SECURE_NO_DEPRECATE -D _CRT_NONSTDC_NO_DEPRECATE -D NDEBUG -D HAVE_OPENAL)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenAL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIRS})
# Add source to this project's executable.
add_executable (VocalTractLab
src/AnalysisResultsDialog.cpp
src/AnalysisSettingsDialog.cpp
src/AnatomyParamsDialog.cpp
src/AnnotationDialog.cpp
src/Application.cpp
src/AreaFunctionPicture.cpp
src/BasicPicture.cpp
src/ColorScale.cpp
src/CrossSectionPicture.cpp
src/Data.cpp
src/EmaConfigDialog.cpp
src/FdsOptionsDialog.cpp
src/FormantOptimizationDialog.cpp
src/GesturalScorePage.cpp
src/GesturalScorePicture.cpp
src/GlottisDialog.cpp
src/GlottisPanel.cpp
src/GlottisPicture.cpp
src/Graph.cpp
src/LfPulseDialog.cpp
src/LfPulsePicture.cpp
src/MainWindow.cpp
src/PhoneticParamsDialog.cpp
src/PoleZeroDialog.cpp
src/PoleZeroPlot.cpp
src/SignalComparisonPicture.cpp
src/SignalPage.cpp
src/SignalPicture.cpp
src/SilentMessageBox.cpp
src/SimpleSpectrumPicture.cpp
src/SoundLib.cpp
src/SpectrogramPicture.cpp
src/SpectrogramPlot.cpp
src/SpectrumOptionsDialog.cpp
src/SpectrumPicture.cpp
src/SynthesisThread.cpp
src/TdsOptionsDialog.cpp
src/TdsPage.cpp
src/TdsSpatialSignalPicture.cpp
src/TdsTimeSignalPicture.cpp
src/TdsTubePicture.cpp
src/TimeAxisPicture.cpp
src/TransitionDialog.cpp
src/VocalTractDialog.cpp
src/VocalTractPage.cpp
src/VocalTractPicture.cpp
src/VocalTractShapesDialog.cpp
)
target_include_directories(VocalTractLab PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Backend/include/VocalTractLabBackend)
target_link_libraries(VocalTractLab VocalTractLabBackend ${wxWidgets_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${OPENAL_LIBRARY} )
endif()


