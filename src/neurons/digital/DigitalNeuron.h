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


#ifndef DIGITALNEURON_H
#define DIGITALNEURON_H


#include "kernel/KernelObject.h"
#include "components/digital/DigitalConnectors.h"
#include "components/digital/MemoryModule.h"
#include "math/ActivationFunction.h"
#include "math/ProcessingUnit.h"


/***************************************************************************
 *   DigitalNeuron class declaration                                       *
 ***************************************************************************/


class DigitalNeuron : public KernelObject
   {
   public:
      DigitalNeuron(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         DigitalConnectors * connectors,
         unsigned int connectorsBaseIndex,
         MemoryModule * memory,
         unsigned int memoryBaseIndex,
         ProcessingUnit * processingUnit,
         ActivationFunction * activationFunction
         );

      virtual ~DigitalNeuron();

      unsigned int getInputsCount() const;

      void setWeight( unsigned int index, double weight );
      double getWeight( unsigned int index );

      double getOutput();

      void compute();

      void createDampingBuffers();

      void snapDelta( double err );
      double getDelta();
      double getWeightedDelta( unsigned int index );
      void modifyWeights( double damping, double speed );

   protected:
      DigitalNeuron();
      DigitalNeuron( const DigitalNeuron & other );

   private:
      unsigned int inputsCount;
      unsigned int * inputConnectors;

      DigitalConnectors * connectors;
      unsigned int connectorsBaseIndex;

      MemoryModule * memory;
      unsigned int memoryBaseIndex;

      double * builtInBuffers;

      ProcessingUnit * processingUnit;
      ActivationFunction * activationFunction;

      double processingUnitOut;
      double delta;
   };


#endif
