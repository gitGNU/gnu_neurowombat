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


#include "api/api.h"


#include <math.h>
#include <stdio.h>
#include <typeinfo>
#include <vector>


#include "kernel/Kernel.h"
#include "components/abstract/AbstractConnectors.h"
#include "components/abstract/AbstractWeights.h"
#include "neurons/abstract/AbstractNeuron.h"
#include "components/analog/AnalogCapacitors.h"
#include "components/analog/AnalogComparators.h"
#include "components/analog/AnalogResistors.h"
#include "components/analog/AnalogWires.h"
#include "neurons/analog/AnalogNeuron.h"
#include "components/digital/DigitalConnectors.h"
#include "components/digital/MemoryModule.h"
#include "neurons/digital/DigitalNeuron.h"
#include "engine/SimulationEngine.h"
#include "math/ActivationFunction.h"
#include "math/Distribution.h"
#include "math/OdeSystemSolver.h"
#include "math/ProcessingUnit.h"


// It is better for API functions to use this pointer instead of
// Kernel::instance() and Kernel::freeInstance() methods;
extern Kernel * kernel;


#define _readIntegersVector( L, index, integers )\
   lua_pushnil( L );\
   while ( lua_next( L, index ) != 0 )\
      {\
      integers.push_back( lua_tointeger( L, -1 ) );\
      lua_pop( L, 1 );\
      }\


#define _readKernelObjectsVector( L, index, T, objects )\
   lua_pushnil( L );\
   while ( lua_next( L, index ) != 0 )\
      {\
      T object = dynamic_cast < T >( kernel->getObject( lua_tointeger( L, -1 ) ) );\
      if ( object != NULL ) objects.push_back( object );\
      lua_pop( L, 1 );\
      }\


void registerApiFunctions( lua_State * L )
   {
   // Register common API functions;
   lua_register( L, "closeId", closeId );
   // Register abstract neuron API functions;
   lua_register( L, "createAbstractConnectors", createAbstractConnectors );
   lua_register( L, "getSignals", getSignals );
   lua_register( L, "setSignals", setSignals );
   lua_register( L, "createAbstractWeights", createAbstractWeights );
   lua_register( L, "getAbstractWeights", getAbstractWeights );
   lua_register( L, "setAbstractWeights", setAbstractWeights );
   lua_register( L, "createAbstractNeuron", createAbstractNeuron );
   lua_register( L, "computeAbstractNeurons", computeAbstractNeurons );
   lua_register( L, "computeAbstractNeuronsC", computeAbstractNeuronsC );
   lua_register( L, "trainBPAbstractNeurons", trainBPAbstractNeurons );
   // Register analog neuron API functions;
   lua_register( L, "createAnalogCapacitors", createAnalogCapacitors );
   lua_register( L, "getAnalogCapacitances", getAnalogCapacitances );
   lua_register( L, "setAnalogCapacitances", setAnalogCapacitances );
   lua_register( L, "createAnalogComparators", createAnalogComparators );
   lua_register( L, "createAnalogResistors", createAnalogResistors );
   lua_register( L, "setupAnalogResistors", setupAnalogResistors );
   lua_register( L, "getAnalogResistances", getAnalogResistances );
   lua_register( L, "setAnalogResistances", setAnalogResistances );
   lua_register( L, "createAnalogWires", createAnalogWires );
   lua_register( L, "getPotentials", getPotentials );
   lua_register( L, "setPotentials", setPotentials );
   lua_register( L, "createAnalogNeuron", createAnalogNeuron );
   lua_register( L, "computeAnalogNeurons", computeAnalogNeurons );
   lua_register( L, "computeAnalogLimNeuronsC", computeAnalogLimNeuronsC );
   // Register digital neuron API functions;
   lua_register( L, "createDigitalConnectors", createDigitalConnectors );
   lua_register( L, "getValues", getValues );
   lua_register( L, "setValues", setValues );
   lua_register( L, "createMemoryModule", createMemoryModule );
   lua_register( L, "getDigitalWeights", getDigitalWeights );
   lua_register( L, "setDigitalWeights", setDigitalWeights );
   lua_register( L, "createDigitalNeuron", createDigitalNeuron );
   lua_register( L, "computeDigitalNeurons", computeDigitalNeurons );
   lua_register( L, "trainBPDigitalNeurons", trainBPDigitalNeurons );
   // Math API functions;
   lua_register( L, "createActFunc", createActFunc );
   lua_register( L, "createProcUnit", createProcUnit );
   lua_register( L, "createDistribution", createDistribution );
   lua_register( L, "calcMeanCI", calcMeanCI );
   lua_register( L, "calcACProbabilityCI", calcACProbabilityCI );
   // Simulation engine API functions;
   lua_register( L, "createInterruptManager", createInterruptManager );
   lua_register( L, "createSimulationEngine", createSimulationEngine );
   lua_register( L, "appendInterruptManager", appendInterruptManager );
   lua_register( L, "getIntSourcesCount", getIntSourcesCount );
   lua_register( L, "getInterruptsCount", getInterruptsCount );
   lua_register( L, "simulateInterrupt", simulateInterrupt );
   lua_register( L, "restartEngine", restartEngine );
   lua_register( L, "stepOverEngine", stepOverEngine );
   lua_register( L, "getCurrentTime", getCurrentTime );
   lua_register( L, "getFutureTime", getFutureTime );
   lua_register( L, "getCurrentSource", getCurrentSource );
   lua_register( L, "getFutureSource", getFutureSource );
   };


unsigned int readArray( lua_State * L, int index, unsigned int length, unsigned int * array )
   {
   unsigned int i = 0;

   // Push first key;
   lua_pushnil( L );
   while ( lua_next( L, index ) != 0 && i < length )
      {
      // Read key and decrease it by 1 to provide compatibility
      // between C and Lua-style arrays;
      unsigned int key = lua_tointeger( L, -2 ) - 1;
      if ( key < length ) array[ key ] = lua_tointeger( L, -1 );

      // Remove 'value' but keep 'key' for next iteration;
      lua_pop( L, 1 );

      i ++;
      }

   return i;
   };


unsigned int readArray( lua_State * L, int index, unsigned int length, double * array )
   {
   unsigned int i = 0;

   // Push first key;
   lua_pushnil( L );
   while ( lua_next( L, index ) != 0 && i < length )
      {
      // Read key and decrease it by 1 to provide compatibility
      // between C and Lua-style arrays;
      unsigned int key = lua_tointeger( L, -2 ) - 1;
      if ( key < length ) array[ key ] = lua_tonumber( L, -1 );

      // Remove 'value' but keep 'key' for next iteration;
      lua_pop( L, 1 );

      i ++;
      }

   return i;
   };


