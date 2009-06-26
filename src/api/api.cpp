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


#include "api/api.h"


#include <math.h>
#include <stdio.h>
#include <typeinfo>


#include "kernel/Kernel.h"
#include "components/abstract/AbstractActivators.h"
#include "components/abstract/AbstractAdders.h"
#include "components/abstract/AbstractConnectors.h"
#include "components/abstract/AbstractWeights.h"
#include "neurons/abstract/AbstractNeuron.h"
#include "components/analog/AnalogComparators.h"
#include "components/analog/AnalogResistors.h"
#include "components/analog/AnalogWires.h"
#include "neurons/analog/AnalogNeuron.h"
#include "engine/SimulationEngine.h"
#include "math/Destribution.h"
#include "math/TransferFunction.h"



// It is better for API functions to use this pointer instead of
// Kernel::instance() and Kernel::freeInstance() methods;
extern Kernel * kernel;


void registerApiFunctions( lua_State * L )
   {
   // Register common API functions;
   lua_register( L, "apiVersion", apiVersion );
   lua_register( L, "closeId", closeId );
   lua_register( L, "readArray", readArray );
   // Register abstract neuron API functions;
   lua_register( L, "createAbstractActivators", createAbstractActivators );
   lua_register( L, "createAbstractAdders", createAbstractAdders );
   lua_register( L, "createAbstractConnectors", createAbstractConnectors );
   lua_register( L, "getSignals", getSignals );
   lua_register( L, "setSignals", setSignals );
   lua_register( L, "createAbstractWeights", createAbstractWeights );
   lua_register( L, "getAbstractWeights", getAbstractWeights );
   lua_register( L, "setAbstractWeights", setAbstractWeights );
   lua_register( L, "createAbstractNeuron", createAbstractNeuron );
   lua_register( L, "computeAbstractNeurons", computeAbstractNeurons );
   // Register analog neuron API functions;
   lua_register( L, "createAnalogComparators", createAnalogComparators );
   lua_register( L, "createAnalogResistors", createAnalogResistors );
   lua_register( L, "setupAnalogResistors", setupAnalogResistors );
   lua_register( L, "createAnalogWires", createAnalogWires );
   lua_register( L, "getPotentials", getPotentials );
   lua_register( L, "setPotentials", setPotentials );
   lua_register( L, "createAnalogNeuron", createAnalogNeuron );
   lua_register( L, "computeAnalogNeurons", computeAnalogNeurons );
   // Math API functions;
   lua_register( L, "createLinearActFunc", createLinearActFunc );
   lua_register( L, "createSigmoidActFunc", createSigmoidActFunc );
   // Simulation engine API functions;
   lua_register( L, "createExponentialDestribution", createExponentialDestribution );
   lua_register( L, "createWeibullDestribution", createWeibullDestribution );
   lua_register( L, "createAnalogResistorsManager", createAnalogResistorsManager );
   lua_register( L, "createSimulationEngine", createSimulationEngine );
   lua_register( L, "appendInterruptManager", appendInterruptManager );
   lua_register( L, "stepOverEngine", stepOverEngine );
   lua_register( L, "getCurrentTime", getCurrentTime );
   };


