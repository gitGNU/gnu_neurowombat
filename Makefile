#   Copyright (C) 2009 Andrew Timashov
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


CFLAGS=		-g
CC=		g++ ${CFLAGS}
LUA_INCLUDE=	/usr/include/lua5.1
LUA_LIB=	lua5.1

# Neurowombat configuration;
MODULES_PATH=	$(CURDIR)/modules


all:	bin/neurowombat


bin/neurowombat:	build/main.o build/api.o build/constants.o build/Kernel.o build/KernelObject.o build/InterruptManager.o build/SimulationEngine.o build/AbstractActivators.o build/AbstractBuffers.o build/AbstractConnectors.o build/AbstractProcessor.o build/AbstractWeights.o build/AbstractNeuron.o build/AnalogComparators.o build/AnalogResistors.o build/AnalogWires.o build/AnalogNeuron.o build/ActivationFunction.o build/Destribution.o build/OdeSystem.o build/OdeSystemSolver.o build/CustomFunction.o
		${CC} -o $@ -ldl -lrt -l${LUA_LIB} build/main.o build/api.o build/constants.o build/Kernel.o build/KernelObject.o build/InterruptManager.o build/SimulationEngine.o build/AbstractActivators.o build/AbstractBuffers.o build/AbstractConnectors.o build/AbstractProcessor.o build/AbstractWeights.o build/AbstractNeuron.o build/AnalogComparators.o build/AnalogResistors.o build/AnalogWires.o build/AnalogNeuron.o build/ActivationFunction.o build/Destribution.o build/OdeSystem.o build/OdeSystemSolver.o build/CustomFunction.o


build/main.o:	src/main.cpp src/kernel/Kernel.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/main.cpp


# api


build/api.o:	src/api/api.cpp src/api/api.h src/api/constants.h src/kernel/Kernel.h src/components/abstract/AbstractActivators.h src/components/abstract/AbstractBuffers.h src/components/abstract/AbstractConnectors.h src/components/abstract/AbstractWeights.h src/neurons/abstract/AbstractNeuron.h src/components/analog/AnalogComparators.h src/components/analog/AnalogResistors.h src/components/analog/AnalogWires.h src/neurons/analog/AnalogNeuron.h src/engine/SimulationEngine.h src/math/Destribution.h src/math/ActivationFunction.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/api/api.cpp


build/constants.o:	src/api/constants.cpp src/api/constants.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/api/constants.cpp


# components - abstract


build/AbstractActivators.o:	src/components/abstract/AbstractActivators.cpp src/components/abstract/AbstractActivators.h src/kernel/KernelObject.h src/math/ActivationFunction.h
		${CC} -c -o $@ -Isrc src/components/abstract/AbstractActivators.cpp


build/AbstractBuffers.o:	src/components/abstract/AbstractBuffers.cpp src/components/abstract/AbstractBuffers.h src/kernel/KernelObject.h src/engine/InterruptManager.h
		${CC} -c -o $@ -Isrc src/components/abstract/AbstractBuffers.cpp


build/AbstractConnectors.o:	src/components/abstract/AbstractConnectors.cpp src/components/abstract/AbstractConnectors.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/components/abstract/AbstractConnectors.cpp


build/AbstractMeters.o:	src/components/abstract/AbstractMeters.cpp src/components/abstract/AbstractMeters.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/components/abstract/AbstractMeters.cpp


build/AbstractProcessor.o:	src/components/abstract/AbstractProcessor.cpp src/components/abstract/AbstractProcessor.h src/kernel/KernelObject.h src/components/abstract/AbstractConnectors.h src/components/abstract/AbstractWeights.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/components/abstract/AbstractProcessor.cpp


build/AbstractWeights.o:	src/components/abstract/AbstractWeights.cpp src/components/abstract/AbstractWeights.h src/kernel/KernelObject.h src/engine/InterruptManager.h
		${CC} -c -o $@ -Isrc src/components/abstract/AbstractWeights.cpp


# components - analog


build/AnalogComparators.o:	src/components/analog/AnalogComparators.cpp src/components/analog/AnalogComparators.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/components/analog/AnalogComparators.cpp


build/AnalogResistors.o:	src/components/analog/AnalogResistors.cpp src/components/analog/AnalogResistors.h src/kernel/KernelObject.h src/engine/InterruptManager.h
		${CC} -c -o $@ -Isrc src/components/analog/AnalogResistors.cpp


build/AnalogWires.o:	src/components/analog/AnalogWires.cpp src/components/analog/AnalogWires.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/components/analog/AnalogWires.cpp


# engine


build/InterruptManager.o:	src/engine/InterruptManager.cpp src/engine/InterruptManager.h src/kernel/KernelObject.h src/math/Destribution.h
		${CC} -c -o $@ -Isrc src/engine/InterruptManager.cpp


build/SimulationEngine.o:	src/engine/SimulationEngine.cpp src/engine/SimulationEngine.h src/kernel/KernelObject.h src/engine/InterruptManager.h
		${CC} -c -o $@ -Isrc src/engine/SimulationEngine.cpp


# kernel


build/Kernel.o:	src/kernel/Kernel.cpp src/kernel/Kernel.h src/kernel/KernelObject.h src/patterns/Singleton.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} -DMODULES_PATH=\"${MODULES_PATH}\" src/kernel/Kernel.cpp


build/KernelObject.o:	src/kernel/KernelObject.cpp src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/kernel/KernelObject.cpp


# neurons - abstract


build/AbstractNeuron.o:	src/neurons/abstract/AbstractNeuron.cpp src/neurons/abstract/AbstractNeuron.h src/components/abstract/AbstractActivators.h src/components/abstract/AbstractBuffers.h src/components/abstract/AbstractConnectors.h src/components/abstract/AbstractProcessor.h src/components/abstract/AbstractWeights.h src/kernel/KernelObject.h src/exceptions.h
		${CC} -c -o $@ -Isrc src/neurons/abstract/AbstractNeuron.cpp


# neurons - analog


build/AnalogNeuron.o:	src/neurons/analog/AnalogNeuron.cpp src/neurons/analog/AnalogNeuron.h src/components/analog/AnalogComparators.h src/components/analog/AnalogResistors.h src/components/analog/AnalogWires.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/neurons/analog/AnalogNeuron.cpp


# math


build/ActivationFunction.o:	src/math/ActivationFunction.cpp src/math/ActivationFunction.h src/kernel/KernelObject.h src/objects/CustomFunction.h
		${CC} -c -o $@ -Isrc src/math/ActivationFunction.cpp


build/Destribution.o:	src/math/Destribution.cpp src/math/Destribution.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/math/Destribution.cpp


build/OdeSystem.o:	src/math/OdeSystem.cpp src/math/OdeSystem.h
		${CC} -c -o $@ -Isrc src/math/OdeSystem.cpp


build/OdeSystemSolver.o:	src/math/OdeSystemSolver.cpp src/math/OdeSystemSolver.h src/math/OdeSystem.h
		${CC} -c -o $@ -Isrc src/math/OdeSystemSolver.cpp


# objects


build/CustomFunction.o:	src/objects/CustomFunction.cpp src/objects/CustomFunction.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/objects/CustomFunction.cpp


clean:
		rm -f build/*.o bin/neurowombat
