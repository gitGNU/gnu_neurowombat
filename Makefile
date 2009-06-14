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


all:	bin/neurowombat


bin/neurowombat:	build/main.o build/api.o build/Kernel.o build/KernelObject.o build/InterruptManager.o build/SimulationEngine.o build/AbstractActivators.o build/AbstractAdders.o build/AbstractConnectors.o build/AbstractWeights.o build/AbstractNeuron.o build/AnalogComparators.o build/AnalogResistors.o build/AnalogWires.o build/AnalogNeuron.o build/TransferFunction.o build/Destribution.o
		${CC} -o $@ -ldl -lrt -l${LUA_LIB} build/main.o build/api.o build/Kernel.o build/KernelObject.o build/InterruptManager.o build/SimulationEngine.o build/AbstractActivators.o build/AbstractAdders.o build/AbstractConnectors.o build/AbstractWeights.o build/AbstractNeuron.o build/AnalogComparators.o build/AnalogResistors.o build/AnalogWires.o build/AnalogNeuron.o build/TransferFunction.o build/Destribution.o


build/main.o:	src/main.cpp src/kernel/Kernel.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/main.cpp


# api


build/api.o:	src/api/api.cpp src/api/api.h src/kernel/Kernel.h src/components/abstract/AbstractActivators.h src/components/abstract/AbstractAdders.h src/components/abstract/AbstractConnectors.h src/components/abstract/AbstractWeights.h src/neurons/abstract/AbstractNeuron.h src/components/analog/AnalogComparators.h src/components/analog/AnalogResistors.h src/components/analog/AnalogWires.h src/neurons/analog/AnalogNeuron.h src/engine/SimulationEngine.h src/math/Destribution.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/api/api.cpp


# coponents - abstract


build/AbstractActivators.o:	src/components/abstract/AbstractActivators.cpp src/components/abstract/AbstractActivators.h src/kernel/KernelObject.h src/math/TransferFunction.h
		${CC} -c -o $@ -Isrc src/components/abstract/AbstractActivators.cpp


build/AbstractAdders.o:	src/components/abstract/AbstractAdders.cpp src/components/abstract/AbstractAdders.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/components/abstract/AbstractAdders.cpp


build/AbstractConnectors.o:	src/components/abstract/AbstractConnectors.cpp src/components/abstract/AbstractConnectors.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/components/abstract/AbstractConnectors.cpp


build/AbstractWeights.o:	src/components/abstract/AbstractWeights.cpp src/components/abstract/AbstractWeights.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/components/abstract/AbstractWeights.cpp


# coponents - analog


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
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/kernel/Kernel.cpp


build/KernelObject.o:	src/kernel/KernelObject.cpp src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc -I${LUA_INCLUDE} src/kernel/KernelObject.cpp


# neurons - abstract


build/AbstractNeuron.o:	src/neurons/abstract/AbstractNeuron.cpp src/neurons/abstract/AbstractNeuron.h src/components/abstract/AbstractActivators.h src/components/abstract/AbstractAdders.h src/components/abstract/AbstractConnectors.h src/components/abstract/AbstractWeights.h src/kernel/KernelObject.h src/exceptions.h
		${CC} -c -o $@ -Isrc src/neurons/abstract/AbstractNeuron.cpp


# neurons - analog


build/AnalogNeuron.o:	src/neurons/analog/AnalogNeuron.cpp src/neurons/analog/AnalogNeuron.h src/components/analog/AnalogComparators.h src/components/analog/AnalogResistors.h src/components/analog/AnalogWires.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/neurons/analog/AnalogNeuron.cpp


# math


build/Destribution.o:	src/math/Destribution.cpp src/math/Destribution.h src/kernel/KernelObject.h
		${CC} -c -o $@ -Isrc src/math/Destribution.cpp


build/TransferFunction.o:	src/math/TransferFunction.cpp src/math/TransferFunction.h
		${CC} -c -o $@ -Isrc src/math/TransferFunction.cpp


clean:
		rm -f build/*.o bin/neurowombat
