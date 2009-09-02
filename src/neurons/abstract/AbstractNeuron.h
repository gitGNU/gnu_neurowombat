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


#ifndef ABSTRACTNEURON_H
#define ABSTRACTNEURON_H


#include "exceptions.h"
#include "kernel/KernelObject.h"
#include "components/abstract/AbstractActivators.h"
#include "components/abstract/AbstractAdders.h"
#include "components/abstract/AbstractBuffers.h"
#include "components/abstract/AbstractConnectors.h"
#include "components/abstract/AbstractWeights.h"


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
         AbstractBuffers * buffers,
         unsigned int buffersBaseIndex,
         AbstractAdders * adders,
         unsigned int addersBaseIndex,
         ActivationFunction * activationFunction
         );

      AbstractNeuron(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         unsigned int connectorsBaseIndex,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex,
         AbstractBuffers * buffers,
         unsigned int buffersBaseIndex,
         AbstractAdders * adders,
         unsigned int addersBaseIndex,
         AbstractActivators * activators,
         unsigned int activatorsBaseIndex
         );
      virtual ~AbstractNeuron();

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
      AbstractNeuron();
      AbstractNeuron( const AbstractNeuron & other );


   private:
      inline void sharedConstructor(
         unsigned int inputsCount,
         unsigned int * inputConnectors,
         AbstractConnectors * connectors,
         unsigned int connectorsBaseIndex,
         AbstractWeights * weights,
         unsigned int weightsBaseIndex,
         AbstractBuffers * buffers,
         unsigned int buffersBaseIndex,
         AbstractAdders * adders,
         unsigned int addersBaseIndex
         );

      unsigned int inputsCount;
      unsigned int * inputConnectors;

      AbstractConnectors * connectors;
      unsigned int connectorsBaseIndex;

      double * builtInWeights;
      AbstractWeights * weights;
      unsigned int weightsBaseIndex;

      unsigned int builtInBuffersCount;
      double * builtInBuffers;
      AbstractBuffers * buffers;
      unsigned int buffersBaseIndex;

      ActivationFunction * activationFunction;
      AbstractActivators * activators;
      unsigned int activatorsBaseIndex;

      AbstractAdders * adders;
      unsigned int addersBaseIndex;

      double net;
      double delta;
   };


#endif