/***************************************************************************
 *   Commonn API functions implementation                                  *
 ***************************************************************************/


int closeId( lua_State * L )
   {
   kernel->deleteObject( luaL_checknumber( L, 1 ) );
   return 0;
   };


/***************************************************************************
 *   Abstract neuron API functions implementation                          *
 ***************************************************************************/


int createAbstractConnectors( lua_State * L )
   {
   KernelObjectId id = 0;

   unsigned int count = luaL_checkinteger( L, 1 );
   if ( count > 0 )
      {
      AbstractConnectors * connectors = new AbstractConnectors( count );
      id = kernel->insertObject( connectors );
      }

   lua_pushnumber( L, id );
   return 1;
   };


int getSignals( lua_State * L )
   {
   // Read connectors argument;
   KernelObjectId connectorsId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( connectorsId );
   AbstractConnectors * connectors = dynamic_cast < AbstractConnectors * >( object );

   // Read baseIndex argument;
   unsigned int baseIndex = luaL_checkinteger( L, 2 );

   // Read count argument;
   unsigned int count = luaL_checkinteger( L, 3 );

   // Create table;
   lua_newtable( L );
   for ( unsigned int i = 0; i < count; i ++ )
      {
      // Increase key by 1 to provide compatibility between C and Lua-style arrays;
      lua_pushnumber( L, i + 1 );
      lua_pushnumber( L, connectors->at( baseIndex + i ) );
      lua_rawset( L, -3 );
      }

   return 1;
   };


int setSignals( lua_State * L )
   {
   // Read connectors argument;
   KernelObjectId connectorsId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( connectorsId );
   AbstractConnectors * connectors = dynamic_cast < AbstractConnectors * >( object );

   // Read baseIndex argument;
   unsigned int baseIndex = luaL_checkinteger( L, 2 );

   // Read signals argument;
   unsigned int limit = connectors->count();
   lua_pushnil( L );
   while ( lua_next( L, 3 ) != 0 )
      {
      // Read key and decrease it by 1 to provide compatibility
      // between C and Lua-style arrays;
      unsigned int index = baseIndex + lua_tointeger( L, -2 ) - 1;
      if ( index < limit ) connectors->at( index ) = lua_tonumber( L, -1 );
      lua_pop( L, 1 );
      }

   return 0;
   };


int createAbstractWeights( lua_State * L )
   {
   KernelObjectId id = 0;

   unsigned int count = luaL_checkinteger( L, 1 );
   if ( count > 0 )
      {
      AbstractWeights * weights = new AbstractWeights( count );
      id = kernel->insertObject( weights );
      }

   lua_pushnumber( L, id );
   return 1;
   };


int getAbstractWeights( lua_State * L )
   {
   // Read neuron argument;
   KernelObjectId neuronId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( neuronId );
   AbstractNeuron * neuron = dynamic_cast < AbstractNeuron * >( object );

   if ( neuron != NULL )
      {
      // Create table;
      lua_newtable( L );
      for ( unsigned int i = 0; i < neuron->getInputsCount(); i ++ )
         {
         // Increase key by 1 to provide compatibility between C and Lua-style arrays;
         lua_pushnumber( L, i + 1 );
         lua_pushnumber( L, neuron->getWeight( i ) );
         lua_rawset( L, -3 );
         }
      }
   else
      {
      AbstractWeights * weights = dynamic_cast < AbstractWeights * >( object );

      // Read baseIndex argument;
      unsigned int baseIndex = luaL_checkinteger( L, 2 );

      // Read count argument;
      unsigned int count = luaL_checkinteger( L, 3 );

      // Create table;
      lua_newtable( L );
      for ( unsigned int i = 0; i < count; i ++ )
         {
         // Increase key by 1 to provide compatibility between C and Lua-style arrays;
         lua_pushnumber( L, i + 1 );
         lua_pushnumber( L, weights->at( baseIndex + i ) );
         lua_rawset( L, -3 );
         }
      }

   return 1;
   };


int setAbstractWeights( lua_State * L )
   {
   // Read neuron argument;
   KernelObjectId neuronId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( neuronId );
   AbstractNeuron * neuron = dynamic_cast < AbstractNeuron * >( object );

   if ( neuron != NULL )
      {
      // Read weights argument;
      unsigned int limit = neuron->getInputsCount();
      lua_pushnil( L );
      while ( lua_next( L, 2 ) != 0 )
         {
         // Read key and decrease it by 1 to provide compatibility
         // between C and Lua-style arrays;
         unsigned int index = lua_tointeger( L, -2 ) - 1;
         if ( index < limit ) neuron->setWeight( index, lua_tonumber( L, -1 ) );
         lua_pop( L, 1 );
         }
      }
   else
      {
      AbstractWeights * weights = dynamic_cast < AbstractWeights * >( object );

      // Read baseIndex argument;
      unsigned int baseIndex = luaL_checkinteger( L, 2 );

      // Read weights argument;
      unsigned int limit = weights->count();
      lua_pushnil( L );
      while ( lua_next( L, 3 ) != 0 )
         {
         // Read key and decrease it by 1 to provide compatibility
         // between C and Lua-style arrays;
         unsigned int index = baseIndex + lua_tointeger( L, -2 ) - 1;
         if ( index < limit ) weights->at( index ) = lua_tonumber( L, -1 );
         lua_pop( L, 1 );
         }
      }

   return 0;
   };


int createAbstractNeuron( lua_State * L )
   {
   KernelObject * object = NULL;

   // Read inputsCount argument;
   unsigned int inputsCount = luaL_checkinteger( L, 1 );

   // Read inputConnectors argument;
   unsigned int * inputConnectors = NULL;
   if ( inputsCount > 0 )
      {
      inputConnectors = new unsigned int[ inputsCount ];
      readArray( L, 2, inputsCount, inputConnectors );
      }

   // Read connectors argument;
   KernelObjectId connectorsId = luaL_checkinteger( L, 3 );
   object = kernel->getObject( connectorsId );
   AbstractConnectors * connectors = dynamic_cast < AbstractConnectors * >( object );

   // Read connectorsBaseIndex argument;
   unsigned int connectorsBaseIndex = luaL_checkinteger( L, 4 );

   // Read weights argument;
   KernelObjectId weightsId = luaL_checkinteger( L, 5 );
   object = kernel->getObject( weightsId );
   AbstractWeights * weights = dynamic_cast < AbstractWeights * >( object );

   // Read weightsBaseIndex argument;
   unsigned int weightsBaseIndex = luaL_checkinteger( L, 6 );

   // Read processingUnit argument;
   KernelObjectId processingUnitId = luaL_checkinteger( L, 7 );
   object = kernel->getObject( processingUnitId );
   ProcessingUnit * processingUnit = dynamic_cast < ProcessingUnit * >( object );

   // Read activationFunction argument;
   KernelObjectId activationFunctionId = luaL_checkinteger( L, 8 );
   object = kernel->getObject( activationFunctionId );
   ActivationFunction * activationFunction = dynamic_cast < ActivationFunction * >( object );

   // Create abstract neuron;
   AbstractNeuron * neuron = new AbstractNeuron(
      inputsCount, inputConnectors,
      connectors, connectorsBaseIndex,
      weights, weightsBaseIndex,
      processingUnit,
      activationFunction
      );

   KernelObjectId id = kernel->insertObject( neuron );

   // Free inputConnectors;
   if ( inputConnectors != NULL ) delete[] inputConnectors;

   lua_pushnumber( L, id );
   return 1;
   };


