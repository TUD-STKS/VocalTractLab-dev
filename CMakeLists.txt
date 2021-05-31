# CMakeList.txt : CMake project for VocalTractLab, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("VocalTractLab")
set(wxWidgets_ROOT_DIR C:/wxwidgets-3.1.3)
add_definitions(-D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D UNICODE -D wxUSE_UNICODE -D _WINDOWS -D __WXMSW__ -D _CRT_SECURE_NO_DEPRECATE -D _CRT_NONSTDC_NO_DEPRECATE -D NDEBUG)

# Request the required wxWidgets libs
find_package(wxWidgets 3.1 COMPONENTS core base gl adv REQUIRED)

# Include the wxWidgets use file to initialize various settings
include(${wxWidgets_USE_FILE})

# Add source to this project's executable.
add_executable (VocalTractLab WIN32
Backend/AnatomyParams.cpp Backend/AnatomyParams.h
Backend/AudioFile.h
Backend/Constants.h
Backend/Dsp.cpp Backend/Dsp.h
Backend/F0EstimatorYin.cpp Backend/F0EstimatorYin.h
Backend/GeometricGlottis.cpp Backend/Geometricglottis.h
Backend/Geometry.cpp Backend/Geometry.h
Backend/GesturalScore.cpp Backend/GesturalScore.h
Backend/Glottis.cpp Backend/Glottis.h
Backend/IirFilter.cpp Backend/IirFilter.h
Backend/ImpulseExcitation.cpp Backend/ImpulseExcitation.h
Backend/LfPulse.cpp Backend/LfPulse.h
Backend/Matrix2x2.cpp Backend/Matrix2x2.h
Backend/PoleZeroPlan.cpp Backend/PoleZeroPlan.h
Backend/Sampa.cpp Backend/Sampa.h
Backend/SegmentSequence.cpp Backend/SegmentSequence.h
Backend/Signal.cpp Backend/Signal.h
Backend/SoundLib.cpp Backend/SoundLib.h
Backend/Splines.cpp Backend/Splines.h
Backend/StaticPhone.cpp Backend/StaticPhone.h
Backend/Surface.cpp Backend/Surface.h
Backend/Synthesizer.cpp Backend/Synthesizer.h
Backend/TdsModel.cpp Backend/TdsModel.h
Backend/TimeFunction.cpp Backend/TimeFunction.h
Backend/TlModel.cpp Backend/TlModel.h
Backend/TriangularGlottis.cpp Backend/TriangularGlottis.h
Backend/Tube.cpp Backend/Tube.h
Backend/TubeSequence.h
Backend/TwoMassModel.cpp Backend/TwoMassModel.h
Backend/VocalTract.cpp Backend/VocalTract.h
Backend/VoiceQualityEstimator.cpp Backend/VoiceQualityEstimator.h
Backend/VowelLf.cpp Backend/VowelLf.h
Backend/XmlHelper.cpp Backend/XmlHelper.h
Backend/XmlNode.cpp Backend/XmlNode.h
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

target_link_libraries(VocalTractLab ${wxWidgets_LIBRARIES})

