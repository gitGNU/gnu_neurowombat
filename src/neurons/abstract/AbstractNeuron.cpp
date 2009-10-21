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


#include "neurons/abstract/AbstractNeuron.h"


#include <string.h>


/***************************************************************************
 *   AbstractNeuronExcp class implementation                               *
 ***************************************************************************/


AbstractNeuronExcp::AbstractNeuronExcp( NS_ABSTRACTNEURON::EC error )
   : Exception < NS_ABSTRACTNEURON::EC > ( error )
   {
   // Do nothing;
   };


/***************************************************************************
 *   AbstractNeuron class implementation                                   *
 ***************************************************************************/


AbstractNeuron::AbstractNeuron(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   AbstractConnectors * connectors,
   unsigned int connectorsBaseIndex,
   AbstractWeights * weights,
   unsigned int weightsBaseIndex,
   AbstractBuffers * buffers,
   unsigned int buffersBaseIndex,
   AbstractProcessor * processor,
   ActivationFunction * activationFunction
   )
   : KernelObject()
   {
   this->sharedConstructor(
      inputsCount, inputConnectors,
      connectors, connectorsBaseIndex,
      weights, weightsBaseIndex,
      buffers, buffersBaseIndex,
      processor
      );

   // Setup activationFunction;
   this->activationFunction = activationFunction;
   this->activators = NULL;
   this->activatorsBaseIndex = 0;
   if ( activationFunction != NULL ) activationFunction->capture();
   };


AbstractNeuron::AbstractNeuron(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   AbstractConnectors * connectors,
   unsigned int connectorsBaseIndex,
   AbstractWeights * weights,
   unsigned int weightsBaseIndex,
   AbstractBuffers * buffers,
   unsigned int buffersBaseIndex,
   AbstractProcessor * processor,
   AbstractActivators * activators,
   unsigned int activatorsBaseIndex
   )
   : KernelObject()
   {
   this->sharedConstructor(
      inputsCount, inputConnectors,
      connectors, connectorsBaseIndex,
      weights, weightsBaseIndex,
      buffers, buffersBaseIndex,
      processor
      );

   // Setup activators;
   this->activationFunction = NULL;
   this->activators = activators;
   this->activatorsBaseIndex = activatorsBaseIndex;
   if ( activators != NULL ) activators->capture();
   };


AbstractNeuron::~AbstractNeuron()
   {
   if ( this->inputConnectors != NULL ) delete[] this->inputConnectors;
   if ( this->builtInWeights != NULL ) delete[] this->builtInWeights;
   if ( this->builtInBuffers != NULL ) delete[] this->builtInBuffers;

   // Release captured objects;
   if ( connectors != NULL ) connectors->release();
   if ( weights != NULL ) weights->release();
   if ( buffers != NULL ) buffers->release();
   if ( activationFunction != NULL ) activationFunction->release();
   if ( activators != NULL ) activators->release();
   if ( processor != NULL ) processor->release();
   };


unsigned int AbstractNeuron::getInputsCount() const
   {
   return this->inputsCount;
   };


void AbstractNeuron::setWeight( unsigned int index, double weight )
   {
   if ( this->weights == NULL )
      {
      // Set built-in weight;
      this->builtInWeights[ index ] = weight;
      }
   else
      {
      // Set external weight;
      this->weights->setWeight( weightsBaseIndex + index, weight );
      }
   };


double AbstractNeuron::getWeight( unsigned int index )
   {
   if ( this->weights == NULL )
      {
      // Return built-in weights;
      return this->builtInWeights[ index ];
      }
   else
      {
      // Return external weights;
      return this->weights->getWeight( weightsBaseIndex + index );
      }
   };


double AbstractNeuron::getOutput()
   {
   return connectors->getSignal( connectorsBaseIndex );
   };


void AbstractNeuron::compute()
   {
   // Calculate processor out;
   processorOut = processor->process(
      inputsCount, inputConnectors, connectors,
      builtInWeights, weights, weightsBaseIndex
      );

   // Activation signal through the activator to neuron's output connector;
   if ( activators == NULL )
      {
      // Use built-in activation function;
      connectors->setSignal(
         connectorsBaseIndex,
         activationFunction->evaluateFunction( processorOut )
         );
      }
   else
      {
      // Use external activator;
      activators->setSignal( activatorsBaseIndex, processorOut );
      connectors->setSignal(
         connectorsBaseIndex,
         activators->getSignal( activatorsBaseIndex )
         );
      }
   };


void AbstractNeuron::createDampingBuffers()
   {
   if ( buffers == NULL && builtInBuffersCount != inputsCount )
      {
      if ( builtInBuffers != NULL ) delete[] builtInBuffers;

      builtInBuffers = new double[ inputsCount ];
      builtInBuffersCount = inputsCount;
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         builtInBuffers[ i ] = 0.0;
         };
      };
   };


