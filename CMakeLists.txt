#
# Text:
#      CMakeLists.txt
#
# Description:
#      Top level project definitions which includes the C++ files and the
#      header files as required to compile the Exopanel GUI project
#
# Documentation:
#      See the CMake documentation.
#
# MIT License
#
# Copyright (c) 2017 Doug Barbieri
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required(VERSION 3.5)

project(exopanel-project)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Automatically run the moc compiler on qt files
set( CMAKE_AUTOMOC TRUE )
set( CMAKE_AUTORCC TRUE )
set( CMAKE_AUTOUIC TRUE )

## Include support modules
##
find_package( Qt5Core             REQUIRED )
find_package( Qt5Widgets          REQUIRED )
find_package( SnapCMakeModules    REQUIRED )

##
## Version
##
SnapGetVersion( EXOPANEL ${CMAKE_SOURCE_DIR} )
add_definitions( -DEXOPANEL_VERSION=\"${EXOPANEL_VERSION_MAJOR}.${EXOPANEL_VERSION_MINOR}.${EXOPANEL_VERSION_PATCH}\" )

set( QT_LIBRARIES
    Qt5::Core
    Qt5::Widgets
)

include_directories( SYSTEM
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

add_subdirectory(src)

install( FILES exopanel.desktop DESTINATION share/applications )

# vim: ts=4 sw=4 et
