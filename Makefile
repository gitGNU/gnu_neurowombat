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


CFLAGS=	
CC=		g++ ${CFLAGS}
CP=		cp
CPDIR=		cp -r
MKDIR=		mkdir -p
RM=		rm -f
LUA_INCLUDE=	/usr/include/lua5.1
LUA_LIB=	lua5.1


# Neurowombat configuration;
PREFIX=		/usr

# Absolute modules path;
#MODULES_PATH=	${PREFIX}/share/neurowombat/modules/?.lua
# Relative modules path;
MODULES_PATH=	$(CURDIR)/modules/?.lua
# Relative modules path for WIN32;
#MODULES_PATH=	!\\\\..\\\\modules\\\\?.lua


all:	bin/neurowombat


bin/neurowombat:	build/main.o build/api.o build/constants.o build/Kernel.o build/KernelObject.o build/InterruptManager.o build/SimulationEngine.o build/AbstractConnectors.o build/AbstractWeights.o build/AbstractNeuron.o build/AnalogCapacitors.o build/AnalogComparators.o build/AnalogResistors.o build/AnalogWires.o build/AnalogNeuron.o build/DigitalConnectors.o build/MemoryModule.o build/DigitalNeuron.o build/ActivationFunction.o build/Distribution.o build/OdeSystem.o build/OdeSystemSolver.o build/ProcessingUnit.o build/CustomFunction.o
		${CC} -o $@ -ldl -lrt -l${LUA_LIB} build/main.o build/api.o build/constants.o build/Kernel.o build/KernelObject.o build/InterruptManager.o build/SimulationEngine.o build/AbstractConnectors.o build/AbstractWeights.o build/AbstractNeuron.o build/AnalogCapacitors.o build/AnalogComparators.o build/AnalogResistors.o build/AnalogWires.o build/AnalogNeuron.o build/DigitalConnectors.o build/MemoryModule.o build/DigitalNeuron.o build/ActivationFunction.o build/Distribution.o build/OdeSystem.o build/OdeSystemSolver.o build/ProcessingUnit.o build/CustomFunction.o


build/main.o:	src/main.cpp src/kernel/Kernel.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/main.cpp


# api


build/api.o:	src/api/api.cpp src/api/api.h src/api/constants.h src/kernel/Kernel.h src/components/abstract/AbstractConnectors.h src/components/abstract/AbstractWeights.h src/neurons/abstract/AbstractNeuron.h src/components/analog/AnalogCapacitors.h src/components/analog/AnalogComparators.h src/components/analog/AnalogResistors.h src/components/analog/AnalogWires.h src/neurons/analog/AnalogNeuron.h src/components/digital/DigitalConnectors.h src/components/digital/MemoryModule.h src/neurons/digital/DigitalNeuron.h src/engine/SimulationEngine.h src/math/Distribution.h src/math/ActivationFunction.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/api/api.cpp


build/constants.o:	src/api/constants.cpp src/api/constants.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/api/constants.cpp


# components - abstract


build/AbstractConnectors.o:	src/components/abstract/AbstractConnectors.cpp src/components/abstract/AbstractConnectors.h src/components/ComponentsSet.h
		${CC} -c -o $@ -Isrc src/components/abstract/AbstractConnectors.cpp


build/AbstractWeights.o:	src/components/abstract/AbstractWeights.cpp src/components/abstract/AbstractWeights.h src/components/ComponentsSet.h src/engine/InterruptManager.h
		${CC} -c -o $@ -Isrc src/components/abstract/AbstractWeights.cpp


# components - analog


build/AnalogCapacitors.o:	src/components/analog/AnalogCapacitors.cpp src/components/analog/AnalogCapacitors.h src/components/ComponentsSet.h src/engine/InterruptManager.h
		${CC} -c -o $@ -Isrc src/components/analog/AnalogCapacitors.cpp


build/AnalogComparators.o:	src/components/analog/AnalogComparators.cpp src/components/analog/AnalogComparators.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/components/analog/AnalogComparators.cpp


build/AnalogResistors.o:	src/components/analog/AnalogResistors.cpp src/components/analog/AnalogResistors.h src/components/ComponentsSet.h src/engine/InterruptManager.h
		${CC} -c -o $@ -Isrc src/components/analog/AnalogResistors.cpp


build/AnalogWires.o:	src/components/analog/AnalogWires.cpp src/components/analog/AnalogWires.h src/components/ComponentsSet.h
		${CC} -c -o $@ -Isrc src/components/analog/AnalogWires.cpp


# components - digital


build/DigitalConnectors.o:	src/components/digital/DigitalConnectors.cpp src/components/digital/DigitalConnectors.h src/components/ComponentsSet.h
		${CC} -c -o $@ -Isrc src/components/digital/DigitalConnectors.cpp