int computeAbstractNeurons( lua_State * L )
   {
   // Create vector for holding AbstractNeuron pointers;
   std::vector < AbstractNeuron * > neurons;

   // Read neurons argument;
   _readKernelObjectsVector( L, 1, AbstractNeuron *, neurons );

   // Read times argument;
   unsigned int times = luaL_checkinteger( L, 2 );

   // Calculate neurons;
   for ( unsigned int i = 0; i < times; i ++ )
      {
      for ( unsigned int j = 0; j < neurons.size(); j ++ )
         {
         if ( neurons[ j ] != NULL ) neurons[ j ]->compute();
         }
      }

   return 0;
   };


int computeAbstractNeuronsC( lua_State * L )
   {
   // Create vector for holding AbstractNeuron pointers;
   std::vector < AbstractNeuron * > neurons;

   // Read neurons argument;
   _readKernelObjectsVector( L, 1, AbstractNeuron *, neurons );

   // Read timeConstant argument;
   double timeConstant = luaL_checkinteger( L, 2 );

   // Read time argument;
   double time = luaL_checkinteger( L, 3 );

   // Read stepsCount argument;
   unsigned int stepsCount = luaL_checkinteger( L, 4 );

   // Solve AbstractNeuronsOdeSystem;
   AbstractNeuronsOdeSystem odeSystem( & neurons, timeConstant );
   OdeSystemSolver::solve( odeSystem, 0, time, stepsCount );

   return 0;
   };


int trainBPAbstractNeurons( lua_State * L )
   {
   // Create vector for holding AbstractNeuron pointers;
   std::vector < AbstractNeuron * > neurons;

   // Read neurons argument;
   _readKernelObjectsVector( L, 1, AbstractNeuron *, neurons );

   // Create vector for holding neurons per layer count;
   std::vector < unsigned int > layers;

   // Read layers argument;
   _readIntegersVector( L, 2, layers );

   // Create array for holding target;
   unsigned int targetLength = layers[ layers.size() - 1 ];
   double * target = new double[ targetLength ];

   // Read target argument;
   readArray( L, 3, targetLength, target );

   // Read damping argument;
   double damping = luaL_checknumber( L, 4 );

   // Read speed argument;
   double speed = luaL_checknumber( L, 5 );

   // Calculate neurons;
   unsigned int neuronsCount = neurons.size();
   for ( unsigned int i = 0; i < neuronsCount; i ++ )
      {
      neurons[ i ]->compute();
      }

   // Snap deltas for output layer;
   for ( unsigned int i = 0; i < targetLength; i ++ )
      {
      neurons[ neuronsCount - 1 - i ]->snapDelta(
         target[ i ] - neurons[ neuronsCount - 1 - i ]->getOutput()
         );
      }

   // Free target;
   delete[] target;

   // Snap deltas for the rest layers;
   unsigned int index = neuronsCount - targetLength - 1;
   unsigned int prevIndex = neuronsCount - 1;
   for ( int i = layers.size() - 2; i >= 0 ; i -- )
      {
      for ( int j = layers[ i ] - 1; j >= 0; j -- )
         {
         // Calculate error;
         double err = 0.0;
         for ( int k = layers[ i + 1 ] - 1; k >= 0; k -- )
            {
            err += neurons[ prevIndex - k ]->getWeightedDelta( j );
            }

         neurons[ index ]->snapDelta( err );
         index --;
         }

      prevIndex -= layers[ i + 1 ];
      }

   // Modify weights for all the neurons;
   for ( unsigned int i = 0; i < neuronsCount; i ++ )
      {
      neurons[ i ]->createDampingBuffers();
      neurons[ i ]->modifyWeights( damping, speed );
      }

   lua_pushnumber( L, 0 );
   return 1;
   };


/***************************************************************************
 *   Analog neuron API functions implementation                            *
 ***************************************************************************/


int createAnalogCapacitors( lua_State * L )
   {
   KernelObjectId id = 0;

   unsigned int count = luaL_checkinteger( L, 1 );
   if ( count > 0 )
      {
      AnalogCapacitors * capacitors = new AnalogCapacitors( count );
      id = kernel->insertObject( capacitors );
      }

   lua_pushnumber( L, id );
   return 1;
   };


int getAnalogCapacitances( lua_State * L )
   {
   // Read capacitors argument;
   KernelObjectId resistorsId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( resistorsId );
   AnalogCapacitors * capacitors = dynamic_cast < AnalogCapacitors * >( object );

   // Read baseIndex argument;
   unsigned int baseIndex = luaL_checkinteger( L, 2 );

   // Read count argument;
   unsigned int count = luaL_checkinteger( L, 3 );

   // Create table;
   lua_newtable( L );
   for ( unsigned int i = 0; i < count; i ++ )
      {
      // Increase key by 1 to provide compatibility between C and Lua-style arrays;
      lua_pushnumber( L, i + 1 );
      lua_pushnumber( L, capacitors->at( baseIndex + i ) );
      lua_rawset( L, -3 );
      }

   return 1;
   };


int setAnalogCapacitances( lua_State * L )
   {
   // Read capacitors argument;
   KernelObjectId resistorsId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( resistorsId );
   AnalogCapacitors * capacitors = dynamic_cast < AnalogCapacitors * >( object );

   // Read baseIndex argument;
   unsigned int baseIndex = luaL_checkinteger( L, 2 );

   // Read capacitances argument;
   unsigned int limit = capacitors->count();
   lua_pushnil( L );
   while ( lua_next( L, 3 ) != 0 )
      {
      // Read key and decrease it by 1 to provide compatibility
      // between C and Lua-style arrays;
      unsigned int index = baseIndex + lua_tointeger( L, -2 ) - 1;
      if ( index < limit ) capacitors->at( index ) = lua_tonumber( L, -1 );
      lua_pop( L, 1 );
      }

   return 0;
   };