unsigned int readArray( lua_State * L, int index, unsigned int length, unsigned int * array )
   {
   unsigned int i = 0;

   // Push first key;
   lua_pushnil( L );
   while ( lua_next( L, index ) != 0 && i < length )
      {
      // Use 'key' ( at index -2 ) and 'value' ( at index -1 );
      unsigned int key = lua_tointeger( L, -2 );
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
      // Use 'key' ( at index -2 ) and 'value' ( at index -1 );
      unsigned int key = lua_tointeger( L, -2 );
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


int apiVersion( lua_State * L )
   {
   lua_pushstring( L, API_VERSION );
   return 1;
   };


int closeId( lua_State * L )
   {
   kernel->deleteObject( luaL_checknumber( L, 1 ) );
   return 0;
   };


int readArray( lua_State * L )
   {
   printf( "len = %i\n", lua_gettop( L ) );
   lua_pushnil( L );  /* first key */
   while ( lua_next( L, 1 ) != 0 )
      {
      /* uses 'key' ( at index -2 ) and 'value' ( at index -1 ) */
      printf( "arr[ %i ] = %i\n",
         ( int ) lua_tonumber( L, -2 ),
         ( int ) lua_tonumber( L, -1 )
         );

      /* removes 'value'; keeps 'key' for next iteration */
      lua_pop( L, 1 );
      }

   //lua_pop( L, 2 );

   //printf( "len = %i\n", lua_getn( L, -1 ) );
   return 0;
   };


int trainAnalogHopfield( lua_State * L )
   {

   return 0;
   };


/***************************************************************************
 *   Abstract neuron API functions implementation                          *
 ***************************************************************************/


int createAbstractActivators( lua_State * L )
   {
   KernelObjectId id = 0;

   unsigned int count = luaL_checkinteger( L, 1 );
   if ( count > 0 )
      {
      LinearTransferFunction activationFunc( 1.0, 0 );
      AbstractActivators * activators = new AbstractActivators( count, & activationFunc );
      id = kernel->insertObject( activators );
      }

   lua_pushnumber( L, id );
   return 1;
   };


int createAbstractAdders( lua_State * L )
   {
   KernelObjectId id = 0;

   unsigned int count = luaL_checkinteger( L, 1 );
   if ( count > 0 )
      {
      AbstractAdders * adders = new AbstractAdders( count );
      id = kernel->insertObject( adders );
      }

   lua_pushnumber( L, id );
   return 1;
   };


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
      lua_pushnumber( L, i );
      lua_pushnumber( L, connectors->getSignal( baseIndex + i ) );
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

   // Read potentials argument;
   unsigned int limit = connectors->count();
   lua_pushnil( L );
   while ( lua_next( L, 3 ) != 0 )
      {
      unsigned int key = lua_tointeger( L, -2 ) - 1;
      if ( key < limit ) connectors->setSignal( baseIndex + key, lua_tonumber( L, -1 ) );
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
         lua_pushnumber( L, i );
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
         lua_pushnumber( L, i );
         lua_pushnumber( L, weights->getWeight( baseIndex + i ) );
         lua_rawset( L, -3 );
         }
      }

   return 1;
   };


int setAbstractWeights( lua_State * L )
   {
   // Read count argument;
   unsigned int count = luaL_checkinteger( L, 3 );

   // Read weights argument;
   double * w = NULL;
   if ( count > 0 )
      {
      w = new double[ count ];
      readArray( L, 2, count, w );
      }

   // Read neuron argument;
   KernelObjectId neuronId = luaL_checkinteger( L, 1 );
   KernelObject * object = kernel->getObject( neuronId );
   AbstractNeuron * neuron = dynamic_cast < AbstractNeuron * >( object );

   if ( neuron != NULL )
      {
      neuron->setupWeights( w, count );
      }
   else
      {
      AbstractWeights * weights = dynamic_cast < AbstractWeights * >( object );
      weights->setupWeights( w, count );
      }

   if ( w != NULL ) delete[] w;

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

   // Read adders argument;
   KernelObjectId addersId = luaL_checkinteger( L, 7 );
   object = kernel->getObject( addersId );
   AbstractAdders * adders = dynamic_cast < AbstractAdders * >( object );

   // Read addersBaseIndex argument;
   unsigned int addersBaseIndex = luaL_checkinteger( L, 8 );

   // Read activationFunction argument;
   KernelObjectId activationFunctionId = luaL_checkinteger( L, 9 );
   object = kernel->getObject( activationFunctionId );
   TransferFunction * activationFunction = dynamic_cast < TransferFunction * >( object );

   // Create abstract neuron;
   AbstractNeuron * neuron = NULL;
   if ( activationFunction != NULL )
      {
      neuron = new AbstractNeuron(
         inputsCount, inputConnectors,
         connectors, connectorsBaseIndex,
         weights, weightsBaseIndex,
         adders, addersBaseIndex,
         activationFunction
         );
      }
   else
      {
      AbstractActivators * activators = dynamic_cast < AbstractActivators * >( object );

      // Read activatorsBaseIndex argument;
      unsigned int activatorsBaseIndex = luaL_checkinteger( L, 10 );

      neuron = new AbstractNeuron(
         inputsCount, inputConnectors,
         connectors, connectorsBaseIndex,
         weights, weightsBaseIndex,
         adders, addersBaseIndex,
         activators, activatorsBaseIndex
         );
      }

   KernelObjectId id = kernel->insertObject( neuron );

   // Free inputConnectors;
   if ( inputConnectors != NULL ) delete[] inputConnectors;

   lua_pushnumber( L, id );
   return 1;
   };


