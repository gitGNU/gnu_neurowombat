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


#ifndef ABSTRACTPROCESSOR_H
#define ABSTRACTPROCESSOR_H


#include <lua.hpp>


#include "kernel/KernelObject.h"
#include "components/abstract/AbstractConnectors.h"
#include "components/abstract/AbstractWeights.h"


/***************************************************************************
 *   AbstractProcessor abstract class declaration                          *
 ***************************************************************************/


class AbstractProcessor : public KernelObject
   {
   public:
      AbstractProcessor();
      virtual ~AbstractProcessor();

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         double * builtInWeights,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex
         ) = 0;
   };


/***************************************************************************
 *   AbstractCustomProcessor class declaration                             *
 ***************************************************************************/


class AbstractCustomProcessor : public AbstractProcessor
   {
   public:
      AbstractCustomProcessor( lua_State * L, int processRef, bool useMultiplier );
      virtual ~AbstractCustomProcessor();

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         double * builtInWeights,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex
         );

   private:
      lua_State * L;
      int processRef;
      bool useMultiplier;
   };


/***************************************************************************
 *   AbstractRadialBasisProcessor class declaration                        *
 ***************************************************************************/


class AbstractRadialBasisProcessor : public AbstractProcessor
   {
   public:
      AbstractRadialBasisProcessor( bool useMultiplier );
      virtual ~AbstractRadialBasisProcessor();

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         double * builtInWeights,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex
         );

   private:
      bool useMultiplier;
   };


/***************************************************************************
 *   AbstractScalarProcessor class declaration                             *
 ***************************************************************************/


class AbstractScalarProcessor : public AbstractProcessor
   {
   public:
      AbstractScalarProcessor();
      virtual ~AbstractScalarProcessor();

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         double * builtInWeights,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex
         );
   };


/***************************************************************************
 *   AbstractWeightedSumProcessor class declaration                        *
 ***************************************************************************/


class AbstractWeightedSumProcessor : public AbstractProcessor
   {
   public:
      AbstractWeightedSumProcessor();
      virtual ~AbstractWeightedSumProcessor();

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         double * builtInWeights,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex
         );
   };


#endif