int createAnalogComparators( lua_State * L )
   {
   KernelObjectId id = 0;

   unsigned int count = luaL_checkinteger( L, 1 );
   if ( count > 0 )
      {
      AnalogComparators * comparators = new AnalogComparators( count );
      id = kernel->insertObject( comparators );
      }

   lua_pushnumber( L, id );
   return 1;
   };


int createAnalogResistors( lua_State * L )
   {
   KernelObjectId id = 0;

   unsigned int count = luaL_checkinteger( L, 1 );
   if ( count > 0 )
      {
      AnalogResistors * resistors = new AnalogResistors( count );
      id = kernel->insertObject( resistors );
      }

   lua_pushnumber( L, id );
   return 1;
   };


// Count argument must be deleted;
int setupAnalogResistors( lua_State * L )
   {
   // Read resistors argument;
   KernelObjectId resistorsId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( resistorsId );
   AnalogResistors * resistors = dynamic_cast < AnalogResistors * >( object );

   // Read baseIndex argument;
   unsigned int baseIndex = luaL_checkinteger( L, 2 );

   // Read count argument;
   unsigned int count = luaL_checkinteger( L, 3 );

   // Read weights argument;
   double * weights = NULL;
   if ( count > 0 )
      {
      weights = new double[ count ];
      readArray( L, 4, count, weights );
      }

   // Read numCopies argument;
   unsigned int numCopies = luaL_checkinteger( L, 5 );

   // Calculate weights product;
   double wProduct = 1.0;
   unsigned int numNonZeroW = 0;
   for ( unsigned int j = 0; j < count; j ++ )
      {
      if ( weights[ j ] != 0.0 )
         {
         numNonZeroW ++;
         wProduct *= weights[ j ];
         }
      }

   // Calculate resistances;
   for ( unsigned int j = 0; j < count; j ++ )
      {
      double resistance = 0.0;
      if ( weights[ j ] != 0.0 )
         {
         if ( numNonZeroW > 1 )
            {
            resistance = pow( fabs( wProduct ), 1.0 / ( double ) ( numNonZeroW - 1 ) ) / weights[ j ];
            }
         else
            {
            resistance = weights[ j ];
            }
         }

      // Set resistances;
      for ( unsigned int k = 0; k < numCopies; k ++ )
         {
         resistors->at( baseIndex + count * k + j ) = resistance;
         }
      }

   if ( weights != NULL ) delete[] weights;

   return 0;
   };


int getAnalogResistances( lua_State * L )
   {
   // Read resistors argument;
   KernelObjectId resistorsId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( resistorsId );
   AnalogResistors * resistors = dynamic_cast < AnalogResistors * >( object );

   // Read baseIndex argument;
   unsigned int baseIndex = luaL_checkinteger( L, 2 );

   // Read count argument;
   unsigned int count = luaL_checkinteger( L, 3 );

   // Create table;
   lua_newtable( L );
   for ( unsigned int i = 0; i < count; i ++ )
      {
      // Increase key by 1 to provide compatibility between C and Lua-style arrays;
      lua_pushnumber( L, i + 1 );
      lua_pushnumber( L, resistors->at( baseIndex + i ) );
      lua_rawset( L, -3 );
      }

   return 1;
   };


int setAnalogResistances( lua_State * L )
   {
   // Read resistors argument;
   KernelObjectId resistorsId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( resistorsId );
   AnalogResistors * resistors = dynamic_cast < AnalogResistors * >( object );

   // Read baseIndex argument;
   unsigned int baseIndex = luaL_checkinteger( L, 2 );

   // Read resistances argument;
   unsigned int limit = resistors->count();
   lua_pushnil( L );
   while ( lua_next( L, 3 ) != 0 )
      {
      // Read key and decrease it by 1 to provide compatibility
      // between C and Lua-style arrays;
      unsigned int index = baseIndex + lua_tointeger( L, -2 ) - 1;
      if ( index < limit ) resistors->at( index ) = lua_tonumber( L, -1 );
      lua_pop( L, 1 );
      }

   return 0;
   };


int createAnalogWires( lua_State * L )
   {
   KernelObjectId id = 0;

   unsigned int count = luaL_checkinteger( L, 1 );
   if ( count > 0 )
      {
      AnalogWires * wires = new AnalogWires( count );
      id = kernel->insertObject( wires );
      }

   lua_pushnumber( L, id );
   return 1;
   };


int getPotentials( lua_State * L )
   {
   // Read wires argument;
   KernelObjectId wiresId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( wiresId );
   AnalogWires * wires = dynamic_cast < AnalogWires * >( object );

   // Read baseIndex argument;
   unsigned int baseIndex = luaL_checkinteger( L, 2 );

   // Read count argument;
   unsigned int count = luaL_checkinteger( L, 3 );

   // Create table;
   lua_newtable( L );
   for ( unsigned int i = 0; i < count; i ++ )
      {
      // Increase key by 1 to provide compatibility between C and Lua-style arrays;
      lua_pushnumber( L, i + 1 );
      lua_pushnumber( L, wires->at( baseIndex + i ) );
      lua_rawset( L, -3 );
      }

   return 1;
   };


int setPotentials( lua_State * L )
   {
   // Read wires argument;
   KernelObjectId wiresId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( wiresId );
   AnalogWires * wires = dynamic_cast < AnalogWires * >( object );

   // Read baseIndex argument;
   unsigned int baseIndex = luaL_checkinteger( L, 2 );

   // Read potentials argument;
   unsigned int limit = wires->count();
   lua_pushnil( L );
   while ( lua_next( L, 3 ) != 0 )
      {
      // Read key and decrease it by 1 to provide compatibility
      // between C and Lua-style arrays;
      unsigned int index = baseIndex + lua_tointeger( L, -2 ) - 1;
      if ( index < limit ) wires->at( index ) = lua_tonumber( L, -1 );
      lua_pop( L, 1 );
      }

   return 0;
   };


