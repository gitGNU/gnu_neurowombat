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


#include "kernel/KernelObject.h"
#include "objects/CustomFunction.h"
#include "components/abstract/AbstractConnectors.h"
#include "components/abstract/AbstractWeights.h"


/***************************************************************************
 *   T_ABSTRACT_PROCESSOR enum declaration                                 *
 ***************************************************************************/

namespace ABSTRACT_PROCESSOR
   {
   enum T_ABSTRACT_PROCESSOR
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
      AbstractCustomProcessor( CustomFunction * processingFunc );
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
      CustomFunction * processingFunc;
   };


/***************************************************************************
 *   AbstractRadialBasisProcessor class declaration                        *
 ***************************************************************************/


class AbstractRadialBasisProcessor : public AbstractProcessor
   {
   public:
      AbstractRadialBasisProcessor( COEFF_USAGE::T_COEFF_USAGE coeffUsage );
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
      COEFF_USAGE::T_COEFF_USAGE coeffUsage;
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