build/MemoryModule.o:	src/components/digital/MemoryModule.cpp src/components/digital/MemoryModule.h src/components/ComponentsSet.h src/engine/InterruptManager.h
		${CC} -c -o $@ -Isrc src/components/digital/MemoryModule.cpp


# engine


build/InterruptManager.o:	src/engine/InterruptManager.cpp src/engine/InterruptManager.h src/kernel/KernelObject.h src/math/Distribution.h
		${CC} -c -o $@ -Isrc src/engine/InterruptManager.cpp


build/SimulationEngine.o:	src/engine/SimulationEngine.cpp src/engine/SimulationEngine.h src/kernel/KernelObject.h src/engine/InterruptManager.h
		${CC} -c -o $@ -Isrc src/engine/SimulationEngine.cpp


# kernel


build/Kernel.o:	src/kernel/Kernel.cpp src/kernel/Kernel.h src/kernel/KernelObject.h src/patterns/Singleton.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} -DMODULES_PATH=\"${MODULES_PATH}\" src/kernel/Kernel.cpp


build/KernelObject.o:	src/kernel/KernelObject.cpp src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/kernel/KernelObject.cpp


# neurons - abstract


build/AbstractNeuron.o:	src/neurons/abstract/AbstractNeuron.cpp src/neurons/abstract/AbstractNeuron.h src/components/abstract/AbstractConnectors.h src/components/abstract/AbstractWeights.h src/kernel/KernelObject.h src/math/ProcessingUnit.h src/math/ActivationFunction.h src/exceptions.h
		${CC} -c -o $@ -Isrc src/neurons/abstract/AbstractNeuron.cpp


# neurons - analog


build/AnalogNeuron.o:	src/neurons/analog/AnalogNeuron.cpp src/neurons/analog/AnalogNeuron.h src/components/analog/AnalogCapacitors.h src/components/analog/AnalogComparators.h src/components/analog/AnalogResistors.h src/components/analog/AnalogWires.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/neurons/analog/AnalogNeuron.cpp


# neurons - digital


build/DigitalNeuron.o:	src/neurons/digital/DigitalNeuron.cpp src/neurons/digital/DigitalNeuron.h src/components/digital/DigitalConnectors.h src/components/digital/MemoryModule.h src/kernel/KernelObject.h src/math/ProcessingUnit.h src/math/ActivationFunction.h
		${CC} -c -o $@ -Isrc src/neurons/digital/DigitalNeuron.cpp


# math


build/ActivationFunction.o:	src/math/ActivationFunction.cpp src/math/ActivationFunction.h src/kernel/KernelObject.h src/objects/CustomFunction.h
		${CC} -c -o $@ -Isrc src/math/ActivationFunction.cpp


build/Distribution.o:	src/math/Distribution.cpp src/math/Distribution.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/math/Distribution.cpp


build/OdeSystem.o:	src/math/OdeSystem.cpp src/math/OdeSystem.h src/neurons/abstract/AbstractNeuron.h src/neurons/analog/AnalogNeuron.h
		${CC} -c -o $@ -Isrc src/math/OdeSystem.cpp


build/OdeSystemSolver.o:	src/math/OdeSystemSolver.cpp src/math/OdeSystemSolver.h src/math/OdeSystem.h
		${CC} -c -o $@ -Isrc src/math/OdeSystemSolver.cpp


build/ProcessingUnit.o:	src/math/ProcessingUnit.cpp src/math/ProcessingUnit.h src/kernel/KernelObject.h src/components/abstract/AbstractConnectors.h src/components/abstract/AbstractWeights.h src/components/digital/DigitalConnectors.h src/components/digital/MemoryModule.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/math/ProcessingUnit.cpp


# objects


build/CustomFunction.o:	src/objects/CustomFunction.cpp src/objects/CustomFunction.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/objects/CustomFunction.cpp


clean:
		${RM} build/*.o bin/neurowombat


install:	bin/neurowombat modules/abstract/GeneralizedRegressionNetwork.lua modules/abstract/HammingNetwork.lua modules/abstract/HopfieldNetwork.lua modules/abstract/KohonenLayer.lua modules/abstract/MultilayerPerceptron.lua modules/abstract/ProbabilisticNetwork.lua modules/analog/HammingNetwork.lua modules/analog/HopfieldNetwork.lua modules/digital/GeneralizedRegressionNetwork.lua modules/digital/HammingNetwork.lua modules/digital/HopfieldNetwork.lua modules/digital/KohonenLayer.lua modules/digital/MultilayerPerceptron.lua modules/digital/ProbabilisticNetwork.lua modules/reliability.lua
		${CP} bin/neurowombat ${PREFIX}/bin
		${MKDIR} ${PREFIX}/share/neurowombat
		${CPDIR} modules ${PREFIX}/share/neurowombat