int createAnalogNeuron( lua_State * L )
   {
   KernelObjectId id = 0;
   KernelObject * object = NULL;

   // Read inputsCount argument;
   unsigned int inputsCount = luaL_checkinteger( L, 1 );

   // Read inputWires argument;
   unsigned int * inputWires = NULL;
   if ( inputsCount > 0 )
      {
      inputWires = new unsigned int[ inputsCount ];
      readArray( L, 2, inputsCount, inputWires );
      }

   // Read gndWireIndex argument;
   unsigned int gndWireIndex = luaL_checkinteger( L, 3 );

   // Read srcWireIndex argument;
   unsigned int srcWireIndex = luaL_checkinteger( L, 4 );

   // Read capacitors argument;
   KernelObjectId capacitorsId = luaL_checkinteger( L, 5 );
   object = kernel->getObject( capacitorsId );
   AnalogCapacitors * capacitors = dynamic_cast < AnalogCapacitors * >( object );

   // Read capacitorsBaseIndex argument;
   unsigned int capacitorsBaseIndex = luaL_checkinteger( L, 6 );

   // Read comparators argument;
   KernelObjectId comparatorsId = luaL_checkinteger( L, 7 );
   object = kernel->getObject( comparatorsId );
   AnalogComparators * comparators = dynamic_cast < AnalogComparators * >( object );

   // Read comparatorsBaseIndex argument;
   unsigned int comparatorsBaseIndex = luaL_checkinteger( L, 8 );

   // Read resistors argument;
   KernelObjectId resistorsId = luaL_checkinteger( L, 9 );
   object = kernel->getObject( resistorsId );
   AnalogResistors * resistors = dynamic_cast < AnalogResistors * >( object );

   // Read resistorsBaseIndex argument;
   unsigned int resistorsBaseIndex = luaL_checkinteger( L, 10 );

   // Read wires argument;
   KernelObjectId wiresId = luaL_checkinteger( L, 11 );
   object = kernel->getObject( wiresId );
   AnalogWires * wires = dynamic_cast < AnalogWires * >( object );

   // Read wiresBaseIndex argument;
   unsigned int wiresBaseIndex = luaL_checkinteger( L, 12 );

   // Create AnalogNeuron;
   AnalogNeuron * neuron = new AnalogNeuron(
      inputsCount,
      inputWires,
      gndWireIndex,
      srcWireIndex,
      capacitors,
      capacitorsBaseIndex,
      comparators,
      comparatorsBaseIndex,
      resistors,
      resistorsBaseIndex,
      wires,
      wiresBaseIndex
      );

   id = kernel->insertObject( neuron );

   // Free inputWires;
   if ( inputWires != NULL ) delete[] inputWires;

   lua_pushnumber( L, id );
   return 1;
   };


int computeAnalogNeurons( lua_State * L )
   {
   // Create vector for holding AnalogNeuron pointers;
   std::vector < AnalogNeuron * > neurons;

   // Read neurons argument;
   _readKernelObjectsVector( L, 1, AnalogNeuron *, neurons );

   // Read times argument;
   unsigned int times = luaL_checkinteger( L, 2 );

   // Calculate neurons;
   for ( unsigned int i = 0; i < times; i ++ )
      {
      for ( unsigned int j = 0; j < neurons.size(); j ++ )
         {
         if ( neurons[ j ] != NULL ) neurons[ j ]->compute();
         }
      }

   return 0;
   };


int computeAnalogLimNeuronsC( lua_State * L )
   {
   // Create vector for holding AnalogNeuron pointers;
   std::vector < AnalogNeuron * > neurons;

   // Read neurons argument;
   _readKernelObjectsVector( L, 1, AnalogNeuron *, neurons );

   // Read time argument;
   double time = luaL_checkinteger( L, 2 );

   // Read stepsCount argument;
   unsigned int stepsCount = luaL_checkinteger( L, 3 );

   // Solve AnalogLimNeuronsOdeSystem;
   AnalogLimNeuronsOdeSystem odeSystem( & neurons );
   OdeSystemSolver::solve( odeSystem, 0, time, stepsCount );

   return 0;
   };


/***************************************************************************
 *   Digital neuron API functions implementation                           *
 ***************************************************************************/


int createDigitalConnectors( lua_State * L )
   {
   KernelObjectId id = 0;

   unsigned int count = luaL_checkinteger( L, 1 );
   if ( count > 0 )
      {
      DigitalConnectors * connectors = new DigitalConnectors( count );
      id = kernel->insertObject( connectors );
      }

   lua_pushnumber( L, id );
   return 1;
   };


int getValues( lua_State * L )
   {
   // Read connectors argument;
   KernelObjectId connectorsId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( connectorsId );
   DigitalConnectors * connectors = dynamic_cast < DigitalConnectors * >( object );

   // Read baseIndex argument;
   unsigned int baseIndex = luaL_checkinteger( L, 2 );

   // Read count argument;
   unsigned int count = luaL_checkinteger( L, 3 );

   // Create table;
   lua_newtable( L );
   for ( unsigned int i = 0; i < count; i ++ )
      {
      // Increase key by 1 to provide compatibility between C and Lua-style arrays;
      lua_pushnumber( L, i + 1 );
      lua_pushnumber( L, connectors->at( baseIndex + i ) );
      lua_rawset( L, -3 );
      }

   return 1;
   };


int setValues( lua_State * L )
   {
   // Read connectors argument;
   KernelObjectId connectorsId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( connectorsId );
   DigitalConnectors * connectors = dynamic_cast < DigitalConnectors * >( object );

   // Read baseIndex argument;
   unsigned int baseIndex = luaL_checkinteger( L, 2 );

   // Read signals argument;
   unsigned int limit = connectors->count();
   lua_pushnil( L );
   while ( lua_next( L, 3 ) != 0 )
      {
      // Read key and decrease it by 1 to provide compatibility
      // between C and Lua-style arrays;
      unsigned int index = baseIndex + lua_tointeger( L, -2 ) - 1;
      if ( index < limit ) connectors->at( index ) = lua_tonumber( L, -1 );
      lua_pop( L, 1 );
      }

   return 0;
   };


int createMemoryModule( lua_State * L )
   {
   KernelObjectId id = 0;

   unsigned int count = luaL_checkinteger( L, 1 );
   if ( count > 0 )
      {
      MemoryModule * memory = new MemoryModule( count );
      id = kernel->insertObject( memory );
      }

   lua_pushnumber( L, id );
   return 1;
   };


