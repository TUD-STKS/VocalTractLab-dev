# CMakeList.txt : CMake project for VocalTractLabGui, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("VocalTractLabGui")

add_definitions(-D
_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D WXUSINGDLL -D UNICODE -D wxUSE_UNICODE -D _WINDOWS -D __WXMSW__ -D _CRT_SECURE_NO_DEPRECATE -D _CRT_NONSTDC_NO_DEPRECATE -D NDEBUG)

# Request the required wxWidgets libs
find_package(wxWidgets 3.1 COMPONENTS core base gl adv REQUIRED)

# Include the wxWidgets use file to initialize various settings
include(${wxWidgets_USE_FILE})

# Add source to this project's executable.
add_executable (VocalTractLabGui WIN32
VocalTractLabApi/src/AnatomyParams.cpp VocalTractLabApi/src/AnatomyParams.h
VocalTractLabApi/src/AudioFile.h
VocalTractLabApi/src/Constants.h
VocalTractLabApi/src/Dsp.cpp VocalTractLabApi/src/Dsp.h
VocalTractLabApi/src/F0EstimatorYin.cpp VocalTractLabApi/src/F0EstimatorYin.h
VocalTractLabApi/src/GeometricGlottis.cpp VocalTractLabApi/src/Geometricglottis.h
VocalTractLabApi/src/Geometry.cpp VocalTractLabApi/src/Geometry.h
VocalTractLabApi/src/GesturalScore.cpp VocalTractLabApi/src/GesturalScore.h
VocalTractLabApi/src/Glottis.cpp VocalTractLabApi/src/Glottis.h
VocalTractLabApi/src/IirFilter.cpp VocalTractLabApi/src/IirFilter.h
VocalTractLabApi/src/ImpulseExcitation.cpp VocalTractLabApi/src/ImpulseExcitation.h
VocalTractLabApi/src/LfPulse.cpp VocalTractLabApi/src/LfPulse.h
VocalTractLabApi/src/Matrix2x2.cpp VocalTractLabApi/src/Matrix2x2.h
VocalTractLabApi/src/PoleZeroPlan.cpp VocalTractLabApi/src/PoleZeroPlan.h
VocalTractLabApi/src/Sampa.cpp VocalTractLabApi/src/Sampa.h
VocalTractLabApi/src/SegmentSequence.cpp VocalTractLabApi/src/SegmentSequence.h
VocalTractLabApi/src/Signal.cpp VocalTractLabApi/src/Signal.h
VocalTractLabApi/src/Splines.cpp VocalTractLabApi/src/Splines.h
VocalTractLabApi/src/StaticPhone.cpp VocalTractLabApi/src/StaticPhone.h
VocalTractLabApi/src/Surface.cpp VocalTractLabApi/src/Surface.h
VocalTractLabApi/src/Synthesizer.cpp VocalTractLabApi/src/Synthesizer.h
VocalTractLabApi/src/TdsModel.cpp VocalTractLabApi/src/TdsModel.h
VocalTractLabApi/src/TimeFunction.cpp VocalTractLabApi/src/TimeFunction.h
VocalTractLabApi/src/TlModel.cpp VocalTractLabApi/src/TlModel.h
VocalTractLabApi/src/TriangularGlottis.cpp VocalTractLabApi/src/TriangularGlottis.h
VocalTractLabApi/src/Tube.cpp VocalTractLabApi/src/Tube.h
VocalTractLabApi/src/TubeSequence.h
VocalTractLabApi/src/TwoMassModel.cpp VocalTractLabApi/src/TwoMassModel.h
VocalTractLabApi/src/VocalTract.cpp VocalTractLabApi/src/VocalTract.h
VocalTractLabApi/src/VoiceQualityEstimator.cpp VocalTractLabApi/src/VoiceQualityEstimator.h
VocalTractLabApi/src/VowelLf.cpp VocalTractLabApi/src/VowelLf.h
VocalTractLabApi/src/XmlHelper.cpp VocalTractLabApi/src/XmlHelper.h
VocalTractLabApi/src/XmlNode.cpp VocalTractLabApi/src/XmlNode.h
AnalysisResultsDialog.cpp AnalysisResultsDialog.h
AnalysisSettingsDialog.cpp AnalysisSettingsDialog.h
AnatomyParamsDialog.cpp AnatomyParamsDialog.h
AnnotationDialog.cpp AnnotationDialog.h
Application.cpp Application.h
AreaFunctionPicture.cpp AreaFunctionPicture.h
BasicPicture.cpp BasicPicture.h
ColorScale.cpp ColorScale.h
CrossSectionPicture.cpp CrossSectionPicture.h
Data.cpp Data.h
EmaConfigDialog.cpp EmaConfigDialog.h
FdsOptionsDialog.cpp FdsOptionsDialog.h
FormantOptimizationDialog.cpp FormantOptimizationDialog.h
GesturalScorePage.cpp GesturalScorePage.h
GesturalScorePicture.cpp GesturalScorePicture.h
GlottisDialog.cpp GlottisDialog.h
GlottisPanel.cpp GlottisPanel.h
GlottisPicture.cpp GlottisPicture.h
Graph.cpp Graph.h
IconsXpm.h
LfPulseDialog.cpp LfPulseDialog.h
LfPulsePicture.cpp LfPulsePicture.h
MainWindow.cpp MainWindow.h
PhoneticParamsDialog.cpp PhoneticParamsDialog.h
PoleZeroDialog.cpp PoleZeroDialog.h
PoleZeroPlot.cpp PoleZeroPlot.h
SignalComparisonPicture.cpp SignalComparisonPicture.h
SignalPage.cpp SignalPage.h
SignalPicture.cpp SignalPicture.h
SilentMessageBox.cpp SilentMessageBox.h
SimpleSpectrumPicture.cpp SimpleSpectrumPicture.h
SoundLib.cpp SoundLib.h
SpectrogramPicture.cpp SpectrogramPicture.h
SpectrogramPlot.cpp SpectrogramPlot.h
SpectrumOptionsDialog.cpp SpectrumOptionsDialog.h
SpectrumPicture.cpp SpectrumPicture.h
SynthesisThread.cpp SynthesisThread.h
TdsOptionsDialog.cpp TdsOptionsDialog.h
TdsPage.cpp TdsPage.h
TdsSpatialSignalPicture.cpp TdsSpatialSignalPicture.h
TdsTimeSignalPicture.cpp TdsTimeSignalPicture.h
TdsTubePicture.cpp TdsTubePicture.h
TimeAxisPicture.cpp TimeAxisPicture.h
TransitionDialog.cpp TransitionDialog.h
VocalTractDialog.cpp VocalTractDialog.h
VocalTractPage.cpp VocalTractPage.h
VocalTractPicture.cpp VocalTractPicture.h
VocalTractShapesDialog.cpp VocalTractShapesDialog.h
)

target_link_libraries(VocalTractLabGui ${wxWidgets_LIBRARIES})

