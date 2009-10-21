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


#include "components/abstract/AbstractProcessor.h"


#include <math.h>


/***************************************************************************
 *   AbstractProcessor abstract class implementation                       *
 ***************************************************************************/


AbstractProcessor::AbstractProcessor()
   : KernelObject()
   {
   // Do nothing;
   };


AbstractProcessor::~AbstractProcessor()
   {
   // Do nothing;
   };


/***************************************************************************
 *   AbstractCustomProcessor class implementation                          *
 ***************************************************************************/


AbstractCustomProcessor::AbstractCustomProcessor( lua_State * L, int processRef, bool useMultiplier )
   : AbstractProcessor()
   {
   this->L = L;
   this->processRef = processRef;
   this->useMultiplier = useMultiplier;
   };


AbstractCustomProcessor::~AbstractCustomProcessor()
   {
   luaL_unref( L, LUA_REGISTRYINDEX, processRef );
   };


double AbstractCustomProcessor::process(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   AbstractConnectors * connectors,
   double * builtInWeights,
   AbstractWeights * weights,
   unsigned int weightsBaseIndex
   )
   {
   lua_rawgeti( L, LUA_REGISTRYINDEX, processRef );

   int parametersCount = 2;

   // Push signals array;
   lua_newtable( L );
   for ( unsigned int i = 0; i < inputsCount; i ++ )
      {
      // Increase key by 1 to provide compatibility between C and Lua-style arrays;
      lua_pushnumber( L, i + 1 );
      lua_pushnumber( L, connectors->getSignal( inputConnectors[ i ] ) );
      lua_rawset( L, -3 );
      }

   // Push weights array;
   if ( weights == NULL )
      {
      // Use build-in weights;
      lua_newtable( L );
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         // Increase key by 1 to provide compatibility between C and Lua-style arrays;
         lua_pushnumber( L, i + 1 );
         lua_pushnumber( L, builtInWeights[ i ] );
         lua_rawset( L, -3 );
         }

      if ( useMultiplier )
         {
         lua_pushnumber( L, builtInWeights[ inputsCount ] );
         parametersCount ++;
         }
      }
   else
      {
      // Use external weights;
      lua_newtable( L );
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         // Increase key by 1 to provide compatibility between C and Lua-style arrays;
         lua_pushnumber( L, i + 1 );
         lua_pushnumber( L, weights->getWeight( weightsBaseIndex + i ) );
         lua_rawset( L, -3 );
         }

      if ( useMultiplier )
         {
         lua_pushnumber( L, weights->getWeight( weightsBaseIndex + inputsCount ) );
         parametersCount ++;
         }
      }

   lua_pcall( L, parametersCount, 1, 0 );
   double result = lua_tonumber( L, -1 );
   lua_pop( L, 1 );
   return result;
   };


/***************************************************************************
 *   AbstractRadialBasisProcessor class implementation                     *
 ***************************************************************************/


AbstractRadialBasisProcessor::AbstractRadialBasisProcessor( bool useMultiplier )
   : AbstractProcessor()
   {
   this->useMultiplier = useMultiplier;
   };


AbstractRadialBasisProcessor::~AbstractRadialBasisProcessor()
   {
   // Do nothing;
   };


double AbstractRadialBasisProcessor::process(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   AbstractConnectors * connectors,
   double * builtInWeights,
   AbstractWeights * weights,
   unsigned int weightsBaseIndex
   )
   {
   // Calculate distance between input signals and weights;
   double dist = 0.0;
   double buffer = 0.0;
   if ( weights == NULL )
      {
      // Use build-in weights;
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         buffer = builtInWeights[ i ] -
            connectors->getSignal( inputConnectors[ i ] );

         dist += buffer * buffer;
         }

      if ( useMultiplier )
         {
         buffer = builtInWeights[ inputsCount ];
         dist *= buffer * buffer;
         }
      }
   else
      {
      // Use external weights;
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         buffer = weights->getWeight( weightsBaseIndex + i ) -
            connectors->getSignal( inputConnectors[ i ] );

         dist += buffer * buffer;
         }

      if ( useMultiplier )
         {
         buffer = weights->getWeight( weightsBaseIndex + inputsCount );
         dist *= buffer * buffer;
         }
      }

   return sqrt( dist );
   };


/***************************************************************************
 *   AbstractScalarProcessor class implementation                          *
 ***************************************************************************/


AbstractScalarProcessor::AbstractScalarProcessor()
   : AbstractProcessor()
   {
   // Do nothing;
   };


AbstractScalarProcessor::~AbstractScalarProcessor()
   {
   // Do nothing;
   };


double AbstractScalarProcessor::process(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   AbstractConnectors * connectors,
   double * builtInWeights,
   AbstractWeights * weights,
   unsigned int weightsBaseIndex
   )
   {
   // Calculate normalized scalar product of input signals and weights;
   double buffer = 0.0;
   double product = 0.0;
   double sum = 0.0;
   if ( weights == NULL )
      {
      // Use build-in weights;
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         buffer = connectors->getSignal( inputConnectors[ i ] );
         product += builtInWeights[ i ] * buffer;
         sum += buffer;
         }
      }
   else
      {
      // Use external weights;
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         buffer = connectors->getSignal( inputConnectors[ i ] );
         product += weights->getWeight( weightsBaseIndex + i ) * buffer;
         sum += buffer;
         }
      }

   return ( sum != 0.0 ) ? product / sum : 0.0;
   };


/***************************************************************************
 *   AbstractWeightedSumProcessor class implementation                     *
 ***************************************************************************/


AbstractWeightedSumProcessor::AbstractWeightedSumProcessor()
   : AbstractProcessor()
   {
   // Do nothing
   };


AbstractWeightedSumProcessor::~AbstractWeightedSumProcessor()
   {
   // Do nothing;
   };


double AbstractWeightedSumProcessor::process(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   AbstractConnectors * connectors,
   double * builtInWeights,
   AbstractWeights * weights,
   unsigned int weightsBaseIndex
   )
   {
   // Calculate weighted sum of input signals;
   double net = 0.0;
   if ( weights == NULL )
      {
      // Use build-in weights;
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         net += builtInWeights[ i ] *
            connectors->getSignal( inputConnectors[ i ] );
         }
      }
   else
      {
      // Use external weights;
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         net += weights->getWeight( weightsBaseIndex + i ) *
            connectors->getSignal( inputConnectors[ i ] );
         }
      }

   return net;
   };