int getDigitalWeights( lua_State * L )
   {
   // Read neuron argument;
   KernelObjectId neuronId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( neuronId );
   DigitalNeuron * neuron = dynamic_cast < DigitalNeuron * >( object );

   if ( neuron != NULL )
      {
      // Create table;
      lua_newtable( L );
      for ( unsigned int i = 0; i < neuron->getInputsCount(); i ++ )
         {
         // Increase key by 1 to provide compatibility between C and Lua-style arrays;
         lua_pushnumber( L, i + 1 );
         lua_pushnumber( L, neuron->getWeight( i ) );
         lua_rawset( L, -3 );
         }
      }
   else
      {
      MemoryModule * memory = dynamic_cast < MemoryModule * >( object );

      // Read baseIndex argument;
      unsigned int baseIndex = luaL_checkinteger( L, 2 );

      // Read count argument;
      unsigned int count = luaL_checkinteger( L, 3 );

      // Create table;
      lua_newtable( L );
      for ( unsigned int i = 0; i < count; i ++ )
         {
         // Increase key by 1 to provide compatibility between C and Lua-style arrays;
         lua_pushnumber( L, i + 1 );
         lua_pushnumber( L, memory->at( baseIndex + i ) );
         lua_rawset( L, -3 );
         }
      }

   return 1;
   };


int setDigitalWeights( lua_State * L )
   {
   // Read neuron argument;
   KernelObjectId neuronId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( neuronId );
   DigitalNeuron * neuron = dynamic_cast < DigitalNeuron * >( object );

   if ( neuron != NULL )
      {
      // Read weights argument;
      unsigned int limit = neuron->getInputsCount();
      lua_pushnil( L );
      while ( lua_next( L, 2 ) != 0 )
         {
         // Read key and decrease it by 1 to provide compatibility
         // between C and Lua-style arrays;
         unsigned int index = lua_tointeger( L, -2 ) - 1;
         if ( index < limit ) neuron->setWeight( index, lua_tonumber( L, -1 ) );
         lua_pop( L, 1 );
         }
      }
   else
      {
      MemoryModule * memory = dynamic_cast < MemoryModule * >( object );

      // Read baseIndex argument;
      unsigned int baseIndex = luaL_checkinteger( L, 2 );

      // Read weights argument;
      unsigned int limit = memory->count();
      lua_pushnil( L );
      while ( lua_next( L, 3 ) != 0 )
         {
         // Read key and decrease it by 1 to provide compatibility
         // between C and Lua-style arrays;
         unsigned int index = baseIndex + lua_tointeger( L, -2 ) - 1;
         if ( index < limit ) memory->at( index ) = lua_tonumber( L, -1 );
         lua_pop( L, 1 );
         }
      }

   return 0;
   };


int createDigitalNeuron( lua_State * L )
   {
   KernelObject * object = NULL;

   // Read inputsCount argument;
   unsigned int inputsCount = luaL_checkinteger( L, 1 );

   // Read inputConnectors argument;
   unsigned int * inputConnectors = NULL;
   if ( inputsCount > 0 )
      {
      inputConnectors = new unsigned int[ inputsCount ];
      readArray( L, 2, inputsCount, inputConnectors );
      }

   // Read connectors argument;
   KernelObjectId connectorsId = luaL_checkinteger( L, 3 );
   object = kernel->getObject( connectorsId );
   DigitalConnectors * connectors = dynamic_cast < DigitalConnectors * >( object );

   // Read connectorsBaseIndex argument;
   unsigned int connectorsBaseIndex = luaL_checkinteger( L, 4 );

   // Read memory argument;
   KernelObjectId weightsId = luaL_checkinteger( L, 5 );
   object = kernel->getObject( weightsId );
   MemoryModule * memory = dynamic_cast < MemoryModule * >( object );

   // Read memoryBaseIndex argument;
   unsigned int memoryBaseIndex = luaL_checkinteger( L, 6 );

   // Read processingUnit argument;
   KernelObjectId processingUnitId = luaL_checkinteger( L, 7 );
   object = kernel->getObject( processingUnitId );
   ProcessingUnit * processingUnit = dynamic_cast < ProcessingUnit * >( object );

   // Read activationFunction argument;
   KernelObjectId activationFunctionId = luaL_checkinteger( L, 8 );
   object = kernel->getObject( activationFunctionId );
   ActivationFunction * activationFunction = dynamic_cast < ActivationFunction * >( object );

   // Create digital neuron;
   DigitalNeuron * neuron = new DigitalNeuron(
      inputsCount, inputConnectors,
      connectors, connectorsBaseIndex,
      memory, memoryBaseIndex,
      processingUnit,
      activationFunction
      );

   KernelObjectId id = kernel->insertObject( neuron );

   // Free inputConnectors;
   if ( inputConnectors != NULL ) delete[] inputConnectors;

   lua_pushnumber( L, id );
   return 1;
   };


int computeDigitalNeurons( lua_State * L )
   {
   // Create vector for holding DigitalNeuron pointers;
   std::vector < DigitalNeuron * > neurons;

   // Read neurons argument;
   _readKernelObjectsVector( L, 1, DigitalNeuron *, neurons );

   // Read times argument;
   unsigned int times = luaL_checkinteger( L, 2 );

   // Calculate neurons;
   for ( unsigned int i = 0; i < times; i ++ )
      {
      for ( unsigned int j = 0; j < neurons.size(); j ++ )
         {
         if ( neurons[ j ] != NULL ) neurons[ j ]->compute();
         }
      }

   return 0;
   };


int trainBPDigitalNeurons( lua_State * L )
   {
   // Create vector for holding DigitalNeuron pointers;
   std::vector < DigitalNeuron * > neurons;

   // Read neurons argument;
   _readKernelObjectsVector( L, 1, DigitalNeuron *, neurons );

   // Create vector for holding neurons per layer count;
   std::vector < unsigned int > layers;

   // Read layers argument;
   _readIntegersVector( L, 2, layers );

   // Create array for holding target;
   unsigned int targetLength = layers[ layers.size() - 1 ];
   double * target = new double[ targetLength ];

   // Read target argument;
   readArray( L, 3, targetLength, target );

   // Read damping argument;
   double damping = luaL_checknumber( L, 4 );

   // Read speed argument;
   double speed = luaL_checknumber( L, 5 );

   // Calculate neurons;
   unsigned int neuronsCount = neurons.size();
   for ( unsigned int i = 0; i < neuronsCount; i ++ )
      {
      neurons[ i ]->compute();
      }

   // Snap deltas for output layer;
   for ( unsigned int i = 0; i < targetLength; i ++ )
      {
      neurons[ neuronsCount - 1 - i ]->snapDelta(
         target[ i ] - neurons[ neuronsCount - 1 - i ]->getOutput()
         );
      }

   // Free target;
   delete[] target;

   // Snap deltas for the rest layers;
   unsigned int index = neuronsCount - targetLength - 1;
   unsigned int prevIndex = neuronsCount - 1;
   for ( int i = layers.size() - 2; i >= 0 ; i -- )
      {
      for ( int j = layers[ i ] - 1; j >= 0; j -- )
         {
         // Calculate error;
         double err = 0.0;
         for ( int k = layers[ i + 1 ] - 1; k >= 0; k -- )
            {
            err += neurons[ prevIndex - k ]->getWeightedDelta( j );
            }

         neurons[ index ]->snapDelta( err );
         index --;
         }

      prevIndex -= layers[ i + 1 ];
      }

   // Modify weights for all the neurons;
   for ( unsigned int i = 0; i < neuronsCount; i ++ )
      {
      neurons[ i ]->createDampingBuffers();
      neurons[ i ]->modifyWeights( damping, speed );
      }

   lua_pushnumber( L, 0 );
   return 1;
   };


