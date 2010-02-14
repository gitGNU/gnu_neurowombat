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


#ifndef PROCESSINGUNIT_H
#define PROCESSINGUNIT_H


#include "kernel/KernelObject.h"
#include "objects/CustomFunction.h"
#include "components/abstract/AbstractConnectors.h"
#include "components/abstract/AbstractWeights.h"
#include "components/digital/DigitalConnectors.h"
#include "components/digital/MemoryModule.h"


/***************************************************************************
 *   T_PROC_UNIT enum declaration                                          *
 ***************************************************************************/

namespace PROC_UNIT
   {
   enum T_PROC_UNIT
      {
      CUSTOM,
      RADIAL_BASIS,
      SCALAR,
      WEIGHTED_SUM
      };
   };


/***************************************************************************
 *   T_COEFF_USAGE enum declaration                                        *
 ***************************************************************************/

namespace COEFF_USAGE
   {
   enum T_COEFF_USAGE
      {
      NOP,
      ADD_TO,
      MUL_BY,
      SUB_IT_FROM,
      SUB_FROM_IT,
      DIV_IT_BY,
      DIV_BY_IT
      };
   };


inline double useCoefficient( COEFF_USAGE::T_COEFF_USAGE coeffUsage, double x, double c );


/***************************************************************************
 *   ProcessingUnit abstract class declaration                             *
 ***************************************************************************/


class ProcessingUnit : public KernelObject
   {
   public:
      ProcessingUnit();
      virtual ~ProcessingUnit();

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         double * builtInWeights,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex
         ) = 0;

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         DigitalConnectors * connectors,
         MemoryModule * memory,
         unsigned int memoryBaseIndex
         ) = 0;
   };


/***************************************************************************
 *   CustomProcessingUnit class declaration                                *
 ***************************************************************************/


class CustomProcessingUnit : public ProcessingUnit
   {
   public:
      CustomProcessingUnit( CustomFunction * processingFunc );
      CustomProcessingUnit( CustomProcessingUnit & other );
      virtual ~CustomProcessingUnit();

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         double * builtInWeights,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex
         );

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         DigitalConnectors * connectors,
         MemoryModule * memory,
         unsigned int memoryBaseIndex
         );

   private:
      CustomFunction * processingFunc;
   };


/***************************************************************************
 *   RadialBasisProcessingUnit class declaration                           *
 ***************************************************************************/


class RadialBasisProcessingUnit : public ProcessingUnit
   {
   public:
      RadialBasisProcessingUnit( COEFF_USAGE::T_COEFF_USAGE coeffUsage );
      virtual ~RadialBasisProcessingUnit();

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         double * builtInWeights,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex
         );

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         DigitalConnectors * connectors,
         MemoryModule * memory,
         unsigned int memoryBaseIndex
         );

   private:
      COEFF_USAGE::T_COEFF_USAGE coeffUsage;
   };


/***************************************************************************
 *   ScalarProcessingUnit class declaration                                *
 ***************************************************************************/


class ScalarProcessingUnit : public ProcessingUnit
   {
   public:
      ScalarProcessingUnit();
      virtual ~ScalarProcessingUnit();

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         double * builtInWeights,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex
         );

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         DigitalConnectors * connectors,
         MemoryModule * memory,
         unsigned int memoryBaseIndex
         );
   };


/***************************************************************************
 *   WeightedSumProcessingUnit class declaration                           *
 ***************************************************************************/


class WeightedSumProcessingUnit : public ProcessingUnit
   {
   public:
      WeightedSumProcessingUnit();
      virtual ~WeightedSumProcessingUnit();

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         double * builtInWeights,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex
         );

      virtual double process(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         DigitalConnectors * connectors,
         MemoryModule * memory,
         unsigned int memoryBaseIndex
         );
   };


#endif