void AbstractNeuron::snapDelta( double err )
   {
   delta = err;

   if ( activators == NULL )
      {
      // Use built-in activation function;
      delta *= activationFunction->evaluateDerivative( processorOut );
      }
   else
      {
      // Use external activator;
      delta *= activators->evaluateDerivative( processorOut );
      }
   };


double AbstractNeuron::getDelta()
   {
   return delta;
   };


double AbstractNeuron::getWeightedDelta( unsigned int index )
   {
   if ( weights == NULL )
      {
      // Use build-in weights;
      return delta * builtInWeights[ index ];
      }
   else
      {
      // Use external weights;
      return delta * weights->getWeight( weightsBaseIndex + index );
      }
   };


void AbstractNeuron::modifyWeights( double damping, double speed )
   {
   double d = delta * speed;
   double dw = 0;

   if ( weights == NULL )
      {
      // Use build-in weights;
      if ( buffers == NULL )
         {
         // Use built-in buffers;
         for ( unsigned int i = 0; i < inputsCount; i ++ )
            {
            dw = damping * builtInBuffers[ i ] +
               d * connectors->getSignal( inputConnectors[ i ] );
            builtInBuffers[ i ] = dw;
            builtInWeights[ i ] += dw;
            }
         }
      else
         {
         // Use external buffers;
         for ( unsigned int i = 0; i < inputsCount; i ++ )
            {
            dw = damping * buffers->getBuffer( i ) +
               d * connectors->getSignal( inputConnectors[ i ] );
            buffers->setBuffer( i, dw );
            builtInWeights[ i ] += dw;
            }
         }
      }
   else
      {
      // Use external weights;
      if ( buffers == NULL )
         {
         // Use built-in buffers;
         for ( unsigned int i = 0; i < inputsCount; i ++ )
            {
            dw = damping * builtInBuffers[ i ] +
               d * connectors->getSignal( inputConnectors[ i ] );
            builtInBuffers[ i ] = dw;
            weights->incWeight( weightsBaseIndex + i, dw );
            }
         }
      else
         {
         // Use external buffers;
         for ( unsigned int i = 0; i < inputsCount; i ++ )
            {
            dw = damping * buffers->getBuffer( i ) +
               d * connectors->getSignal( inputConnectors[ i ] );
            buffers->setBuffer( i, dw );
            weights->incWeight( weightsBaseIndex + i, dw );
            }
         }
      }
   };


AbstractNeuron::AbstractNeuron()
   : KernelObject()
   {
   // Do nothing;
   };


AbstractNeuron::AbstractNeuron( const AbstractNeuron & other )
   {
   // Do nothing;
   };


inline void AbstractNeuron::sharedConstructor(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   AbstractConnectors * connectors,
   unsigned int connectorsBaseIndex,
   AbstractWeights * weights,
   unsigned int weightsBaseIndex,
   AbstractBuffers * buffers,
   unsigned int buffersBaseIndex,
   AbstractProcessor * processor
   )
   {
   this->inputsCount = inputsCount;

   // Allocate memory for inputConnectors;
   if ( inputsCount > 0 && inputConnectors != NULL )
      {
      this->inputConnectors = new unsigned int[ inputsCount ];

      // Fill inputConnectors;
      if ( this->inputConnectors != NULL )
         {
         memcpy( this->inputConnectors, inputConnectors, inputsCount * sizeof( unsigned int ) );
         }
      }
   else
      {
      this->inputConnectors = NULL;
      }

   // Setup connectors;
   this->connectors = connectors;
   this->connectorsBaseIndex = connectorsBaseIndex;
   if ( connectors != NULL ) connectors->capture();

   // Try to create built-in weights;
   if ( weights == NULL && inputsCount > 0 )
      {
      builtInWeights = new double[ inputsCount ];
      }
   else
      {
      builtInWeights = NULL;
      }

   // Setup weights;
   this->weights = weights;
   this->weightsBaseIndex = weightsBaseIndex;
   if ( weights != NULL ) weights->capture();

   // Setup built-in buffers;
   this->builtInBuffersCount = 0;
   this->builtInBuffers = NULL;

   // Setup buffers;
   this->buffers = buffers;
   this->buffersBaseIndex = buffersBaseIndex;
   if ( buffers != NULL ) buffers->capture();

   // Setup processor;
   this->processor = processor;
   if ( processor != NULL ) processor->capture();

   // Setup processorOut;
   this->processorOut = 0.0;

   // Setup delta;
   this->delta = 0.0;
   };
