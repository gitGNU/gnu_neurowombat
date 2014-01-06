#   Copyright (C) 2009, 2010 Andrew Timashov
#
#   This file is part of NeuroWombat.
#
#   NeuroWombat is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   NeuroWombat is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with NeuroWombat.  If not, see <http://www.gnu.org/licenses/>.


cmake_minimum_required(VERSION 2.6)

project(neurowombat)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_DEBUG)
if("${BUILD_TYPE_DEBUG}" STREQUAL "DEBUG")
   set (BUILD_TYPE_DEBUG TRUE)
else()
   set (BUILD_TYPE_DEBUG FALSE)
endif()

if(BUILD_TYPE_DEBUG)
   add_definitions(-DMODULES_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/modules/?.lua\")
else()
   if(WIN32)
      add_definitions(-DMODULES_PATH=\"!\\\\..\\\\modules\\\\?.lua\")
   else()
      add_definitions(-DMODULES_PATH=\"${CMAKE_INSTALL_PREFIX}/share/neurowombat/modules/?.lua\")
   endif()
endif()

add_subdirectory(src)

if(NOT BUILD_TYPE_DEBUG)
   if(WIN32)
      install(DIRECTORY modules DESTINATION neurowombat)
   else()
      install(DIRECTORY modules DESTINATION share/neurowombat)
   endif()
endif()