int computeAbstractNeurons( lua_State * L )
   {
   // Read count argument;
   unsigned int count = luaL_checkinteger( L, 2 );

   // Read neurons argument;
   AbstractNeuron ** neurons = NULL;
   if ( count > 0 )
      {
      KernelObjectId neuronId = 0;
      KernelObject * object = NULL;
      neurons = new AbstractNeuron *[ count ];
      lua_pushnil( L );
      while ( lua_next( L, 1 ) != 0 )
         {
         unsigned int key = lua_tointeger( L, -2 );
         if ( key < count )
            {
            neuronId = lua_tointeger( L, -1 );
            object = kernel->getObject( neuronId );
            neurons[ key ] = dynamic_cast < AbstractNeuron * >( object );
            }

         lua_pop( L, 1 );
         }
      }

   // Read times argument;
   unsigned int times = luaL_checkinteger( L, 3 );

   // Calculate neurons;
   for ( unsigned int i = 0; i < times; i ++ )
      {
      for ( unsigned int j = 0; j < count; j ++ )
         {
         if ( neurons[ j ] != NULL ) neurons[ j ]->compute();
         }
      }

   return 0;
   };


/***************************************************************************
 *   Analog neuron API functions implementation                            *
 ***************************************************************************/


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
         resistors->setResistance( baseIndex + count * k + j, resistance );
         }
      }

   if ( weights != NULL ) delete[] weights;

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
      lua_pushnumber( L, i );
      lua_pushnumber( L, wires->getPotential( baseIndex + i ) );
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
      unsigned int key = lua_tointeger( L, -2 ) - 1;
      if ( key < limit ) wires->setPotential( baseIndex + key, lua_tonumber( L, -1 ) );
      lua_pop( L, 1 );
      }

   return 0;
   };


