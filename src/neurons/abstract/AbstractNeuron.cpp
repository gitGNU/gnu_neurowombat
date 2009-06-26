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
   AbstractAdders * adders,
   unsigned int addersBaseIndex,
   TransferFunction * activationFunction
   )
   : KernelObject()
   {
   this->sharedConstructor(
      inputsCount, inputConnectors,
      connectors, connectorsBaseIndex,
      weights, weightsBaseIndex,
      adders, addersBaseIndex
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
   AbstractAdders * adders,
   unsigned int addersBaseIndex,
   AbstractActivators * activators,
   unsigned int activatorsBaseIndex
   )
   : KernelObject()
   {
   this->sharedConstructor(
      inputsCount, inputConnectors,
      connectors, connectorsBaseIndex,
      weights, weightsBaseIndex,
      adders, addersBaseIndex
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

   // Release captured objects;
   if ( connectors != NULL ) connectors->release();
   if ( weights != NULL ) weights->release();
   if ( activationFunction != NULL ) activationFunction->release();
   if ( activators != NULL ) activators->release();
   if ( adders != NULL ) adders->release();
   };


unsigned int AbstractNeuron::getInputsCount() const
   {
   return this->inputsCount;
   };


void AbstractNeuron::setupWeights( double * weights, unsigned int count )
   {
   if ( this->weights == NULL )
      {
      // Setup built-in weights;
      memcpy( this->builtInWeights, weights, count * sizeof( double ) );
      }
   else
      {
      // Setup external weights;
      this->weights->setupWeights( weights, count );
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
      this->weights->getWeight( index );
      }
   };


void AbstractNeuron::compute()
   {
   // Calculate weighted sum of input signals;
   double net = 0.0;
   if ( adders == NULL )
      {
      // Use built-in adder;
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
      }
   else
      {
      // Use external adder;
      adders->resetSignalSum( addersBaseIndex );

      if ( weights == NULL )
         {
         // Use build-in weights;
         for ( unsigned int i = 0; i < inputsCount; i ++ )
            {
            adders->addToSignalSum(
               addersBaseIndex,
               builtInWeights[ i ] *
                  connectors->getSignal( inputConnectors[ i ] )
               );
            }
         }
      else
         {
         // Use external weights;
         for ( unsigned int i = 0; i < inputsCount; i ++ )
            {
            adders->addToSignalSum(
               addersBaseIndex,
               weights->getWeight( weightsBaseIndex + i ) *
                  connectors->getSignal( inputConnectors[ i ] )
               );
            }
         }

      net = adders->getSignalSum( addersBaseIndex );
      }

   // Transfer signal through the activator to neuron's output connector;
   if ( activators == NULL )
      {
      // Use built-in activation function;
      connectors->setSignal(
         connectorsBaseIndex,
         activationFunction->evaluateFunction( net )
         );
      }
   else
      {
      // Use external activator;
      activators->setSignal( activatorsBaseIndex, net );
      connectors->setSignal(
         connectorsBaseIndex,
         activators->getSignal( activatorsBaseIndex )
         );
      }
   };


/*void PseudoAnalogNeuron::trainOnce(
   double * x,
   unsigned int length,
   double y,
   double * e,
   double speed
   )
   {
   * e = y - this->compute( x, length );

   // Net value will be loaded from buffer which stores it till the last forvard propagation;
   this->delta = ( * e );
   double d = speed * this->delta;
   if ( ! this->damping )
      {
      for ( unsigned int i = 0; i < this->numInputs; i ++ )
         {
         this->wbuffer[ i ] = this->w[ i ];
         this->w[ i ] += x[ i ] * d;
         }

      this->t += d;
      }
   };*/


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
   AbstractAdders * adders,
   unsigned int addersBaseIndex
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

   // Setup adders;
   this->adders = adders;
   this->addersBaseIndex = addersBaseIndex;
   if ( adders != NULL ) adders->capture();
   };