/***************************************************************************
 *   Math API functions implementation                                     *
 ***************************************************************************/


int createActFunc( lua_State * L )
   {
   KernelObjectId id = 0;

   // Read actFunc argument;
   int actFunc = luaL_checknumber( L, 1 );
   switch ( actFunc )
      {
      case ACT_FUNC::CUSTOM:
         {
         // Read luaFunction argument;
         CustomFunction * luaFunction = new CustomFunction( 2 );

         // Read luaDerivative argument;
         CustomFunction * luaDerivative = new CustomFunction( 3 );

         id = kernel->insertObject( new CustomActivationFunction( luaFunction, luaDerivative ) );
         break;
         }
      case ACT_FUNC::GAUSSIAN:
         {
         double beta = luaL_checknumber( L, 2 );
         id = kernel->insertObject( new GaussianActivationFunction( beta ) );
         break;
         }
      case ACT_FUNC::LIM:
         {
         double xLim = luaL_checknumber( L, 2 );
         double yLow = luaL_checknumber( L, 3 );
         double yHigh = luaL_checknumber( L, 4 );
         id = kernel->insertObject( new LimActivationFunction( xLim, yLow, yHigh ) );
         break;
         }
      case ACT_FUNC::LINEAR:
         {
         double a = luaL_checknumber( L, 2 );
         double b = luaL_checknumber( L, 3 );
         id = kernel->insertObject( new LinearActivationFunction( a, b ) );
         break;
         }
      case ACT_FUNC::LIMLINEAR:
         {
         double a = luaL_checknumber( L, 2 );
         double b = luaL_checknumber( L, 3 );
         double xMin = luaL_checknumber( L, 4 );
         double xMax = luaL_checknumber( L, 5 );
         id = kernel->insertObject( new LimLinearActivationFunction( a, b, xMin, xMax ) );
         break;
         }
      case ACT_FUNC::POSLINEAR:
         {
         double a = luaL_checknumber( L, 2 );
         double b = luaL_checknumber( L, 3 );
         id = kernel->insertObject( new PosLinearActivationFunction( a, b ) );
         break;
         }
      case ACT_FUNC::SIGMOID:
         {
         double lambda = luaL_checknumber( L, 2 );
         id = kernel->insertObject( new SigmoidActivationFunction( lambda ) );
         break;
         }
      case ACT_FUNC::THSIGMOID:
         {
         id = kernel->insertObject( new ThSigmoidActivationFunction() );
         break;
         }
      default:
         // Do nothing;
         break;
      }

   lua_pushnumber( L, id );
   return 1;
   };


int createProcUnit( lua_State * L )
   {
   KernelObjectId id = 0;

   // Read procUnit argument;
   int procUnit = luaL_checknumber( L, 1 );
   switch ( procUnit )
      {
      case PROC_UNIT::CUSTOM:
         {
         // Read luaFunction argument;
         CustomFunction * luaFunction = new CustomFunction( 2 );

         id = kernel->insertObject( new CustomProcessingUnit( luaFunction ) );
         break;
         }
      case PROC_UNIT::RADIAL_BASIS:
         {
         // Read coeffUsage argument;
         COEFF_USAGE::T_COEFF_USAGE coeffUsage = ( COEFF_USAGE::T_COEFF_USAGE ) luaL_checknumber( L, 2 );

         id = kernel->insertObject( new RadialBasisProcessingUnit( coeffUsage ) );
         break;
         }
      case PROC_UNIT::SCALAR:
         {
         id = kernel->insertObject( new ScalarProcessingUnit() );
         break;
         }
      case PROC_UNIT::WEIGHTED_SUM:
         {
         id = kernel->insertObject( new WeightedSumProcessingUnit() );
         break;
         }
      default:
         // Do nothing;
         break;
      }

   lua_pushnumber( L, id );
   return 1;
   };


int createDistribution( lua_State * L )
   {
   KernelObjectId id = 0;

   // Read distr argument;
   int distr = luaL_checknumber( L, 1 );
   switch ( distr )
      {
      case DISTR::CUSTOM:
         {
         CustomFunction * luaInverseFunction = new CustomFunction( 2 );
         id = kernel->insertObject( new CustomDistribution( luaInverseFunction ) );
         break;
         }
      case DISTR::EXP:
         {
         double lambda = luaL_checknumber( L, 2 );
         id = kernel->insertObject( new ExponentialDistribution( lambda ) );
         break;
         }
      case DISTR::WEIBULL:
         {
         double teta = luaL_checknumber( L, 2 );
         double beta = luaL_checknumber( L, 3 );
         id = kernel->insertObject( new WeibullDistribution( teta, beta ) );
         break;
         }
      default:
         // Do nothing;
         break;
      }

   lua_pushnumber( L, id );
   return 1;
   };


int calcMeanCI( lua_State * L )
   {
   // Read mean argument;
   double mean = luaL_checknumber( L, 1 );

   // Read meansqr argument;
   double meansqr = luaL_checknumber( L, 2 );

   // Read times argument;
   double times = luaL_checkinteger( L, 3 );

   // Read beta argument;
   double beta = luaL_checknumber( L, 4 );

   double t = 0.0;
   if ( fabs( beta - 0.95 ) < 0.0001 ) t = 1.960;
   else if ( fabs( beta - 0.99 ) < 0.0001 ) t = 2.576;
   else if ( fabs( beta - 0.999 ) < 0.0001 ) t = 3.291;

   double delta = t * sqrt( ( meansqr - mean * mean ) / ( times - 1.0 ) );

   lua_pushnumber( L, delta );
   return 1;
   };