int createAnalogNeuron( lua_State * L )
   {
   KernelObjectId id = 0;
   KernelObject * object = NULL;

   // Read numInputs argument;
   unsigned int numInputs = luaL_checkinteger( L, 1 );

   // Read inputWires argument;
   unsigned int * inputWires = NULL;
   if ( numInputs > 0 )
      {
      inputWires = new unsigned int[ numInputs ];
      readArray( L, 2, numInputs, inputWires );
      }

   // Read gndWireIndex argument;
   unsigned int gndWireIndex = luaL_checkinteger( L, 3 );

   // Read srcWireIndex argument;
   unsigned int srcWireIndex = luaL_checkinteger( L, 4 );

   // Read comparators argument;
   KernelObjectId comparatorsId = luaL_checkinteger( L, 5 );
   object = kernel->getObject( comparatorsId );
   AnalogComparators * comparators = dynamic_cast < AnalogComparators * >( object );

   // Read comparatorsBaseIndex argument;
   unsigned int comparatorsBaseIndex = luaL_checkinteger( L, 6 );

   // Read resistors argument;
   KernelObjectId resistorsId = luaL_checkinteger( L, 7 );
   object = kernel->getObject( resistorsId );
   AnalogResistors * resistors = dynamic_cast < AnalogResistors * >( object );

   // Read resistorsBaseIndex argument;
   unsigned int resistorsBaseIndex = luaL_checkinteger( L, 8 );

   // Read wires argument;
   KernelObjectId wiresId = luaL_checkinteger( L, 9 );
   object = kernel->getObject( wiresId );
   AnalogWires * wires = dynamic_cast < AnalogWires * >( object );

   // Read wiresBaseIndex argument;
   unsigned int wiresBaseIndex = luaL_checkinteger( L, 10 );

   // Create AnalogNeuron;
   AnalogNeuron * neuron = new AnalogNeuron(
      numInputs,
      inputWires,
      gndWireIndex,
      srcWireIndex,
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
   // Read count argument;
   unsigned int count = luaL_checkinteger( L, 2 );

   // Read neurons argument;
   AnalogNeuron ** neurons = NULL;
   if ( count > 0 )
      {
      KernelObjectId neuronId = 0;
      KernelObject * object = NULL;
      neurons = new AnalogNeuron *[ count ];
      lua_pushnil( L );
      while ( lua_next( L, 1 ) != 0 )
         {
         unsigned int key = lua_tointeger( L, -2 );
         if ( key < count )
            {
            neuronId = lua_tointeger( L, -1 );
            object = kernel->getObject( neuronId );
            neurons[ key ] = dynamic_cast < AnalogNeuron * >( object );
            }

         lua_pop( L, 1 );
         }
      }

   // Read times argument;
   unsigned int times = luaL_checkinteger( L, 3 );

   // Calculate neurons;
   for ( unsigned int i = 0; i < times; i ++ )
      {
      for ( unsigned int j = 0; j < count; j ++ )
         {
         if ( neurons[ j ] != NULL ) neurons[ j ]->compute();
         }
      }

   return 0;
   };


/***************************************************************************
 *   Math API functions implementation                                     *
 ***************************************************************************/


int createLinearActFunc( lua_State * L )
   {
   // Read a argument;
   double a = luaL_checknumber( L, 1 );

   // Read b argument;
   double b = luaL_checknumber( L, 2 );

   LinearTransferFunction * actFunc = new LinearTransferFunction( a, b );
   KernelObjectId id = kernel->insertObject( actFunc );

   lua_pushnumber( L, id );
   return 1;
   };


int createSigmoidActFunc( lua_State * L )
   {
   SigmoidTransferFunction * actFunc = new SigmoidTransferFunction();
   KernelObjectId id = kernel->insertObject( actFunc );

   lua_pushnumber( L, id );
   return 1;
   };


/***************************************************************************
 *   Simulation engine API functions implementation                        *
 ***************************************************************************/


int createExponentialDestribution( lua_State * L )
   {
   KernelObjectId id = 0;

   // Read lambda argument;
   double lambda = luaL_checknumber( L, 1 );

   ExponentialDestribution * destribution = new ExponentialDestribution( lambda );
   id = kernel->insertObject( destribution );

   lua_pushnumber( L, id );
   return 1;
   };


int createWeibullDestribution( lua_State * L )
   {
   KernelObjectId id = 0;

   // Read teta argument;
   double teta = luaL_checknumber( L, 1 );

   // Read beta argument;
   double beta = luaL_checknumber( L, 2 );

   WeibullDestribution * destribution = new WeibullDestribution( teta, beta );
   id = kernel->insertObject( destribution );

   lua_pushnumber( L, id );
   return 1;
   };


int createAnalogResistorsManager( lua_State * L )
   {
   KernelObjectId id = 0;
   KernelObject * object = NULL;

   // Read destribution argument;
   KernelObjectId destributionId = luaL_checkinteger( L, 1 );
   object = kernel->getObject( destributionId );
   Destribution * destribution = dynamic_cast < Destribution * >( object );

   // Read resistors argument;
   KernelObjectId resistorsId = luaL_checkinteger( L, 2 );
   object = kernel->getObject( resistorsId );
   AnalogResistors * resistors = dynamic_cast < AnalogResistors * >( object );

   AnalogResistorsManager * manager = new AnalogResistorsManager( destribution, resistors );
   id = kernel->insertObject( manager );

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
