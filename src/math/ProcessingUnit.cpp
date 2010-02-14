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


#include "math/ProcessingUnit.h"


#include <math.h>
#include <lua.hpp>


#include "kernel/Kernel.h"


// It is better for API functions to use this pointer instead of
// Kernel::instance() and Kernel::freeInstance() methods;
extern Kernel * kernel;


inline double useCoefficient( COEFF_USAGE::T_COEFF_USAGE coeffUsage, double x, double c )
   {
   switch ( coeffUsage )
      {
      case COEFF_USAGE::NOP:
         break;
      case COEFF_USAGE::ADD_TO:
         x += c;
         break;
      case COEFF_USAGE::MUL_BY:
         x *= c;
         break;
      case COEFF_USAGE::SUB_IT_FROM:
         x -= c;
         break;
      case COEFF_USAGE::SUB_FROM_IT:
         x = c - x;
         break;
      case COEFF_USAGE::DIV_IT_BY:
         x = c / x;
         break;
      case COEFF_USAGE::DIV_BY_IT:
         x /= c;
         break;
      }

   return x;
   };


/***************************************************************************
 *   ProcessingUnit abstract class implementation                          *
 ***************************************************************************/


ProcessingUnit::ProcessingUnit()
   : KernelObject()
   {
   // Do nothing;
   };


ProcessingUnit::~ProcessingUnit()
   {
   // Do nothing;
   };


/***************************************************************************
 *   CustomProcessingUnit class implementation                             *
 ***************************************************************************/


CustomProcessingUnit::CustomProcessingUnit( CustomFunction * processingFunc )
   : ProcessingUnit()
   {
   this->processingFunc = processingFunc;
   if ( processingFunc != NULL ) processingFunc->capture();
   };


CustomProcessingUnit::CustomProcessingUnit( CustomProcessingUnit & other )
   : ProcessingUnit( other )
   {
   processingFunc = other.processingFunc;
   if ( processingFunc != NULL ) processingFunc->capture();
   };


CustomProcessingUnit::~CustomProcessingUnit()
   {
   if ( processingFunc != NULL ) processingFunc->release();
   };


double CustomProcessingUnit::process(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   AbstractConnectors * connectors,
   double * builtInWeights,
   AbstractWeights * weights,
   unsigned int weightsBaseIndex
   )
   {
   lua_State * L = kernel->getVM();
   lua_rawgeti( L, LUA_REGISTRYINDEX, processingFunc->getFunctionReference() );

   // Push signals array;
   lua_newtable( L );
   for ( unsigned int i = 0; i < inputsCount; i ++ )
      {
      // Increase key by 1 to provide compatibility between C and Lua-style arrays;
      lua_pushnumber( L, i + 1 );
      lua_pushnumber( L, connectors->at( inputConnectors[ i ] ) );
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
      }
   else
      {
      // Use external weights;
      lua_newtable( L );
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         // Increase key by 1 to provide compatibility between C and Lua-style arrays;
         lua_pushnumber( L, i + 1 );
         lua_pushnumber( L, weights->at( weightsBaseIndex + i ) );
         lua_rawset( L, -3 );
         }
      }

   lua_pcall( L, 2, 1, 0 );
   double result = lua_tonumber( L, -1 );
   lua_pop( L, 1 );
   return result;
   };


double CustomProcessingUnit::process(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   DigitalConnectors * connectors,
   MemoryModule * memory,
   unsigned int memoryBaseIndex
   )
   {
   lua_State * L = kernel->getVM();
   lua_rawgeti( L, LUA_REGISTRYINDEX, processingFunc->getFunctionReference() );

   // Push signals array;
   lua_newtable( L );
   for ( unsigned int i = 0; i < inputsCount; i ++ )
      {
      // Increase key by 1 to provide compatibility between C and Lua-style arrays;
      lua_pushnumber( L, i + 1 );
      lua_pushnumber( L, connectors->at( inputConnectors[ i ] ) );
      lua_rawset( L, -3 );
      }

   // Push weights array;
   lua_newtable( L );
   for ( unsigned int i = 0; i < inputsCount; i ++ )
      {
      // Increase key by 1 to provide compatibility between C and Lua-style arrays;
      lua_pushnumber( L, i + 1 );
      lua_pushnumber( L, memory->at( memoryBaseIndex + i ) );
      lua_rawset( L, -3 );
      }

   lua_pcall( L, 2, 1, 0 );
   double result = lua_tonumber( L, -1 );
   lua_pop( L, 1 );
   return result;
   };


/***************************************************************************
 *   RadialBasisProcessingUnit class implementation                        *
 ***************************************************************************/


RadialBasisProcessingUnit::RadialBasisProcessingUnit( COEFF_USAGE::T_COEFF_USAGE coeffUsage )
   : ProcessingUnit()
   {
   this->coeffUsage = coeffUsage;
   };


