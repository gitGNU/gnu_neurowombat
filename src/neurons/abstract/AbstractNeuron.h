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


#ifndef ABSTRACTNEURON_H
#define ABSTRACTNEURON_H


#include "exceptions.h"
#include "kernel/KernelObject.h"
#include "components/abstract/AbstractConnectors.h"
#include "components/abstract/AbstractWeights.h"
#include "math/ActivationFunction.h"
#include "math/ProcessingUnit.h"


/***************************************************************************
 *   AbstractNeuronExcp class declaration                                  *
 ***************************************************************************/


namespace NS_ABSTRACTNEURON
   {
   enum EC
      {
      };
   };


class AbstractNeuronExcp : public Exception < NS_ABSTRACTNEURON::EC >
   {
   public:
      AbstractNeuronExcp( NS_ABSTRACTNEURON::EC error );
   };


/***************************************************************************
 *   AbstractNeuron class declaration                                      *
 ***************************************************************************/


class AbstractNeuron : public KernelObject
   {
   public:
      AbstractNeuron(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         unsigned int connectorsBaseIndex,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex,
         ProcessingUnit * processingUnit,
         ActivationFunction * activationFunction
         );

      virtual ~AbstractNeuron();

      unsigned int getInputsCount() const;

      void setWeight( unsigned int index, double weight );
      double getWeight( unsigned int index );

      double getOutput();

      double leftCompute();
      void rightCompute( double processingUnitOut );
      void compute();

      void createDampingBuffers();

      void snapDelta( double err );
      double getDelta();
      double getWeightedDelta( unsigned int index );
      void modifyWeights( double damping, double speed );

   protected:
      AbstractNeuron();
      AbstractNeuron( const AbstractNeuron & other );

   private:
      unsigned int inputsCount;
      unsigned int * inputConnectors;

      AbstractConnectors * connectors;
      unsigned int connectorsBaseIndex;

      double * builtInWeights;
      AbstractWeights * weights;
      unsigned int weightsBaseIndex;

      double * builtInBuffers;

      ProcessingUnit * processingUnit;
      ActivationFunction * activationFunction;

      double processingUnitOut;
      double delta;
   };


#endif
