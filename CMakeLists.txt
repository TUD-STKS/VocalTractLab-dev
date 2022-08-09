# ****************************************************************************
# This file is part of VocalTractLab3D.
# Copyright (C) 2022, Peter Birkholz, Dresden, Germany
# www.vocaltractlab.de
# author: Peter Birkholz and Rémi Blandin
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
# ****************************************************************************

# CMakeList.txt : CMake project for VocalTractLabGui, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("VocalTractLabGui")

# Request the required wxWidgets libs
if (MSVC)
set(wxWidgets_ROOT_DIR C:/wxwidgets-3.1.5)
endif()
find_package(wxWidgets 3.1 COMPONENTS core base gl adv REQUIRED)

# Include the wxWidgets use file to initialize various settings
include(${wxWidgets_USE_FILE})

# Request the required CGAL lib
if (MSVC)
set(BOOST_ROOT C:/boost_1_71_0)
endif()
find_package(CGAL)

# Request the required Eigen lib
set(Eigen3_DIR C:/eigen-3.3.9)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

if (MSVC)
add_definitions(-D
_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D UNICODE -D wxUSE_UNICODE -D _WINDOWS -D __WXMSW__ -D _CRT_SECURE_NO_DEPRECATE -D _CRT_NONSTDC_NO_DEPRECATE -D NDEBUG -D CGAL_NO_GMP)
elseif(UNIX)
add_definitions(-D
_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D WXUSINGDLL -D UNICODE -D wxUSE_UNICODE -D _CRT_SECURE_NO_DEPRECATE -D _CRT_NONSTDC_NO_DEPRECATE -D NDEBUG -D HAVE_OPENAL)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenAL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIRS})
endif()


file(GLOB all_SRCS
  "Backend/*.h"
  "*.h"
  "Backend/*.cpp"
  "*.cpp"
)

add_executable(
  VocalTractLab WIN32 
   ${all_SRCS}
)

if (MSVC)
target_link_libraries(VocalTractLab 
  ${wxWidgets_LIBRARIES} 
  CGAL::CGAL
  Eigen3::Eigen
)
elseif(UNIX)
target_link_libraries(VocalTractLab 
  ${wxWidgets_LIBRARIES} 
  CGAL::CGAL
  Eigen3::Eigen
  ${OPENGL_LIBRARIES} 
  ${GLUT_LIBRARY} 
  ${OPENAL_LIBRARY} 
)
endif()