RadialBasisProcessingUnit::~RadialBasisProcessingUnit()
   {
   // Do nothing;
   };


double RadialBasisProcessingUnit::process(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   AbstractConnectors * connectors,
   double * builtInWeights,
   AbstractWeights * weights,
   unsigned int weightsBaseIndex
   )
   {
   if ( coeffUsage != COEFF_USAGE::NOP ) inputsCount --;

   // Calculate distance between input signals and weights;
   double dist = 0.0;
   double buffer = 0.0;
   if ( weights == NULL )
      {
      // Use build-in weights;
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         buffer = builtInWeights[ i ] - connectors->at( inputConnectors[ i ] );
         dist += buffer * buffer;
         }

      if ( coeffUsage != COEFF_USAGE::NOP )
         {
         buffer = builtInWeights[ inputsCount ] * connectors->at( inputConnectors[ inputsCount ] );
         }
      }
   else
      {
      // Use external weights;
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         buffer = weights->at( weightsBaseIndex + i ) - connectors->at( inputConnectors[ i ] );
         dist += buffer * buffer;
         }

      if ( coeffUsage != COEFF_USAGE::NOP )
         {
         buffer = weights->at( weightsBaseIndex + inputsCount ) * connectors->at( inputConnectors[ inputsCount ] );
         }
      }

   dist = useCoefficient( coeffUsage, dist, buffer );
   dist = sqrt( dist );

   return dist;
   };


double RadialBasisProcessingUnit::process(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   DigitalConnectors * connectors,
   MemoryModule * memory,
   unsigned int memoryBaseIndex
   )
   {
   if ( coeffUsage != COEFF_USAGE::NOP ) inputsCount --;

   // Calculate distance between input signals and weights;
   double dist = 0.0;
   double buffer = 0.0;
   for ( unsigned int i = 0; i < inputsCount; i ++ )
      {
      buffer = memory->at( memoryBaseIndex + i ) - connectors->at( inputConnectors[ i ] );
      dist += buffer * buffer;
      }

   if ( coeffUsage != COEFF_USAGE::NOP )
      {
      buffer = memory->at( memoryBaseIndex + inputsCount ) * connectors->at( inputConnectors[ inputsCount ] );
      }

   dist = useCoefficient( coeffUsage, dist, buffer );
   dist = sqrt( dist );

   return dist;
   };


/***************************************************************************
 *   ScalarProcessingUnit class implementation                             *
 ***************************************************************************/


ScalarProcessingUnit::ScalarProcessingUnit()
   : ProcessingUnit()
   {
   // Do nothing;
   };


ScalarProcessingUnit::~ScalarProcessingUnit()
   {
   // Do nothing;
   };


double ScalarProcessingUnit::process(
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
         buffer = connectors->at( inputConnectors[ i ] );
         product += builtInWeights[ i ] * buffer;
         sum += buffer;
         }
      }
   else
      {
      // Use external weights;
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         buffer = connectors->at( inputConnectors[ i ] );
         product += weights->at( weightsBaseIndex + i ) * buffer;
         sum += buffer;
         }
      }

   return ( sum != 0.0 ) ? product / sum : 0.0;
   };


double ScalarProcessingUnit::process(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   DigitalConnectors * connectors,
   MemoryModule * memory,
   unsigned int memoryBaseIndex
   )
   {
   // Calculate normalized scalar product of input signals and weights;
   double buffer = 0.0;
   double product = 0.0;
   double sum = 0.0;
   for ( unsigned int i = 0; i < inputsCount; i ++ )
      {
      buffer = connectors->at( inputConnectors[ i ] );
      product += memory->at( memoryBaseIndex + i ) * buffer;
      sum += buffer;
      }

   return ( sum != 0.0 ) ? product / sum : 0.0;
   };


/***************************************************************************
 *   WeightedSumProcessingUnit class implementation                        *
 ***************************************************************************/


WeightedSumProcessingUnit::WeightedSumProcessingUnit()
   : ProcessingUnit()
   {
   // Do nothing
   };


WeightedSumProcessingUnit::~WeightedSumProcessingUnit()
   {
   // Do nothing;
   };


double WeightedSumProcessingUnit::process(
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
            connectors->at( inputConnectors[ i ] );
         }
      }
   else
      {
      // Use external weights;
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         net += weights->at( weightsBaseIndex + i ) * connectors->at( inputConnectors[ i ] );
         }
      }

   return net;
   };


double WeightedSumProcessingUnit::process(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   DigitalConnectors * connectors,
   MemoryModule * memory,
   unsigned int memoryBaseIndex
   )
   {
   // Calculate weighted sum of input signals;
   double net = 0.0;
   for ( unsigned int i = 0; i < inputsCount; i ++ )
      {
      net += memory->at( memoryBaseIndex + i ) * connectors->at( inputConnectors[ i ] );
      }

   return net;
   };
