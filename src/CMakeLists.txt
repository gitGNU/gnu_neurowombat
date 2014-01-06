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


find_package(Lua51 REQUIRED)

if(NOT LUA51_FOUND)
   message(FATAL_ERROR "Lua 5.1 not found")
endif()

set(HEADERS
   api/api.h
   api/constants.h
   components/abstract/AbstractConnectors.h
   components/abstract/AbstractWeights.h
   components/analog/AnalogCapacitors.h
   components/analog/AnalogComparators.h
   components/analog/AnalogResistors.h
   components/analog/AnalogWires.h
   components/digital/DigitalConnectors.h
   components/digital/MemoryModule.h
   components/ComponentsSet.h
   engine/InterruptManager.h
   engine/SimulationEngine.h
   kernel/Kernel.h
   kernel/KernelObject.h
   math/ActivationFunction.h
   math/Distribution.h
   math/OdeSystem.h
   math/OdeSystemSolver.h
   math/ProcessingUnit.h
   neurons/abstract/AbstractNeuron.h
   neurons/analog/AnalogNeuron.h
   neurons/digital/DigitalNeuron.h
   objects/CustomFunction.h
   patterns/Singleton.h
   exceptions.h
)

set(SOURCES
   api/api.cpp
   api/constants.cpp
   components/abstract/AbstractConnectors.cpp
   components/abstract/AbstractWeights.cpp
   components/analog/AnalogCapacitors.cpp
   components/analog/AnalogComparators.cpp
   components/analog/AnalogResistors.cpp
   components/analog/AnalogWires.cpp
   components/digital/DigitalConnectors.cpp
   components/digital/MemoryModule.cpp
   engine/InterruptManager.cpp
   engine/SimulationEngine.cpp
   kernel/Kernel.cpp
   kernel/KernelObject.cpp
   math/ActivationFunction.cpp
   math/Distribution.cpp
   math/OdeSystem.cpp
   math/OdeSystemSolver.cpp
   math/ProcessingUnit.cpp
   neurons/abstract/AbstractNeuron.cpp
   neurons/analog/AnalogNeuron.cpp
   neurons/digital/DigitalNeuron.cpp
   objects/CustomFunction.cpp
   main.cpp
)

include_directories(${LUA_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})

if(NOT BUILD_TYPE_DEBUG)
   if(WIN32)
      install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION neurowombat/bin)
   else()
      install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
   endif()
endif()