int calcACProbabilityCI( lua_State * L )
   {
   // Read p argument;
   double p = luaL_checknumber( L, 1 );

   // Read times argument;
   double times = luaL_checkinteger( L, 2 );

   // Read alpha argument;
   double alpha = luaL_checknumber( L, 3 );

   double x = 0.0;
   if ( fabs( alpha - 0.05 ) < 0.0001 ) x = 1.959964;
   else if ( fabs( alpha - 0.01 ) < 0.0001 ) x = 2.5758293;
   else if ( fabs( alpha - 0.001 ) < 0.0001 ) x = 3.2905267;

   double timesAC = times + x * x;
   double pAC = ( p * times + 0.5 * x * x ) / timesAC;
   double delta = x * sqrt( pAC * ( 1.0 - pAC ) / timesAC );

   lua_pushnumber( L, pAC - delta );
   lua_pushnumber( L, pAC + delta );
   return 2;
   };


/***************************************************************************
 *   Simulation engine API functions implementation                        *
 ***************************************************************************/


int createInterruptManager( lua_State * L )
   {
   KernelObjectId id = 0;

   // Read component argument;
   KernelObjectId componentId = luaL_checkinteger( L, 1 );
   KernelObject * component = kernel->getObject( componentId );

   // Read distribution argument;
   KernelObjectId distributionId = luaL_checkinteger( L, 2 );
   KernelObject * object = kernel->getObject( distributionId );
   Distribution * distribution = dynamic_cast < Distribution * >( object );

   // Read fixFunction argument;
   CustomFunction * fixFunction = NULL;
   if ( ! lua_isnil( L, 3 ) )
      {
      fixFunction = new CustomFunction( 3 );
      }

   // Read intSrc argument;
   AbstractWeights * weights = dynamic_cast < AbstractWeights * >( component );
   if ( weights != NULL )
      {
      id = kernel->insertObject( new AbstractWeightsManager( distribution, weights, fixFunction ) );
      lua_pushnumber( L, id );
      return 1;
      }

   AnalogCapacitors * capacitors = dynamic_cast < AnalogCapacitors * >( component );
   if ( capacitors != NULL )
      {
      id = kernel->insertObject( new AnalogCapacitorsManager( distribution, capacitors, fixFunction ) );
      lua_pushnumber( L, id );
      return 1;
      }

   AnalogResistors * resistors = dynamic_cast < AnalogResistors * >( component );
   if ( resistors != NULL )
      {
      id = kernel->insertObject( new AnalogResistorsManager( distribution, resistors, fixFunction ) );
      lua_pushnumber( L, id );
      return 1;
      }

   MemoryModule * memory = dynamic_cast < MemoryModule * >( component );
   if ( memory != NULL )
      {
      id = kernel->insertObject( new MemoryModuleManager( distribution, memory, fixFunction ) );
      lua_pushnumber( L, id );
      return 1;
      }

   lua_pushnumber( L, id );
   return 1;
   };


int createSimulationEngine( lua_State * L )
   {
   SimulationEngine * engine = new SimulationEngine();
   KernelObjectId id = kernel->insertObject( engine );

   lua_pushnumber( L, id );
   return 1;
   };


int appendInterruptManager( lua_State * L )
   {
   KernelObjectId id = 0;
   KernelObject * object = NULL;

   // Read engine argument;
   KernelObjectId engineId = luaL_checkinteger( L, 1 );
   object = kernel->getObject( engineId );
   SimulationEngine * engine = dynamic_cast < SimulationEngine * >( object );

   // Read manager argument;
   KernelObjectId managerId = luaL_checkinteger( L, 2 );
   object = kernel->getObject( managerId );
   InterruptManager * manager = dynamic_cast < InterruptManager * >( object );

   // Append manager;
   engine->appendManager( manager );

   return 0;
   };


int getIntSourcesCount( lua_State * L )
   {
   // Read manager argument;
   KernelObjectId managerId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( managerId );
   InterruptManager * manager = dynamic_cast < InterruptManager * >( object );

   lua_pushnumber( L, manager->getIntSourcesCount() );
   return 1;
   };


int getInterruptsCount( lua_State * L )
   {
   // Read manager argument;
   KernelObjectId managerId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( managerId );
   InterruptManager * manager = dynamic_cast < InterruptManager * >( object );

   lua_pushnumber( L, manager->getInterruptsCount() );
   return 1;
   };


int simulateInterrupt( lua_State * L )
   {
   // Read manager argument;
   KernelObjectId managerId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( managerId );
   InterruptManager * manager = dynamic_cast < InterruptManager * >( object );

   // Read intSource argument;
   unsigned int intSource = luaL_checkinteger( L, 2 );

   manager->simulateInterrupt( intSource );
   return 0;
   };


int restartEngine( lua_State * L )
   {
   // Read engine argument;
   KernelObjectId engineId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( engineId );
   SimulationEngine * engine = dynamic_cast < SimulationEngine * >( object );

   // Restart engine;
   engine->restart();

   return 0;
   };


int stepOverEngine( lua_State * L )
   {
   // Read engine argument;
   KernelObjectId engineId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( engineId );
   SimulationEngine * engine = dynamic_cast < SimulationEngine * >( object );

   lua_pushboolean( L, engine->stepOver() );
   return 1;
   };


int getCurrentTime( lua_State * L )
   {
   // Read engine argument;
   KernelObjectId engineId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( engineId );
   SimulationEngine * engine = dynamic_cast < SimulationEngine * >( object );

   lua_pushnumber( L, engine->getCurrentTime() );
   return 1;
   };


int getFutureTime( lua_State * L )
   {
   // Read engine argument;
   KernelObjectId engineId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( engineId );
   SimulationEngine * engine = dynamic_cast < SimulationEngine * >( object );

   lua_pushnumber( L, engine->getFutureTime() );
   return 1;
   };


int getCurrentSource( lua_State * L )
   {
   // Read engine argument;
   KernelObjectId engineId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( engineId );
   SimulationEngine * engine = dynamic_cast < SimulationEngine * >( object );

   KernelObjectId managerId = 0;
   int intSource = -1;

   InterruptManager * manager = engine->getCurrentIntSource();
   if ( manager != NULL )
      {
      managerId = kernel->getId( manager );
      intSource = manager->getLastIntSource();
      }

   lua_pushnumber( L, managerId );
   lua_pushnumber( L, intSource );
   return 2;
   };


int getFutureSource( lua_State * L )
   {
   // Read engine argument;
   KernelObjectId engineId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( engineId );
   SimulationEngine * engine = dynamic_cast < SimulationEngine * >( object );

   KernelObjectId managerId = 0;
   int intSource = -1;

   InterruptManager * manager = engine->getFutureIntSource();
   if ( manager != NULL )
      {
      managerId = kernel->getId( manager );
      intSource = manager->getIntSource();
      }

   lua_pushnumber( L, managerId );
   lua_pushnumber( L, intSource );
   return 2;
   };
