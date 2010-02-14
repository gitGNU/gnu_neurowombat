/***************************************************************************
 *   Copyright (C) 2009, 2010 Andrew Timashov                              *
 *                                                                         *
 *   This file is part of NeuroWombat.                                     *
 *                                                                         *
 *   NeuroWombat is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   NeuroWombat is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with NeuroWombat.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef API_H
#define API_H


#include <lua.hpp>


void registerApiFunctions( lua_State * L );


unsigned int readArray( lua_State * L, int index, unsigned int length, unsigned int * array );
unsigned int readArray( lua_State * L, int index, unsigned int length, double * array );


/***************************************************************************
 *   Commonn API functions declaration                                     *
 ***************************************************************************/


extern "C" int closeId( lua_State * L );


/***************************************************************************
 *   Abstract neuron API functions declaration                             *
 ***************************************************************************/


extern "C" int createAbstractConnectors( lua_State * L );


extern "C" int getSignals( lua_State * L );


extern "C" int setSignals( lua_State * L );


extern "C" int createAbstractWeights( lua_State * L );


extern "C" int getAbstractWeights( lua_State * L );


extern "C" int setAbstractWeights( lua_State * L );


extern "C" int createAbstractNeuron( lua_State * L );


extern "C" int computeAbstractNeurons( lua_State * L );


extern "C" int computeAbstractNeuronsC( lua_State * L );


extern "C" int trainBPAbstractNeurons( lua_State * L );


/***************************************************************************
 *   Analog neuron API functions declaration                               *
 ***************************************************************************/


extern "C" int createAnalogCapacitors( lua_State * L );


extern "C" int getAnalogCapacitances( lua_State * L );


extern "C" int setAnalogCapacitances( lua_State * L );


extern "C" int createAnalogComparators( lua_State * L );


extern "C" int createAnalogResistors( lua_State * L );


extern "C" int setupAnalogResistors( lua_State * L );


extern "C" int getAnalogResistances( lua_State * L );


extern "C" int setAnalogResistances( lua_State * L );


extern "C" int createAnalogWires( lua_State * L );


extern "C" int getPotentials( lua_State * L );


extern "C" int setPotentials( lua_State * L );


extern "C" int createAnalogNeuron( lua_State * L );


extern "C" int computeAnalogNeurons( lua_State * L );


extern "C" int computeAnalogLimNeuronsC( lua_State * L );


/***************************************************************************
 *   Digital neuron API functions declaration                              *
 ***************************************************************************/


extern "C" int createDigitalConnectors( lua_State * L );


extern "C" int getValues( lua_State * L );


extern "C" int setValues( lua_State * L );


extern "C" int createMemoryModule( lua_State * L );


extern "C" int getDigitalWeights( lua_State * L );


extern "C" int setDigitalWeights( lua_State * L );


extern "C" int createDigitalNeuron( lua_State * L );


extern "C" int computeDigitalNeurons( lua_State * L );


extern "C" int trainBPDigitalNeurons( lua_State * L );


/***************************************************************************
 *   Math API functions declaration                                        *
 ***************************************************************************/


extern "C" int createActFunc( lua_State * L );


extern "C" int createProcUnit( lua_State * L );


extern "C" int createDistribution( lua_State * L );


extern "C" int calcMeanCI( lua_State * L );


extern "C" int calcACProbabilityCI( lua_State * L );


/***************************************************************************
 *   Simulation engine API functions declaration                           *
 ***************************************************************************/


extern "C" int createInterruptManager( lua_State * L );


extern "C" int createSimulationEngine( lua_State * L );


extern "C" int appendInterruptManager( lua_State * L );


extern "C" int getIntSourcesCount( lua_State * L );


extern "C" int getInterruptsCount( lua_State * L );


extern "C" int simulateInterrupt( lua_State * L );


extern "C" int restartEngine( lua_State * L );


extern "C" int stepOverEngine( lua_State * L );


extern "C" int getCurrentTime( lua_State * L );


extern "C" int getFutureTime( lua_State * L );


extern "C" int getCurrentSource( lua_State * L );


extern "C" int getFutureSource( lua_State * L );


#endif
