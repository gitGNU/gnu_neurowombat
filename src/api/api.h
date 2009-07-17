/***************************************************************************
 *   Copyright (C) 2009 Andrew Timashov                                    *
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


#define API_VERSION "0.1"


void registerApiFunctions( lua_State * L );


unsigned int readArray( lua_State * L, int index, unsigned int length, unsigned int * array );
unsigned int readArray( lua_State * L, int index, unsigned int length, double * array );


/***************************************************************************
 *   Commonn API functions declaration                                     *
 ***************************************************************************/


extern "C" int apiVersion( lua_State * L );


extern "C" int closeId( lua_State * L );


extern "C" int readArray( lua_State * L );


extern "C" int trainAnalogHopfield( lua_State * L );


/***************************************************************************
 *   Abstract neuron API functions declaration                             *
 ***************************************************************************/


extern "C" int createAbstractActivators( lua_State * L );


extern "C" int createAbstractAdders( lua_State * L );


extern "C" int createAbstractBuffers( lua_State * L );


extern "C" int createAbstractConnectors( lua_State * L );


extern "C" int getSignals( lua_State * L );


extern "C" int setSignals( lua_State * L );


extern "C" int createAbstractWeights( lua_State * L );


extern "C" int getAbstractWeights( lua_State * L );


extern "C" int setAbstractWeights( lua_State * L );


extern "C" int createAbstractNeuron( lua_State * L );


extern "C" int computeAbstractNeurons( lua_State * L );


extern "C" int trainBPAbstractNeurons( lua_State * L );


/***************************************************************************
 *   Analog neuron API functions declaration                               *
 ***************************************************************************/


extern "C" int createAnalogComparators( lua_State * L );


extern "C" int createAnalogResistors( lua_State * L );


extern "C" int setupAnalogResistors( lua_State * L );


extern "C" int createAnalogWires( lua_State * L );


extern "C" int getPotentials( lua_State * L );


extern "C" int setPotentials( lua_State * L );


extern "C" int createAnalogNeuron( lua_State * L );


extern "C" int computeAnalogNeurons( lua_State * L );


/***************************************************************************
 *   Math API functions declaration                                        *
 ***************************************************************************/


extern "C" int createLinearActFunc( lua_State * L );


extern "C" int createSigmoidActFunc( lua_State * L );


/***************************************************************************
 *   Simulation engine API functions declaration                           *
 ***************************************************************************/


extern "C" int createExponentialDestribution( lua_State * L );


extern "C" int createWeibullDestribution( lua_State * L );


extern "C" int createAbstractWeightsManager( lua_State * L );


extern "C" int createAnalogResistorsManager( lua_State * L );


extern "C" int createSimulationEngine( lua_State * L );


extern "C" int appendInterruptManager( lua_State * L );


extern "C" int stepOverEngine( lua_State * L );


extern "C" int getCurrentTime( lua_State * L );


#endif
